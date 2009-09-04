#include <pcap.h>
#include <stdio.h>
#define Ethernet_len 14

int main(int argc, char *argv[])
{
    pcap_t *handle;                 /* Session handle */
    char *dev;                      /* The device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];  /* Error string */
    struct bpf_program fp;          /* The compiled filter */
    char filter_exp[] = "port 110"; /* The filter expression */
    bpf_u_int32 mask;               /* Our netmask */
    bpf_u_int32 net;                /* Our IP */
    struct pcap_pkthdr header;      /* The header that pcap gives us */
    const u_char *packet;           /* The actual packet */

    /* Define the device */
    dev = pcap_lookupdev(errbuf);
    dev = "wlan0";
    if (dev == NULL) {
            fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
            return(2);
    }
    /* Find the properties for the device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
            fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
            net = 0;
            mask = 0;
    }
    /* Open the session in promiscuous mode */
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if (handle == NULL) {
            fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
            return(2);
    }
    /* Compile and apply the filter */
    if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
            fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
            return(2);
    }
    if (pcap_setfilter(handle, &fp) == -1) {
            fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
            return(2);
    }
    unsigned short src_port, dst_port;
    unsigned int src_ip, dst_ip;
    char iphosts[8];
    char ports[4];
    /* Grab a packet */
    while(1){
        size_t offset=Ethernet_len;
        do{
            packet = pcap_next(handle, &header);
        }while(packet==NULL);
        /* Print its length */
        //printf("Jacked a packet with length of [%d]\n", header.len);
        int i=0;
        char ip_len = packet[offset]&0x0f;
        //printf("ip len=%x\n",(int)ip_len);
	for(i=0;i<8;i++){
		iphosts[7-i]=packet[offset+i+12];
	}
	src_ip=((int*)iphosts)[1];
	dst_ip=((int*)iphosts)[0];
        printf("src=%x dst=%x\n",src_ip,dst_ip);
        offset+=ip_len*4;
	for(i=0;i<4;i++){
		ports[3-i]=packet[offset+i];
	}
	src_port=((short*)ports)[1];
	dst_port=((short*)ports)[0];
        printf("src=%u dst=%hu\n",src_port,dst_port);
        printf("src=%x dst=%x\n",src_port,dst_port);
        char tcp_len = packet[offset+12]>>4;
        //printf("tcp len=%x\n",(int)tcp_len);
        int mf=(packet[offset+3]>>5)&0x01;
        offset+=tcp_len*4;
        if(offset<header.len && !mf)
            printf("\n");
        
        for(i=offset; i<header.len; i++){
            if(isprint(packet[i]) )
                printf("%c",packet[i]);
            else
                if(0x0d==packet[i]){
                    i++;
                    printf("\n");
                }
//                printf("%.",packet[i]);
            
            //if(i%4==0)printf(" ");

        }
        //printf("\n");
       /* And close the session */
    }
    pcap_close(handle);
    return(0);
}

