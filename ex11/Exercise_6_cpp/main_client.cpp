//============================================================================
// Name        : file_client.cpp
// Author      : Lars Mortensen
// Version     : 1.0
// Description : file_client in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <iknlib.h>

#define MAX_BYTES 1000

using namespace std;

void receiveFile(char* fileName, int socketfd);

//Parameters for main needs to be set in run settings during debugging -
//or alternatively provided when running the program from the terminal.
int main(int argc, char *argv[])
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);

    //The socket system call creates a new socket.
    //First argument = socket domain -> AF_INET = Internet Domain.
    //Second argument = Type of socket -> SOCK_STREAM = TCP [SOCK_DGRAM = UDP]
    //Third argument = Protocol -> 0 = TCP (0 chooses the most appropriate protocol).
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    //Retrieving server parameters using function gethostbyname.
    //Server variable is a struct of type hostent.
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); //Sets all values of server address to zero. First argument = pointer to buffer. Second argument = size of buffer.
    serv_addr.sin_family = AF_INET; //Serv_addr is a struct of type sockaddr_in. .sin_family should allways be AF_INET.
    //Copying server addres length from 2nd to 1st parameter.
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    serv_addr.sin_port = htons(portno); //Converts port number in host byte order => network byte order.

    //Connect function establishes connection to the server. It takes the socked file descriptor as argument
    //as well as the server address and the size of this address.
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    //-----------------Retrieving file name from client user-------------------
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    buffer[strlen(buffer)-1] = 0; //Removing enter from buffer.
    //-------------------------------------------------------------------------

    //Writing message to server.
    writeTextTCP(sockfd,buffer);

    char fileName[256];
    strcpy(fileName,extractFileName(buffer)); //Using library function extractfilename to get filename.
    receiveFile(fileName,sockfd); //Receiving file from server.

    close(sockfd);
    return 0;
}


/**
 * Modtager filstørrelsen og udskriver meddelelsen: "Filen findes ikke" hvis størrelsen = 0
 * ellers
 * Åbnes filen for at skrive de bytes som senere modtages fra serveren (HUSK kun selve filnavn)
 * Modtag og gem filen i blokke af 1000 bytes indtil alle bytes er modtaget.
 * Luk filen, samt input output streams
 *
 * @param fileName Det fulde filnavn incl. evt. stinavn
 * @param sockfd Stream for at skrive til/læse fra serveren
 */
void receiveFile(char* fileName, int sockfd)
{
    //Declaration of variables to handle file transfer.
    char readFileBuffer[MAX_BYTES];
    int fd, n;

    //Retrieving file-size using library function.
    long fileSize = getFileSizeTCP(sockfd);
    cout << "File size is: " << fileSize << endl; //Outputting file-size for debugging.

    if(fileSize==0)
        error("File does not exist...");

    char fileLocation[256] = "/root/IKN/filesFromServer/"; //Directory in which files should be located.
    strcat(fileLocation,fileName); //Caternating filename to path.

    cout << fileLocation << endl; //Outputting file location for debugging purposes

    //Opening file which can then be accessed using filedescriptor fd
    //Using write-only, create, append, and trunctuate flags
    fd = open(fileLocation, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC);

    while(fileSize != 0) //FileSize is decremented during file transfer. Break while loop when completely transferred.
        {

        cout << "fileSize remaining:" << fileSize << endl; //Output for debugging purposes

                n = read(sockfd,readFileBuffer,1000); //Reading from server 1000 bytes allowed.
                if (n < 0)
                    error("ERROR reading from socket");

                write(fd, readFileBuffer, n); //Writing received data to the file using file descreiptor
                fileSize -=n; //Reduce filesize by the number of bytes received
        }

    close(fd); //Closing file:

    cout << "FILE TRANSFER COMPLETED SUCCESSFULLY" << endl;

}

