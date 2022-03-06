#include "../src/raft.h"

//g++ -o main main.cpp src/*cpp lib/lei/src/*cpp -lrocksdb -ldl -lpthread -lboost_filesystem -lboost_system

int main(){
    RaftNode* raft = new RaftNode("conf/raft02.conf", "rocksdb02/");
    raft->Run();
    return 0;
}