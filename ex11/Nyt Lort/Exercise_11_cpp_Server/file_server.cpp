#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include "../include/Transport.h"
#include "../include/lib.h"
#include "file_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>



/// <summary>
/// The BUFSIZE
/// </summary>
#define BUFSIZE 1000
#define MAX_BYTES 1000

/// <summary>
/// Initializes a new instance of the <see cref="file_server"/> class.
/// </summary>
file_server::file_server ()
{
    char buffer[256]; //Char array used to store data/messages in and out from client.
    Transport::Transport trans(MAX_BYTES);
    while(1) //Server should be iterative.
    {
    //----------------------------------Client communication------------------------------
    bzero(buffer,256); //Resets buffer.

    trans.receive(buffer,0);

    printf("Here is the message: %s\n",buffer); //Displaying message for debugging.

    //2) CHECK IF FILE EXISTS AND CHECK FILE SIZE
    long fileSize = check_File_Exists(buffer);
    std::cout << "filesize is: " << fileSize << std::endl;

    //3 SEND FILE:
        sendFile(buffer,fileSize,&trans);

    } //End of while(1).
}

/// <summary>
/// Sends the file.
/// </summary>
/// <param name='fileName'>
/// File name.
/// </param>
/// <param name='fileSize'>
/// File size.
/// </param>
/// <param name='transport'>
/// Transport lag.
/// </param>
void file_server::sendFile(std::string fileName, long fileSize, Transport::Transport *transport)
{
    if(fileSize<=0) //File not found
        transport->send("0", 1);

    else
    {
        //1)Sending file-size as text
        char fileSizeASCII[256];
        sprintf(fileSizeASCII,"%ld",fileSize);
        //strcat(fileSizeASCII,"ABC");
        transport->send(fileSizeASCII,strlen(fileSizeASCII)+1);
        std::cout << "Size of what is sent: " << strlen(fileSizeASCII)+1 << std::endl;
        std::cout << "What is sent: " << fileSizeASCII << std::endl;

        //Declaration of variables to handle file transfer
        char fileTransmitBuffer[MAX_BYTES]; //MAX_BYTES defined as 1000.
        size_t remainder = fileSize;
        size_t bytesTransmitted;
        int fp; //File-pointer used to open the specified file


        // DER ER NOGET GALT HERTIL....


        //Opening file as read only
        fp = open(fileName.c_str(), O_RDONLY);

        std::cout << "Sending file..." << std::endl;
        while(remainder>0)
        {
            bytesTransmitted = read(fp, fileTransmitBuffer, MAX_BYTES);
            std::cout << "Bytes transmitted: " << bytesTransmitted << std::endl;
            transport->send(fileTransmitBuffer, bytesTransmitted);
            remainder -= bytesTransmitted;
            std::cout << "Bytes remaining: " << remainder << std::endl;
        }
        close(fp);

    }
}

/// <summary>
/// The entry point of the program, where the program control starts and ends.
/// </summary>
/// <param name='args'>
/// The command-line arguments.
/// </param>
int main(int argc, char **argv)
{
	new file_server();
	
	return 0;
}
