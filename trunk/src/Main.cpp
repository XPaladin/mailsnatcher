#include <pcap.h>
#include <stdlib.h>
#include <iostream>
#include "MessageManager.h"
#include "ConversationManager.h"

#define Ethernet_len 14

short Server_ports[] = {110};

bool is_server(short port){
	int i;
	for(i=0; i<110 ; i++)
	{
		if(port==Server_ports[i])return true;
	}
	return false;
}

int main(int argc, char *argv[])
{
    pcap_t *handle;                 /* Session handle */
    char *dev=NULL;                      /* The device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];  /* Error string */
    struct bpf_program fp;          /* The compiled filter */
    char filter_exp[] = "port 110"; /* The filter expression */
    bpf_u_int32 mask;               /* Our netmask */
    bpf_u_int32 net;                /* Our IP */
    struct pcap_pkthdr header;      /* The header that pcap gives us */
    const u_char *packet;           /* The actual packet */

    /* Define the device */

	pcap_if_t *interfaces;

	if (pcap_findalldevs(&interfaces, errbuf) != 0 || interfaces == NULL ) {
		fprintf(stderr, "Couldn't find any suitable device: %s\n", errbuf);
		return(2);
	}
	unsigned int i=0;
	unsigned int if_n;
	if(argc>1){
		if_n= atoi(argv[1]);

		for(i=0;i<if_n;i++){
			interfaces=interfaces->next;
		}
		if(interfaces!=0){
			dev=interfaces->name;
		}
	}
	else{
		std::cout<< "usage: " << argv[0] <<" n\nWhere n is:"  <<  std::endl;
		do{

			std::cout<< i <<":" << interfaces->name <<  std::endl;
			interfaces=interfaces->next;
			i++;
		}while(interfaces->next!=0);
		return 0;
	}

    /* Find the properties for the device */
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
            fprintf(stderr, "Couldn't get netmask for device %s: %s\n", dev, errbuf);
            net = 0;
            mask = 0;
    }
    std::cout<< "Listening on interface" <<":" << interfaces->name << std::endl;
    /* Open the session in promiscuous mode */
    handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
 //   handle = pcap_open_offline("../etc/pop3.out", errbuf);
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
    MessageManager *msgMan = new MessageManager();
    ConversationManager *convMan = new ConversationManager();
    /* Grab a packet */
    while(1){
        size_t offset=Ethernet_len;
        do{
            packet = pcap_next(handle, &header);
        }while(packet==NULL);
        /* Print its length */
        //printf("Jacked a packet with length of [%d]\n", header.len);
        size_t i=0;
        char ip_len = packet[offset]&0x0f;
        //printf("ip len=%x\n",(int)ip_len);
	for(i=0;i<8;i++){
		iphosts[7-i]=packet[offset+i+12];
	}
	src_ip=((int*)iphosts)[1];
	dst_ip=((int*)iphosts)[0];
	printf("src_ip=%x dst_ip=%x\n",src_ip,dst_ip);
	offset+=ip_len*4;
	for(i=0;i<4;i++){
		ports[3-i]=packet[offset+i];
	}
	src_port=((short*)ports)[1];
	dst_port=((short*)ports)[0];

	printf("src_port=%u dst_port=%hu\n",src_port,dst_port);
	//printf("src=%x dst=%x\n",src_port,dst_port);
	unsigned int server_ip, client_ip;
	unsigned short server_port, client_port;
	if(is_server(dst_port)){
		server_ip = dst_ip;
		client_ip = src_ip;
		server_port = dst_port;
		client_port = src_port;
	}else if( is_server(src_port) ){
		server_ip = dst_ip;
		client_ip = src_ip;
		server_port = dst_port;
		client_port = src_port;
	}else{
		fprintf(stderr, "Protocolo no entendido\n");
		continue;
	}
	char tcp_len = packet[offset+12]>>4;
	int mf=(packet[offset+3]>>5)&0x01;
	offset+=tcp_len*4;

	if(offset<header.caplen){
		msgMan->insert(client_ip, client_port, server_ip, server_port,
			&(packet[offset]),	header.caplen-offset);
		Message *msg=msgMan->get(client_ip, client_port, 
								server_ip, server_port);
		if(msg == NULL){
			fprintf(stderr, "Mensaje perdido\n");
			continue;
		}
		if(!mf && src_port==server_port && msg->isReady() ){
			msg = msgMan->remove(client_ip, client_port, 
								server_ip, server_port);
			if(msg == NULL){
				fprintf(stderr, "Mensaje perdido\n");
				continue;
			}
			convMan->insert(msg);
		}
	}


	for(i=offset; i<header.caplen; i++){
		if(isprint(packet[i]) )
			printf("%c",packet[i]);
		else
			if(0x0d==packet[i]){
				i++;
				printf("\n");
			}
	//		else printf("%x",packet[i]);

//		if(i%4==0)printf(" ");

	}
	printf("\n");
	/* And close the session */
    }
    pcap_close(handle);
    return(0);
}

