#include "src/raft.h"

//g++ -o main main.cpp src/* lib/lei/src/* -lrocksdb -ldl -lpthread -lboost_filesystem -lboost_system

int main(){
    RaftNode* raft = new RaftNode();
    raft->Run();
    return 0;
}