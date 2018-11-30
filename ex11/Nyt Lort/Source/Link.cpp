#include <cstdio>
#include "../include/Link.h"
#include <iostream>

#define BUFSIZE 1000

namespace Link {

/**
  * Explicitet constructor for linklaget.
  * Åbner den serielle port og sætter baudraten, paritet og antal stopbits
  */
Link::Link(int bufsize)
{
    buffer = new char[(bufsize*2)];

    serialPort=v24OpenPort("/dev/ttyS1",V24_STANDARD);
    if ( serialPort==NULL )
    {
        fputs("error: sorry, open failed!\n",stderr);
        exit(1);
    }

    int rc=v24SetParameters(serialPort,V24_B115200,V24_8BIT,V24_NONE);
    if ( rc!=V24_E_OK )
    {
        fputs("error: setup of the port failed!\n",stderr);
        v24ClosePort(serialPort);
        exit(1);
    }

    // Uncomment following lines to use timeout
/*    rc=v24SetTimeouts(serialPort,5);
    if ( rc!=V24_E_OK )
    {
        fputs("error: setup of the port timeout failed!\n",stderr);
        v24ClosePort(serialPort);
        exit(1);
    }

    rc=v24FlushRxQueue(serialPort);
    if ( rc!= V24_E_OK )
    {
        fputs("error: flushing receiverqueue\n", stderr);
        v24ClosePort(serialPort);
        exit(1);
    }

    rc=v24FlushTxQueue(serialPort);
    if ( rc!= V24_E_OK )
    {
        fputs("error: flushing transmitterqueue\n", stderr);
        v24ClosePort(serialPort);
        exit(1);
    }
*/
}

/**
  * Destructor for linklaget.
  * Lukker den serielle port
  */
Link::~Link()
{
    if(serialPort != NULL)
        v24ClosePort(serialPort);
    if(buffer != NULL)
        delete [] buffer;
}

/**
 * Bytestuffer buffer, dog maximalt det antal som angives af size
 * Herefter sendes de til modtageren via den serielle port
 *
 * @param buffer De bytes som skal sendes
 * @param size Antal bytes der skal sendes
 */
void Link::send(const char buf[], short size)
{
    char encodedBuffer[BUFSIZE];
    //char* encodedBufferPtr = encodedBuffer;

    short sizeOfenc = encodeMessage(encodedBuffer, buf, size);

    v24Write(serialPort,(unsigned char*)encodedBuffer, sizeOfenc);
}

/**
 * Modtager data fra den serielle port og debytestuffer data og gemmer disse i buffer.
 * Modtager højst det antal bytes som angives af size.
 * Husk kun at læse en byte ad gangen fra den serielle port.
 *
 * @param buffer De bytes som modtages efter debytestuffing
 * @param size Antal bytes der maximalt kan modtages
 * @return Antal bytes modtaget
 */
short Link::receive(char buf[], short size) // Ignore short size
{
    char receivedChar = NULL;
    int i = 0;

    do{
        receivedChar = v24Getc(serialPort);
    }
    while(receivedChar != 'A'); // Check if received == 'A'

    buffer[i++] = 'A';
    //std::cout << 'A' << std::endl;

    // When 'A' is received, fill buffer with message.
    do{
    receivedChar = v24Getc(serialPort);
    //std::cout << receivedChar << std::endl;
    buffer[i++] = receivedChar;
    }while(receivedChar != 'A'); // Stop at next 'A'

    short returnValue = decodeMessage(buf, buffer, i);
    return returnValue; // Decode message from buffer to buf. Message is i long.
}

short Link::encodeMessage(char* encodedBuffer, const char buf[], short size)
{
    short charCounter = 0;
    encodedBuffer[charCounter++] = 'A';	// Inserting delimiter at beginning of message

    for(int i = 0; i < (int)size; i++)
    {
        if(buf[i] == 'A')
            {
                encodedBuffer[charCounter++] = 'B';
                encodedBuffer[charCounter++] = 'C';
            }
        else if(buf[i] == 'B')
        {
            encodedBuffer[charCounter++] = 'B';
            encodedBuffer[charCounter++] = 'D';
        }
        else
        {
            encodedBuffer[charCounter++] = buf[i];
        }
    }

    encodedBuffer[charCounter++] = 'A';	// Inserting delimiter at end of message

    return charCounter;
}

short Link::decodeMessage(char* buf, const char encodedBuffer[], short size)
{
    short charCounter = 0;

    for(int i = 0; i < size; i++)
    {
        if(encodedBuffer[i] == 'A')
        {}
        else if(encodedBuffer[i] == 'B')
        {
            i++;
            if(encodedBuffer[i] == 'C')
                buf[charCounter++] = 'A';
            else if(encodedBuffer[i] == 'D')
                buf[charCounter++] = 'B';
        }
        else
            buf[charCounter++] = encodedBuffer[i];
    }

    return charCounter;
}

} /* namespace Link */
