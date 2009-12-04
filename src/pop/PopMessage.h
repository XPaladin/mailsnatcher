/**
 * Last change by:  $Author$
 * Last change:     Revision $Rev$, on $Date$
 */

#ifndef POP_MESSAGE
#define POP_MESSAGE

#include "Message.h"

class PopMessage:
    public Message
{
    protected:
    public:
        PopMessage (unsigned int serverIp = 0,
                unsigned short serverPort = 0,
                unsigned int clientIp = 0,
                unsigned short clientPort = 0) :
            Message (serverIp, serverPort, clientIp, clientPort)
        {
        }
        ~PopMessage () { }

        bool isReady () const;
};

#endif /* POP_MESSAGE */

