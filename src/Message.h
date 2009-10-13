/*
 *  Created on: Sep 25, 2009
 *      Author: psepulv
 *
 * Últimos cambios:
 * $Author$
 * $Rev$
 * $Date$
 */

#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdlib>

class Message
{
    protected:
        unsigned int serverIp;
        unsigned short serverPort;
        unsigned int clientIp;
        unsigned short clientPort;

    public:
        Message (unsigned int serverIp = 0,
                unsigned short serverPort = 0,
                unsigned int clientIp = 0,
                unsigned short clientPort = 0) :
            serverIp (serverIp),
            serverPort (serverPort),
            clientIp (clientIp),
            clientPort (clientPort)
            { }
        virtual ~Message () { }

        virtual void append (char* data, size_t len) =0;
        virtual bool isReady () const =0;
        virtual bool isFromServer () const =0;

        unsigned int getServerIp () const { return serverIp; }
        unsigned short getServerPort () const { return serverPort; }
        unsigned int getClientIp () const { return clientIp; }
        unsigned short getClientPort () const { return clientPort; }
};

#endif /* MESSAGE_H */

