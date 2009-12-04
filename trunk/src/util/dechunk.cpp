#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

bool dechunk (const vector<char>& vec, stringstream& ss)
{
    char crlf[2] = { 0x0d, 0x0a };
    char byte;
    size_t chunk_lft = 0;
    vector<char>::const_iterator iter = vec.begin();

    ss.str("");

    while (iter != vec.end()) {
        if (chunk_lft == 0) {
            if (crlf[0] == 0x0d && crlf[1] == 0x0a) {
                char* endptr;
                char num[64];
                unsigned char idx = 0;
                byte = *iter;
                while ((++iter) != vec.end()) {
                    /* printf("%02x ", byte) */;
                    num[idx++] = byte;
                    crlf[0] = crlf[1];
                    crlf[1] = byte;
                    if (crlf[0] == 0x0d && crlf[1] == 0x0a) { break; }
                }
                /* printf("\n") */;
                chunk_lft = strtoul(num, &endptr, 16);
                /* printf("chunk_lft=%lu\n", chunk_lft) */;
                if (endptr == num) { return false; }
                if (chunk_lft == 0) { return true; }
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

    fd = creat("/tmp/mailsnatcher-chunk", O_RDWR | S_IRUSR | S_IWUSR);
    if (fd == -1) { perror("gunzip"); exit(1); }
    while (1) {
        ss >> byte;
        if (ss.eof()) { break; }
        write(fd, &byte, 1);
    }
    close(fd);

    ss.str("");

    fd = open("/tmp/mailsnatcher-chunk", O_RDONLY);
    if (fd == -1) { perror("gunzip"); exit(1); }
    while (read(fd, &byte, 1) == 1) { ss << byte; }
    close(fd);
}

