#include "../src/client.h"
#include <iostream>

//g++ -o test_client test_client.cpp ../src/*cpp ../lib/lei/src/*cpp -lrocksdb -ldl -lpthread -lboost_filesystem -lboost_system

int main(){
    RaftClient* cli = new RaftClient("127.0.0.1:9001");
    if(cli->Put("hell1o", "world")){
        std::cout << "sucessfully put kv into raft" << std::endl;
    }
    std::cout << "val is: " << cli->Get("hell1o") << std::endl;
    return 0;
}