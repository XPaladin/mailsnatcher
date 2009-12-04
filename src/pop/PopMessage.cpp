#include "PopMessage.h"

bool PopMessage::isReady () const
{
    if (data.size() > 4) {
        char seq[5] = { '\x0a', '\x0d', '\x2e', '\x0a', '\x0d' };
        std::vector<char>::const_reverse_iterator riter = data.rbegin();
        for (int i=0; i<5; ++i) {
            if (*riter++ != seq[i]) { return false; }
        }
        return true;
    }
    return false;
}

