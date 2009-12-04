#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <sstream>
#include "dechunk.h"

using namespace std;

bool dechunk (const vector<char>& vec, stringstream& ss)
{
    char crlf[2] = { 0x0d, 0x0a };
    char byte;
    size_t chunk_lft = 0;
    size_t siz = 0;
    vector<char>::const_iterator iter = vec.begin();

    ss.str("");

    while (iter != vec.end()) {
        if (chunk_lft == 0) {
            if (crlf[0] == 0x0d && crlf[1] == 0x0a) {
                char* endptr;
                char num[64];
                unsigned char idx = 0;
                while ((iter) != vec.end()) {
                    byte = *iter++;
                    printf("%02x ", byte);
                    num[idx++] = byte;
                    crlf[0] = crlf[1];
                    crlf[1] = byte;
                    if (crlf[0] == 0x0d && crlf[1] == 0x0a) { break; }
                }
                printf("\n");
                chunk_lft = strtoul(num, &endptr, 16);
                printf("chunk_lft=%lu\n", chunk_lft);
                if (endptr == num) { return false; }
                if (chunk_lft == 0) { printf("siz=%lu\n", siz); return true; }
                siz += chunk_lft;
                crlf[0] = crlf[1] = 0x00;
            } else {
                byte = *iter;
                if ((++iter) == vec.end()) { break; }
                crlf[0] = crlf[1];
                crlf[1] = byte;
            }
        } else {
            byte = *iter++;
            chunk_lft--;
            ss << byte;
        }
    }
    return false;
}

void gunzip (stringstream& ss)
{
    char byte;
    int fd;
    size_t writ=0;

    fd = creat("/tmp/mailsnatcher-chunk.gz", O_RDWR | S_IRUSR | S_IWUSR |
            O_TRUNC);
    if (fd == -1) { perror("gunzip"); exit(1); }
    writ = write(fd, ss.str().c_str(), ss.str().size());
    close(fd);

    printf("writ=%lu\n", writ);

    ss.str("");

    system("chmod 600 /tmp/mailsnatcher-chunk.gz");
    system("gunzip /tmp/mailsnatcher-chunk.gz");

    fd = open("/tmp/mailsnatcher-chunk", O_RDONLY);
    if (fd == -1) { perror("gunzip"); exit(1); }
    while (read(fd, &byte, 1) == 1) { ss << byte; }
    close(fd);
}

/*
int main (int argc, char** argv)
{
    vector<char> vec;
    stringstream ss;
    char buf[1024];
    ssize_t rd;

    int fd = open("/tmp/chunked.out", O_RDONLY);
    while ((rd = read(fd, buf, 1024)) > 0) {
        for (int i=0; i<rd; ++i) { vec.push_back(buf[i]); }
    }

    if (!dechunk(vec, ss)) { printf("Error\n"); exit(1); }
    gunzip(ss);

    return 0;
}
*/

