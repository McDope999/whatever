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
        for(int i = 0; i < size; i++)
        {
            buffer[i+4] = buf[i];
        }
        buffer[SEQNO] = seqNo;
        buffer[TYPE] = type;
        checksum->calcChecksum(buffer,size+4);
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
/*
        // INSERTED NOISE
        if(++errorCount == 3) // Simulate noise
        {
            ackBuf[1]++;
            std::cout << "Noise! byte #1 is spoiled in the third transmitted ACK-page!" << std::endl;
        }
*/
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
        bool receivedAck;
        insertHeader(buf, size, seqNo, 0);
        /*
        if(++errorCount == 3) // Simulate noise
        {
             buffer[CHKSUMLOW]++; // Important: Only spoil a checksum-field (buffer[0] or buffer[1])
             std::cout << "Noise! - byte #1 is spoiled in the third transmission" << std::endl;
        }
        */
        do{
            link->send(buffer, size+4);
            receivedAck = receiveAck();
        }while(receivedAck == false);

        old_seqNo = DEFAULT_SEQNO;
        /* link->send(buf, size);
         * receiveAck(); ack/nack modtages
         * check ack/nack:
         * if ack return
         * else send igen
        */
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
        seqNo = buffer[SEQNO];
        checkCheck = checksum->checkChecksum(buffer, sizeOfMsg);

        sizeOfMsg-=4;
        memcpy(buffer, buffer+4, sizeOfMsg);  // Deleting the size and type slots

        if(checkCheck)
            sendAck(true);
        else
            sendAck(false);
        }while(checkCheck == false || old_seqNo == seqNo);

        if(buffer[SEQNO] != old_seqNo)
            {
                for(int i = 0; i < sizeOfMsg; i++)    // -2 because of checksum, type and seqNo-array slots
                {
                    buf[i] = buffer[i];               // +2 because of type and seqNo-array slots
                }
                old_seqNo = buffer[SEQNO];
            }
     return sizeOfMsg;

        /* receive from link layer
         * check checksum
         * send ack/nack
         * check seqNr
         * if seqNr = new -> send til applikationslag
         * else -> ignore */
    }
}
