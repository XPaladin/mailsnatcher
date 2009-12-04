/**
 * Last change by:  $Author$
 * Last change:     Revision $Rev$, on $Date$
 */

#ifndef SMTP_MESSAGE
#define SMTP_MESSAGE

#include "Message.h"

class SmtpMessage:
    public Message
{
    protected:
    public:
        SmtpMessage (unsigned int serverIp = 0,
                unsigned short serverPort = 0,
                unsigned int clientIp = 0,
                unsigned short clientPort = 0) :
            Message (serverIp, serverPort, clientIp, clientPort)
        {
        }
        ~SmtpMessage () { }

        bool isReady () const;
};

#endif /* SMTP_MESSAGE */

