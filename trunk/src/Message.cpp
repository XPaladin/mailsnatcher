/*
 *  Created on: Sep 25, 2009
 *      Author: psepulv
 *
 * Últimos cambios:
 * $Author$
 * $Rev$
 */

#include "Message.h"

char* Message::getBytes () const
{
    if (bytes == NULL) {
        int i = 0;
        bytes = new char[data.size()];
        for (std::vector<char>::const_iterator iter = data.begin();
                iter != data.end(); ++iter) { bytes[i++] = *iter; }
    }
    return bytes;
}

