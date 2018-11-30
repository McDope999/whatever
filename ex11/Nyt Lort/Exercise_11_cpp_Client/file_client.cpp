#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include "../include/Transport.h"
#include "../include/lib.h"
#include "file_client.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>

/// <summary>
/// The BUFSIZE
/// </summary>
#define BUFSIZE 1000
#define MAX_BYTES 1000

/// <summary>
/// Initializes a new instance of the <see cref="file_client"/> class.
///
/// file_client metoden opretter en peer-to-peer forbindelse
/// Sender en forspørgsel for en bestemt fil om denne findes på serveren
/// Modtager filen hvis denne findes eller en besked om at den ikke findes (jvf. protokol beskrivelse)
/// Lukker alle streams og den modtagede fil
/// Udskriver en fejl-meddelelse hvis ikke antal argumenter er rigtige
/// </summary>
/// <param name='args'>
/// Filnavn med evtuelle sti.
/// </param>
file_client::file_client(void)
{
    char buffer[256];

    // Creating Transport object
    Transport::Transport trans(MAX_BYTES);

    //-----------------Retrieving file name from client user-------------------
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    buffer[strlen(buffer)-1] = 0; //Removing enter from buffer.
    //-------------------------------------------------------------------------

    //Writing message to server.
    trans.send(buffer, strlen(buffer));

    char fileName[256];
    strcpy(fileName,extractFileName(buffer).c_str()); //Using library function extractfilename to get filename.
    receiveFile(fileName,&trans); //Receiving file from server.
}

/// <summary>
/// Receives the file.
/// </summary>
/// <param name='fileName'>
/// File name.
/// </param>
/// <param name='transport'>
/// Transportlaget
/// </param>
void file_client::receiveFile(char* fileName, Transport::Transport* trans)
{
    //Declaration of variables to handle file transfer.
    char readFileBuffer[MAX_BYTES];
    int fd, n;

    //Retrieving file-size using library function.
    long fileSize = trans->receive(readFileBuffer,0);
    std::cout << "File size is: " << fileSize << std::endl; //Outputting file-size for debugging.

    if(fileSize==0)
        error("File does not exist...");

    char fileLocation[256] = "/root/IKN/filesFromServer/"; //Directory in which files should be located.
    strcat(fileLocation,fileName); //Caternating filename to path.

    std::cout << fileLocation << std::endl; //Outputting file location for debugging purposes

    //Opening file which can then be accessed using filedescriptor fd
    //Using write-only, create, append, and trunctuate flags
    fd = open(fileLocation, O_WRONLY | O_CREAT | O_APPEND | O_TRUNC);

    while(fileSize != 0) //FileSize is decremented during file transfer. Break while loop when completely transferred.
        {

        std::cout << "fileSize remaining:" << fileSize << std::endl; //Output for debugging purposes

                n = trans->receive(readFileBuffer,0); //Reading from server 1000 bytes allowed.
                if (n < 0)
                    error("ERROR reading from socket");

                write(fd, readFileBuffer, n); //Writing received data to the file using file descreiptor
                fileSize -=n; //Reduce filesize by the number of bytes received
        }

    close(fd); //Closing file:

    std::cout << "FILE TRANSFER COMPLETED SUCCESSFULLY" << std::endl;
}

/// <summary>
/// The entry point of the program, where the program control starts and ends.
/// </summary>
/// <param name='args'>
/// First argument: Filname
/// </param>
int main(int argc, char** argv)
{
    new file_client();

    return 0;
}
