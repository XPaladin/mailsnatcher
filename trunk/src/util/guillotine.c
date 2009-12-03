#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main (int argc, char** argv)
{
    int ifd, ofd;
    ssize_t nread;
    char pkg[65549];
    unsigned short pkglen;
    char hdroffset;
    size_t totread = 0;

    if ((ifd = open(argv[1], O_RDONLY)) == -1) {
        printf("Error accessing file\n");
        exit(1);
    }
    if ((ofd = creat(argv[2], O_RDWR | O_TRUNC | S_IRUSR | S_IWUSR)) == -1) {
        printf("Error creating file\n");
        exit(1);
    }

    while ((nread = read(ifd, pkg, 18)) > 0) {
        totread += nread;
        printf("%08x  ", totread-18);
        hdroffset = (pkg[14] & 0x0f)*4;
        printf("ip hdr=%d  ", hdroffset);
        pkglen = ntohs(*(short*)(pkg+16));
        printf("flags=%x  ", pkg[14+6] & 0xe0);
        if (pkglen == 0) {
            printf("\npkg=0! totread=%08x\n", totread);
            exit(1);
        }
        printf("pkg=%d  ", pkglen);
        nread = read(ifd, pkg+18, pkglen-4);
        totread += nread;
        printf("re %lu  ", nread);
        hdroffset += ((pkg[14+hdroffset+12] >> 4) & 0x0f)*4;
        printf("ip+tcp hdr=%d  ", hdroffset);
        printf("towr=%d\n", pkglen-hdroffset);
        write(ofd, pkg+14+hdroffset, pkglen-hdroffset);
    }

    if (nread == -1) { perror("Error"); }

    close(ifd);
    close(ofd);
    return 0;
}

