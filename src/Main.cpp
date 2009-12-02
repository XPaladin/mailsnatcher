#include <pcap.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <arpa/inet.h>
#include "MessageManager.h"
#include "ConversationManager.h"

#define Ethernet_len 14

short Server_ports[] = {110,80};

bool is_server(short port){
	int i;
	for(i=0; i<2 ; i++)
	{
		if(port==Server_ports[i])return true;
	}
	return false;
}
int processPacket(const u_char *packet, MessageManager *msgMan, ConversationManager *convMan ){
   	unsigned short src_port, dst_port;
	unsigned int src_ip, dst_ip;
    size_t offset=Ethernet_len;
  	char iphosts[8];
   	char ports[4];
   	char *bytes;
    size_t i=0;
    char ip_len = packet[offset]&0x0f;
	u_short len=ntohs(*(short*)(packet+offset+2))+14;
	
    //printf("len=%d caplen=%d, len=%d\n",len-offset, header.caplen,header.len);
	for(i=0;i<8;i++){
		iphosts[7-i]=packet[offset+i+12];
	}
	src_ip=((int*)iphosts)[1];
	dst_ip=((int*)iphosts)[0];
	//printf("src_ip=%x dst_ip=%x\n",src_ip,dst_ip);
	offset+=ip_len*4;
	for(i=0;i<4;i++){
		ports[3-i]=packet[offset+i];
	}
	src_port=((short*)ports)[1];
	dst_port=((short*)ports)[0];

	//printf("src_port=%u dst_port=%hu\n",src_port,dst_port);
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
		fprintf(stderr, "Protocolo no entendido:%d,%d,%x,%x\n",dst_port,src_port,dst_port,src_port);
		return -1;
	}
	char tcp_len = packet[offset+12]>>4;
	int mf=(packet[offset+3]>>5)&0x01;
	offset+=tcp_len*4;

	if(offset<len){
		msgMan->insert(client_ip, client_port, server_ip, server_port,
			&(packet[offset]), len-offset);
		Message *msg=msgMan->get(client_ip, client_port, 
								server_ip, server_port);
		if(msg == NULL){
			fprintf(stderr, "Mensaje perdido1\n");
			return -2;
		}
		if(mf)printf("Moar fragments!!\n");
		if(!mf && src_port==server_port && msg->isReady() ){
			msg = msgMan->remove(client_ip, client_port, 
								server_ip, server_port);
			if(msg == NULL){
				fprintf(stderr, "Mensaje perdido2\n");
				return -2;
			}
			bytes= msg->getBytes();
			for(i=0;i<msg->getLength();i++){
				printf("%c",bytes[i]);
			}			
			convMan->insert(msg);
		}
	}


	for(i=offset; i<len; i++){
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
	return 0;
}
int main(int argc, char *argv[])
{
    pcap_t *handle;                 /* Session handle */
    char *dev=NULL;                      /* The device to sniff on */
    char errbuf[PCAP_ERRBUF_SIZE];  /* Error string */
    struct bpf_program fp;          /* The compiled filter */
    char filter_exp[] = "port 110 or port 80"; /* The filter expression */
    bpf_u_int32 mask;               /* Our netmask */
    bpf_u_int32 net;                /* Our IP */
    struct pcap_pkthdr header;      /* The header that pcap gives us */
    u_char *packet;           /* The actual packet */
    u_char packet2[65535+14];           /* The actual packet */
    FILE *archivo =NULL;
    /* Define the device */

	pcap_if_t *interfaces;

	if (pcap_findalldevs(&interfaces, errbuf) != 0 || interfaces == NULL ) {
		if(argc<=1 || !(archivo= fopen(argv[1],"r")) ){
			fprintf(stderr, "Couldn't find any suitable device: %s, or file\n", errbuf);
			return(2);
		}
	}
	if(archivo==NULL){
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
    }
    MessageManager *msgMan = new MessageManager();
    ConversationManager *convMan = new ConversationManager();
    printf("Recibiendo...\n");
	/* Grab packets */
	//FILE *asdad=fopen("hotmail.out","w");
    while(1){
		if(archivo!=NULL){
			if(fread(packet2,sizeof(char),Ethernet_len+4, archivo)<Ethernet_len+4)break;
			u_short len=ntohs(*(short*)(packet2+Ethernet_len+2));
			printf("len=%d\n",len);
			fread((packet2+Ethernet_len+4),sizeof(char),len-4,archivo);
			processPacket(packet2, msgMan, convMan);
        }else{
			 do{
            packet =(u_char*) pcap_next(handle, &header);
        	}while(packet==NULL);
			//fwrite(packet,sizeof(char),header.len,asdad);
			processPacket(packet, msgMan, convMan);
		}
    }
	if(archivo==NULL){
        /* And close the session */
	    pcap_close(handle);
	}
    return(0);
}

