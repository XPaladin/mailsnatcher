/**
 * Last change by:  $Author$
 * Last change:     Revision $Rev$, on $Date$
 *
 */

#include <iostream>
#include "PopMessage.h"
#include "util.h"

using namespace std;

void test_message ();
void test_base64 ();

int main (int argc, char* argv[])
{
    //test_message();
    test_base64();
    return 0;
}

void test_message ()
{
    unsigned int serverIp = 0xc0a8000f; // 192.168.1.15
    unsigned short serverPort = 0x006e; // 110
    unsigned int clientIp = 0xc0a800c8; // 192.168.1.200
    unsigned short clientPort = 0x324d; // 12877

    PopMessage m(serverIp, serverPort, clientIp, clientPort);
    serverIp = m.getServerIp();
    serverPort = m.getServerPort();
    clientIp = m.getClientIp();
    clientPort = m.getClientPort();
    cout << "Server " << util::ip_addr_to_string(serverIp) <<
        ":" << serverPort << endl; 
    cout << "Client " << util::ip_addr_to_string(clientIp) <<
        ":" << clientPort << endl; 
}

void test_base64 ()
{
    char txt[6] = "sure.";
    char src[6];
    char dst[9];

    for (size_t i=0; i<6; ++i) { src[i] = txt[i]; }

    dst[8] = 0x00;

    for (size_t i=5; i>1; --i) {
        for (size_t j=0; j<6; ++j) { dst[j] = 0x00; }
        cout << "wrote " << util::enc_base64(src, 0, i, dst, 0, 8) << endl;
        cout << src << " -> " << dst << endl;

        for (size_t j=0; j<6; ++j) { src[j] = 0x00; }
        cout << "wrote " <<
            util::dec_base64(dst, 0, 4*((i-1)/3) + 4, src, 0, 5) << endl;
        cout << dst << " -> " << src << endl;
    }

}

