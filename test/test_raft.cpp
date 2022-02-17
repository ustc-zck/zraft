#include "../src/raft.h"

//g++ -o test_raft test_raft.cpp ../src/*.cpp -lrocksdb -ldl -lboost_filesystem -lboost_system

int main(){

    RaftNode* raft = new RaftNode();
    
    if(raft->Put("hello", "World")){
        std::cout << "successfully put value." << std::endl;
    }

    raft->Apply();
    
    std::cout << raft->Get("hello") << std::endl;
    
    delete raft;
    
    return 0;
}