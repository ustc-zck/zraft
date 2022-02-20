#include "client.h"
#include "utils.h"
#include <iostream>

RaftClient::RaftClient(std::string addr){
    s = new Socket();
    if(s->Connect(addr) < 0){
        abort();
    }
}

RaftClient::~RaftClient(){
    delete s;
}

std::string RaftClient::Get(std::string key){
    std::string requestMsg = "*";
    requestMsg += "\t";
    requestMsg += "GET";
    requestMsg += "\t";
    requestMsg += key;
    requestMsg += "\t";
    if(s->Send(&requestMsg[0]) < 0){
        return "nil";
    }
    if(s->Recev() > 0){
        auto resp = std::string(s->ReadBuf());
        //std::cout << "client receive msg: " << resp << std::endl;
        std::vector<std::string> items = SplitStr(resp, '\t');
        if(items.size() == 1){
            return items[0];
        }else{
            return "resp num error";
        }
    }
    return "nil";
}

bool RaftClient::Put(std::string key, std::string val){
    std::string requestMsg = "*";
    requestMsg += "\t";
    requestMsg += "PUT";
    requestMsg += "\t";
    requestMsg += key;
    requestMsg += "\t";
    requestMsg += val;
    requestMsg += "\t";
     if(s->Send(&requestMsg[0]) < 0){
        return "nil";
    }
    if(s->Recev() > 0){
        auto resp = std::string(s->ReadBuf());
        std::cout << "client receive msg: " << resp << std::endl;
        std::vector<std::string> items = SplitStr(resp, '\t');
        if(items.size() == 1){
            if(items[0] == "OK"){
                return true;
            } else{
                return false;
            }
        }
    }
    return false;
}

bool RaftClient::Del(std::string key){
    std::string requestMsg = "*";
    requestMsg += "\t";
    requestMsg += "DEL";
    requestMsg += "\t";
    requestMsg += key;
    requestMsg += "\t";
    if(s->Send(&requestMsg[0]) < 0){
        return "nil";
    }
    if(s->Recev() > 0){
        auto resp = std::string(s->ReadBuf());
        std::cout << "client receive msg: " << resp << std::endl;
        std::vector<std::string> items = SplitStr(resp, '\t');
        if(items.size() == 1){
            if(items[0] == "OK"){
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}
