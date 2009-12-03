#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFSIZE 512

size_t cur_chunk_siz = 0;
size_t cur_chunk_wrt = 0;

void dechunk (int fd, const char* buf, size_t bufsize)
{
    size_t to_write;
    if (cur_chunk_siz == 0) {
        char* endptr;
        cur_chunk_siz = strtoul(buf, &endptr, 16);
        cur_chunk_wrt = 0;
        bufsize -= ((endptr-buf) + 2);
        buf += (endptr - buf) + 2;
    }
    to_write = (cur_chunk_siz - cur_chunk_writ) > bufsize ?
        bufsize : (cur_chunk_siz - cur_chunk_writ);
    if (to_write == bufsize) {
        write(fd, buf, to_write);
    } else {
        write(fd, buf, to_write);
    }
}

int main (int argc, char** argv)
{
    int ifd, ofd;
    size_t nread, i;
    char buf[BUFSIZE];
    char match[4] = { 0x00, 0x00, 0x00, 0x00 };
    unsigned char dechunking = 0;

    if ((ifd = open(argv[1], O_RDONLY)) < 0) {
        perror("Error opening file");
        exit(1);
    }
    if ((ofd = creat(argv[2], O_RDWR | S_IRUSR | S_IWUSR)) < 0) {
        perror("Error creating file");
        exit(1);
    }

    while ((nread = read(ifd, buf, BUFSIZE)) > 0) {
        for (i=0; !dechunking && i<nread; ++i) {
            match[3] = buf[i];
            if (match[0] == '\x0d' && match[1] == '\x0a' &&
                    match[2] == '\x0d' && match[3] == '\x0a') {
                size_t j;
                dechunking = 1;
                if (i < nread-1) {
                    for (j=++i; j<nread; ++j) { buf[j-i] = buf[j]; }
                } else {
                    i=0;
                }
                nread = read(ifd, buf+i, BUFSIZE-i);
                break;
            }
            match[0] = match[1];
            match[1] = match[2];
            match[2] = match[3];
        }

        if (dechunking) { dechunk(ofd, buf, nread); }
    }

    if (nread == -1) { perror("Error"); exit(1); }

    return 0;
}

