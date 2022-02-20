#include "../src/client.h"
#include <iostream>

//g++ -o test_client test_client.cpp ../src/*cpp ../lib/lei/src/*cpp -lrocksdb -ldl -lpthread -lboost_filesystem -lboost_system

int main(){
    RaftClient* cli = new RaftClient("127.0.0.1:9000");
    if(cli->Put("hell1o1", "world1")){
        std::cout << "sucessfully put kv into raft" << std::endl;
    }
    std::cout << "val is: " << cli->Get("hello1") << std::endl;

    if(cli->Del("hello1")){
        std::cout << "sucessfully del kv from raft" << std::endl;
    }
    
    return 0;
}