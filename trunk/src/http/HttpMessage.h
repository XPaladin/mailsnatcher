/**
 * Last change by:  $Author$
 * Last change:     Revision $Rev$, on $Date$
 */

#ifndef HTTP_MESSAGE
#define HTTP_MESSAGE

#include "Message.h"
#include <string>
#include <map>

class HttpMessage:
    public Message
{
    protected:
        bool ready;
        bool headersReady;
        int ipx;
        std::map<std::string,std::string> headerTable;
        std::vector<char> body;
    public:
        HttpMessage (unsigned int serverIp = 0,
                unsigned short serverPort = 0,
                unsigned int clientIp = 0,
                unsigned short clientPort = 0) :
            Message (serverIp, serverPort, clientIp, clientPort)
        {   
            ipx=0;
            ready=false;
            headersReady=false;
        }
        virtual bool isReady() const;
        void process();
        virtual void append (const unsigned char* cdata, 
			const size_t len);
        ~HttpMessage () { }

};

#endif /* HTTP_MESSAGE */

