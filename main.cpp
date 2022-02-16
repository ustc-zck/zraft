#include "src/raft.h"

int main(){
    RaftNode* raft = new RaftNode();
    raft->Run();
    return 0;
}