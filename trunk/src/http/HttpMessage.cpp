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
    if(data.size()<6){
        return;
    }
    bool fromServer=true;
    char seq[7] = { 'H', 'T', 'T', 'P','/','1','.' };
    size_t i;
    for(i=data.size()-7;i>=7;i--){
        for(int j=6;j>=0;j--){
            fromServer=true;
            if(data[i+j]!=seq[j]){
                //ipx=i;
                fromServer=false;
                break;
            }
        }
        if(fromServer){
            while(data[i]!='\x0d'&& data[i+1]!='\x0a'){
                i++;
            }
            i+=2;
            break;
        }
    }
    if(!fromServer)return;
    if(i>0){
        printf("Borrando todo lo anterior\n");
        body.clear();
        std::vector<char> aux;
        for(int j=0;i+j<data.size();j++){
            aux.push_back(data[i+j]);
        }
    
        data.clear();
        for(i=0;i<aux.size();i++){
            data.push_back(aux[i]);
        }
        headersReady=false;
    }
    if(!headersReady){
        printf("Headers no listos\n");
        for(i=0;i<data.size()-1;i++){
            std::ostringstream hbuffer, buffer;
            while( i<data.size() && data[i]!=':' ){
                if(data[i]=='\x0d' && data[i+1]=='\x0a'){
                    headersReady=true;
                    printf("Headers listos\n");
                    i+=2;
                    break;
                }
                hbuffer<<data[i++];
            }
            i++;
            while(i<data.size()-1 && !(data[i]=='\x0d' && 
data[i+1]=='\x0a')){
                buffer<<data[i++];
            }
            i++;
            if(i<data.size())ipx=i;
            headerTable[hbuffer.str()]=buffer.str();
            printf("insertando:%s->%s\n", hbuffer.str().c_str(),buffer.str().c_str());
        }
    }
    if(headersReady){
        stringstream ss;
        for(;i<data.size()-1;i++){
            body.push_back(data[i]);
        }
        std::map<std::string,std::string>::iterator 
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
            if(atoi(iter->second.c_str())>=body.size()){
               ready=true;
            }
            printf("%d==%d?\n",atoi(iter->second.c_str()) , body.size());
           // printf("Cont-Length:%s,%d\n",iter->second.c_str(), atoi(iter->second.c_str()));
        }
        iter=headerTable.find(std::string("Content-Encoding"));
        if(iter!=headerTable.end()){
            printf("Hay Content-Encoding\n");
            if(!iter->second.compare(" gzip")){
                printf("gzipeado");
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
