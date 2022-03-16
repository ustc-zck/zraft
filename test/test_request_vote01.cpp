#include "../src/raft.h"

int main(){
    RaftNode* raft = new RaftNode("conf/raft01.conf");
    raft->SetAsCandidate();
    auto ret = raft->CandidateRequestVote("127.0.0.1:9002");
    std::cout << "first is " << ret.first << std::endl;
    std::cout << "second is " << ret.second << std::endl;
    raft->Debug();
    return 0;
}