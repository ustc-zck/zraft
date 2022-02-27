#include "../src/socket.h"
#include <iostream>
#include <string>

int main(){
    Socket* s = new Socket();
    if(s->Connect("127.0.0.1:9000") < 0){
        return -1;
    }
    std::string requestMsg = "*";
    requestMsg += "\t";
    requestMsg += "APPENDENTRIES";
    requestMsg += "\t";
    requestMsg += "10000";
    requestMsg += "\t";
    requestMsg += "127.0.0.1:9001";
    requestMsg += "\t";
    requestMsg += "0";
    requestMsg += "\t";
    requestMsg += "0";
    requestMsg += "\t";
    if(s->Send(&requestMsg[0]) < 0){
        return -1;
    }
    if(s->Recev() < 0){
        return -1;
    }
    std::cout << "read result is " << s->ReadBuf() << std::endl;

    return 0;
}