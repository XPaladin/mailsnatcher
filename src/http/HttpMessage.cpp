#include "HttpMessage.h"
#include <stdlib.h>
#include <sstream>
#include "dechunk.h"

bool HttpMessage::isReady () const
{
/*    if (data.size() > 4) {
        char seq[5] = { '\x0a', '\x0d', '\x2e', '\x0a', '\x0d' };
        std::list<char>::const_reverse_iterator riter = data.rbegin();
        for (int i=0; i<5; ++i) {
            if (*riter++ != seq[i]) { return false; }
        }
        return true;
    }
*/    return ready;
}

void HttpMessage::append (const unsigned char* cdata, 
			const size_t len)
{   
    Message::append(cdata,len);
    process(); 
}
void HttpMessage::process()
{
    if(data.size()<8){
        return;
    }
    bool fromServer=false;
    char seq[7] = { 'H', 'T', 'T', 'P','/','1','.' };
    int i;
    /*for(i=0;i<data.size();i++){
        printf("%c",data[i]);
    }
    */
    for(i=data.size()-7;i>=0;i--){
        int j;
        for(j=0;j<7;j++){
            if(data[i+j]!=seq[j]){
                fromServer=false;
                break;
            }
        }
        if(j==7)fromServer=true;
        if(fromServer)break;
    }

    if(fromServer)printf("FROM SERVER\n");
    else return;
    for(int j=0; j<7;j++){
        printf("%c",data[j]);
    }
    printf("\n");
    if(i>0){
        printf("Borrando todo lo anterior\n");
        body.clear();
        headerTable.clear();
        std::vector<char> aux;
        for(int j=0;i+j<data.size();j++){
            aux.push_back(data[i+j]);
        }
    
        data.clear();
        for(int j=0;j<aux.size();j++){
            data.push_back(aux[j]);
        }
        headersReady=false;
    }
    if(data.size()<8){
        return;
    }
    while(data[i]!='\x0d'&& data[i+1]!='\x0a'){
        i++;
    }
    i+=2;
    
    for(int j=i-2; j<i;j++){
        printf("%02x ",data[j]);
    }
    printf("\n");
    for(int j=i; j<i+10;j++){
        printf("%c",data[j]);
    }
    printf("\n");
    if(!headersReady){
        printf("Headers no listos\n");
        printf("i=%d,sze=%d\n",i,data.size());
        for(;!headersReady && i<data.size()-1;){
            std::ostringstream hbuffer, buffer;
            while( i<data.size() && data[i]!=':' ){
               // printf("%02x %02x\n",data[i],data[i+1]);
                if(data[i]=='\x0d' && data[i+1]=='\x0a'){
                    headersReady=true;
                    printf("Headers listos\n");
                    i+=2;
                    break;
                }
                hbuffer<<data[i++];
            }
            if(!headersReady){
                printf("Buscando lado derecho\n");
                i++;
                while(i<data.size()-1 && !(data[i]=='\x0d' && 
data[i+1]=='\x0a')){
                    buffer<<data[i++];
                }
            
                i+=2;
                headerTable[hbuffer.str()]=buffer.str();
                printf("%s->%s\n", hbuffer.str().c_str(),buffer.str().c_str());
            }
        }
    }
    if(headersReady){
        headersReady=false;
        stringstream ss;
        printf("i=%d,size=%d\n",i,data.size());
        int k=i;
        printf("START\n");
        for(;i<data.size();i++){
            body.push_back(data[i]);
            if (body[i-k] >= ' ' && body[i-k] <= '~') printf("%c",body[i-k]);
            else printf(".");
        }
        printf("\nEND\n");
        std::map<std::string,std::string>::iterator 
        iter=headerTable.find(std::string("Content-Type"));
        if(iter!=headerTable.end()){
            if(!iter->second.compare(std::string(" text/html")) || !iter->second.compare(std::string(
    "application/x-javascript; charset=utf-8"))){
            printf("%s: no es text/html ni javascript!\n",iter->second.c_str());
            return;
            }
        }else{
            printf("no tiene Content-Type!\n");
            //return;
        }
        iter=headerTable.find(std::string("Transfer-Encoding"));
        if(iter!=headerTable.end()){
            printf("Hay Transfer-Encoding\n");
            if(!iter->second.compare(" chunked")){
                printf("chunked!!!\n");
                if(dechunk(body,ss)){
                    printf("dechunkeado!!!\n");
                    ready=true;
                }
                
            }
        }
        iter=headerTable.find(std::string("Content-Length"));
        if(iter!=headerTable.end()){
            if(atoi(iter->second.c_str())<=body.size()){
               ready=true;
               ss.str("");
               for (std::vector<char>::iterator itit = body.begin();
                       itit != body.end(); ++itit) { ss << *itit; }
            }
            printf("%d==%d?\n",atoi(iter->second.c_str()) , body.size());
           // printf("Cont-Length:%s,%d\n",iter->second.c_str(), atoi(iter->second.c_str()));
        }
        iter=headerTable.find(std::string("Content-Encoding"));
        if(iter!=headerTable.end()){
            printf("Hay Content-Encoding\n");
            if(!iter->second.compare(" gzip")){
                printf("gzipeado\n");
                if(ready){
                    gunzip(ss);
                    body.clear();
                    while(!ss.eof()){
                        body.push_back(ss.get());
                    }
                }
            }
        }
        
    }
    
    ipx=i;
}
size_t HttpMessage::getLength() const{ 
    return body.size();
}
char* HttpMessage::getBytes() const{
    if (bytes == NULL) {
        int i = 0;
        bytes = new char[body.size()];
        for (std::vector<char>::const_iterator iter = body.begin();
                iter != body.end(); ++iter) { bytes[i++] = *iter; }
    }
    return bytes;
}
