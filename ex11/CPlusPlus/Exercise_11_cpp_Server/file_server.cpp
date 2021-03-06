#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include "../include/Transport.h"
#include "../include/lib.h"
#include "file_server.h"

/// <summary>
/// The BUFSIZE
/// </summary>
#define BUFSIZE 1000

/// <summary>
/// Initializes a new instance of the <see cref="file_server"/> class.
/// </summary>
file_server::file_server ()
{
	char testBuffer[BUFSIZE];
    Transport::Transport Trans(BUFSIZE);
    std::cout << "Server open. Waiting for 'A'..." << std::endl;
    long int numberOfChars = Trans.receive(testBuffer, NULL);

    for(int y = 0; y < numberOfChars; y++)
       std::cout << testBuffer[y];
    std::cout << std::endl;

    std::cout << "Number of Chars received: " << numberOfChars << std::endl;
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
	// To do Your own code
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
