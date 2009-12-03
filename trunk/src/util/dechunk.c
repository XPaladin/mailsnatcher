#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 512

int main (int argc, char** argv)
{
    int ifd, ofd;
    char crlf[2] = { 0x0d, 0x0a };
    char byte;
    size_t chunk_lft = 0;

    if ((ifd = open(argv[1], O_RDONLY)) < 0) {
        perror("Error opening file");
        exit(1);
    }
    if ((ofd = creat(argv[2], O_RDWR | S_IRUSR | S_IWUSR)) < 0) {
        perror("Error creating file");
        exit(1);
    }

    while (1) {
        if (chunk_lft == 0) {
            if (crlf[0] == 0x0d && crlf[1] == 0x0a) {
                char* endptr;
                char num[64];
                unsigned char idx = 0;
                while (read(ifd, &byte, 1) == 1) {
                    printf("%02x ", byte);
                    num[idx++] = byte;
                    crlf[0] = crlf[1];
                    crlf[1] = byte;
                    if (crlf[0] == 0x0d && crlf[1] == 0x0a) { break; }
                }
                printf("\n");
                chunk_lft = strtoul(num, &endptr, 16);
                printf("chunk_lft=%lu\n", chunk_lft);
                crlf[0] = crlf[1] = 0x00;
                if (num == endptr) { printf("Error\n"); exit(1); }
            } else {
                if (read(ifd, &byte, 1) < 1) { break; }
                crlf[0] = crlf[1];
                crlf[1] = byte;
            }
        } else {
            read(ifd, &byte, 1);
            chunk_lft -= write(ofd, &byte, 1);
        }
    }

    return 0;
}

