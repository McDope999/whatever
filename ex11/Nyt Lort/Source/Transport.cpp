#include <iostream>
#include <cstring>
#include <cstdio>
#include "../include/Transport.h"
#include "../include/TransConst.h"


#define DEFAULT_SEQNO 2

namespace Transport
{
    /// <summary>
    /// Initializes a new instance of the <see cref="Transport"/> class.
    /// </summary>
    Transport::Transport (short BUFSIZE)
    {
        link = new Link::Link(BUFSIZE+ACKSIZE);
        checksum = new Checksum();
        buffer = new char[BUFSIZE+ACKSIZE];
        seqNo = 0;
        old_seqNo = DEFAULT_SEQNO;
        errorCount = 0;
        recvSize = 0;
        dataReceived = false;
    }

    /// <summary>
    /// Delete dynamics attribute before delete this object.
    /// </summary>
    Transport::~Transport()
    {
        if(link) delete link;
        if(checksum) delete checksum;
        if(buffer) delete [] buffer;
    }


    /// <summary>
    /// inserts seqNo
    /// inserts type
    /// inserts checksum
    /// </summary>
    void Transport::insertHeader(const char buf[], short size, unsigned char seq, unsigned char type)
    {
        // Debugging
        // std::cout << "insertHeader entered." << std::endl;
        for(int i = 0; i < size; i++)
        {
            buffer[i+4] = buf[i];
        }
        buffer[SEQNO] = seq;
        // Debugging
        // std::cout << seq << " (seq) is inserted at buffer[" << SEQNO << "]" << std::endl;
        buffer[TYPE] = type;
        // Debugging
        // std::cout << type << " (type) is inserted at buffer[" << TYPE << "]" << std::endl;

        checksum->calcChecksum(buffer,size+4);
        // Debugging
        // std::cout << "buffer[0] = " << buffer[0] << std::endl;
        // std::cout << "buffer[1] = " << buffer[1] << std::endl;
        // std::cout << "buffer[2] = " << buffer[2] << std::endl;
        // std::cout << "buffer[3] = " << buffer[3] << std::endl;
        // std::cout << "buffer[4] = " << buffer[4] << std::endl;
        // std::cout << "buffer[5] = " << buffer[5] << std::endl;
        // std::cout << "buffer[6] = " << buffer[6] << std::endl;
        // std::cout << "buffer[7] = " << buffer[7] << std::endl;
        // std::cout << "buffer[8] = " << buffer[8] << std::endl;
        // std::cout << "buffer[9] = " << buffer[9] << std::endl;
    }

    /// <summary>
    /// Receives the ack.
    /// </summary>
    /// <returns>
    /// The ack.
    /// </returns>
    bool Transport::receiveAck()
    {
        recvSize = link->receive(buffer, ACKSIZE);
        dataReceived = true;

        if (recvSize == ACKSIZE)
        {
            if(!checksum->checkChecksum(buffer, ACKSIZE) ||
                buffer[SEQNO] != seqNo ||
                buffer[TYPE] != ACK)
            {
                return false;
            }

            seqNo = ((buffer[SEQNO] + 1) % 2);
        }

        return true;
    }

    /// <summary>
    /// Sends the ack.
    /// </summary>
    /// <param name='ackType'>
    /// Ack type.
    /// </param>
    void Transport::sendAck (bool ackType)
    {
        char ackBuf[ACKSIZE];
        ackBuf [SEQNO] = (ackType ? (buffer [SEQNO] + 1) % 2 : buffer [SEQNO]) ;
        ackBuf [TYPE] = ACK;
        checksum->calcChecksum (ackBuf, ACKSIZE);

        // INSERTED NOISE
        //if(++errorCount == 3) // Simulate noise
        //{
        //    ackBuf[1]++;
        //    std::cout << "Noise! byte #1 is spoiled in the third transmitted ACK-page!" << std::endl;
        //}

        link->send(ackBuf, ACKSIZE);
    }

    /// <summary>
    /// Send the specified buffer and size.
    /// </summary>
    /// <param name='buffer'>
    /// Buffer.
    /// </param>
    /// <param name='size'>
    /// Size.
    /// </param>
    void Transport::send(const char buf[], short size)
    {
        bool receivedAck = false;
        insertHeader(buf, size, seqNo, 0);

         if(++errorCount == 3) // Simulate noise
         {
               buffer[CHKSUMLOW]++; // Important: Only spoil a checksum-field (buffer[0] or buffer[1])
               std::cout << "Noise! - byte #1 is spoiled in the third transmission" << std::endl;
          }

        int attemptCounter = 0; // attempt counter to keep track of times NAK was received and same package is attempted sent
        do{
            if(attemptCounter!=0)
                std::cout << "Attempting to send package again ( Attempt #" << attemptCounter << ") ." << std::endl;
            link->send(buffer, size+4);

            receivedAck = receiveAck();
            old_seqNo = seqNo;

            attemptCounter++;
        }while(receivedAck == false);

        attemptCounter = 0; // resetting attempt counter for next package

        // Debugging
        // std::cout << "buffer after link->send(buffer,size+4)" << std::endl;
        // std::cout << "buffer[4] = " << buffer[4] << std::endl;
        // std::cout << "buffer[5] = " << buffer[5] << std::endl;
        // std::cout << "buffer[6] = " << buffer[6] << std::endl;
        // std::cout << "buffer[7] = " << buffer[7] << std::endl;
        // std::cout << "buffer[8] = " << buffer[8] << std::endl;
        // std::cout << "buffer[9] = " << buffer[9] << std::endl;
        old_seqNo = DEFAULT_SEQNO;

        //link->send(buf, size);
        // receiveAck(); ack/nack modtages
        //  check ack/nack:
        //  if ack return
        //  else send igen

    }

    /// <summary>
    /// Receive the specified buffer.
    /// </summary>
    /// <param name='buffer'>
    /// Buffer.
    /// </param>
    short Transport::receive(char buf[], short size)
    {
        bool checkCheck;
        int sizeOfMsg;

        do{
            sizeOfMsg = link->receive(buffer, NULL);
            checkCheck = checksum->checkChecksum(buffer, sizeOfMsg);
            seqNo = buffer[SEQNO];

            if(checkCheck == true && old_seqNo != seqNo)
            {
                sendAck(false);
            }
            else
            {
                sendAck(true);
            }
        }while((checkCheck == false) || (old_seqNo == buffer[SEQNO]));


        for(int i = 0; i < sizeOfMsg-4; i++)
        {
            buf[i] = buffer[i+4];
        }
        old_seqNo = buffer[SEQNO];
        return sizeOfMsg-4;

        // Debugging
        // std::cout << "buffer after link->receive(buffer,NULL)" << std::endl;
        // std::cout << "buffer[4] = " << buffer[4] << std::endl;
        // std::cout << "buffer[5] = " << buffer[5] << std::endl;
        // std::cout << "buffer[6] = " << buffer[6] << std::endl;
        // std::cout << "buffer[7] = " << buffer[7] << std::endl;
        // std::cout << "buffer[8] = " << buffer[8] << std::endl;
        // std::cout << "buffer[9] = " << buffer[9] << std::endl;
     return sizeOfMsg;

       //  receive from link layer
       //  check checksum
       //  send ack/nack
       //  check seqNr
       //  if seqNr = new -> send til applikationslag
       //  else -> ignore
    }
}
