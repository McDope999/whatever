//============================================================================
// Name        : file_server.cpp
// Author      : Lars Mortensen
// Version     : 1.0
// Description : file_server in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <iknlib.h>

#define MAX_BYTES 1000

using namespace std;

void sendFile(char* fileName, long fileSize, int outToClient);

/**
 * main starter serveren og venter på en forbindelse fra en klient
 * Læser filnavn som kommer fra klienten.
 * Undersøger om filens findes på serveren.
 * Sender filstørrelsen tilbage til klienten (0 = Filens findes ikke)
 * Hvis filen findes sendes den nu til klienten
 *
 * HUSK at lukke forbindelsen til klienten og filen nÃ¥r denne er sendt til klienten
 *
 * @throws IOException
 *
 */

//Parameters for main needs to be set in run settings during debugging -
//or alternatively provided when running the program from the terminal.
int main(int argc, char *argv[])
{
    int sockfd, newsockfd, //File descriptors used for listening, binding, reading, and writing to sockets.
        portno; //Stores port number
    socklen_t clilen; //[Client_length] Used to store size of client address. Datatype from the sys/socket.h library.
    char buffer[256]; //Char array used to store data/messages in and out from client.
    struct sockaddr_in serv_addr, cli_addr; //Stores the internet adress of client and server. Datatype from netinet/in.h

    // The user needs to pass in the port number on which the server will accept connections as an argument.
    //This code displays an error message if the user fails to do this.
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    //The socket system call creates a new socket.
    //First argument = socket domain -> AF_INET = Internet Domain.
    //Second argument = Type of socket -> SOCK_STREAM = TCP [SOCK_DGRAM = UDP]
    //Third argument = Protocol -> 0 = TCP (0 chooses the most appropriate protocol).
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) //Bad file descriptor means an error occured when opening the socket.
       error("ERROR opening socket");

    //-------------------Socket-options reuse socket--------------------------------------
    //-----Allows for faster reconnection to socket which eases the debugging process-----
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        error("setsockopt(SO_REUSEADDR) failed");
    //------------------------------------------------------------------------------------

    bzero((char *) &serv_addr, sizeof(serv_addr)); //Sets all values of server address to zero. First argument = pointer to buffer. Second argument = size of buffer.
    portno = atoi(argv[1]); //Convertion of port number from ascii to integer. (Param in main)
    serv_addr.sin_family = AF_INET; //Serv_addr is a struct of type sockaddr_in. .sin_family should allways be AF_INET.
    serv_addr.sin_addr.s_addr = INADDR_ANY; //s_addr = host IP Adress. INADDR_ANY aotomatically assigns the addres on which the application is running.
    serv_addr.sin_port = htons(portno); //Converts port number in host byte order => network byte order.

    //Binds socket to address. In this case the addres of the current host and port number n which the server will run.
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
             error("ERROR on binding");

    while(1) //Server should be iterative.
    {

    listen(sockfd,5); //ALLOWS the proccess to listen on the socket for connextions. Maximum 5 connecetions in backlog queue.
    clilen = sizeof(cli_addr);

    //Blocks until a client connects to the server.
    //Returns a new file descriptor and all communication on this connection should be done using the new file descriptor.
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0)
         error("ERROR on accept");

    //----------------------------------Client communication------------------------------
    bzero(buffer,256); //Resets buffer.

    readTextTCP(buffer,255,newsockfd); //

    printf("Here is the message: %s\n",buffer); //Displaying message for debugging.

    //2) CHECK IF FILE EXISTS AND CHECK FILE SIZE
    long fileSize = check_File_Exists(buffer);
    cout << "filesize is: " << fileSize <<endl;

    //3 SEND FILE:
    sendFile(buffer,fileSize,newsockfd);

    close(newsockfd);

    } //End of while(1).

    close(sockfd);
    return 0;
}

/**
 * Sender filen som har navnet fileName til klienten
 *
 * @param fileName Filnavn som skal sendes til klienten
 * @param fileSize Størrelsen på filen, 0 hvis den ikke findes
 * @param outToClient Stream som der skrives til socket
     */

void sendFile(char* fileName, long fileSize, int outToClient)
{
    if(fileSize<=0) //File not found
        writeTextTCP(outToClient,"0");

    else
    {
        //1)Sending file-size as text
        char fileSizeASCII[256];
        sprintf(fileSizeASCII,"%ld",fileSize);
        writeTextTCP(outToClient,fileSizeASCII);

        //Declaration of variables to handle file transfer
        char fileTransmitBuffer[MAX_BYTES]; //MAX_BYTES defined as 1000.
        size_t remainder = fileSize;
        size_t bytesTransmitted;
        int fp; //File-pointer used to open the specified file

        //Opening file as read only
        fp = open(fileName, O_RDONLY);

        cout << "Sending file..." << endl;
        while(remainder>0)
        {
            bytesTransmitted = read(fp, fileTransmitBuffer, MAX_BYTES);
            cout << "Bytes transmitted: " << bytesTransmitted << endl;
            write(outToClient,fileTransmitBuffer, bytesTransmitted);
            remainder -= bytesTransmitted;
            cout << "Bytes remaining: " << remainder << endl;
        }
        close(fp);
    }
}














