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

#include <list>

class Message
{
    protected:
        std::list<char> data;
        unsigned int serverIp;
        unsigned short serverPort;
        unsigned int clientIp;
        unsigned short clientPort;

        mutable char* bytes;

    public:
        Message (unsigned int serverIp = 0,
                unsigned short serverPort = 0,
                unsigned int clientIp = 0,
                unsigned short clientPort = 0) :
            serverIp (serverIp),
            serverPort (serverPort),
            clientIp (clientIp),
            clientPort (clientPort),
            bytes (NULL)
            { }
        virtual ~Message ()
        {
            if (bytes != NULL) { delete[](bytes); }
        }

        virtual void append (const unsigned char* cdata, 
			const size_t len)
        {
            for (size_t i=0; i<len; ++i) { this->data.push_back(cdata[i]); }
        }

        virtual bool isReady () const =0;
        virtual bool isFromServer () const =0;

        size_t getLength () const { return data.size(); }
        char* getBytes () const;

        unsigned int getServerIp () const { return serverIp; }
        unsigned short getServerPort () const { return serverPort; }
        unsigned int getClientIp () const { return clientIp; }
        unsigned short getClientPort () const { return clientPort; }
};

#endif /* MESSAGE_H */

