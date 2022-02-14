#include "../src/raft.h"
#include <iostream>
#include <string>

//comple command line: g++ -o test_rocksdb test_rocksdb.cpp ../src/* -lrocksdb -ldl

int main(){
    RaftNode *raftNode = new RaftNode();
    bool result = raftNode->PutRocks("hello", "world");

    if(result){
        std::cout << "successfully put key and value into db." << std::endl;
    }

    std::string val = raftNode->GetRocks("hello");
    std::cout << "value is " << val << std::endl;

    result = raftNode->DelRocks("hello");
    if(result){
        std::cout << "successfully del key and value from db." << std::endl;
    }
    
    val = raftNode->GetRocks("hello");
    
    std::cout << "value is " << val << std::endl;

    return 0;
}