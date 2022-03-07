#include "../src/raft.h"

//g++ -o test_handler test_handler.cpp ../src/*cpp ../lib/lei/src/*cpp -lrocksdb -ldl -lpthread -lboost_filesystem -lboost_system
int main(){
    RaftNode* raft = new RaftNode("conf/raft02.conf");
    //char* buf = "*\tAPPENDENTRIES\t57\t127.0.0.1:9000\t0\t0\t0\t";
    raft->SetAsFollower();
    raft->Handle();
    return 0;
}

