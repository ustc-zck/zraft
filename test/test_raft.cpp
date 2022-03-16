#include "../src/raft.h"



//g++ -o test_append_entries test_append_entries.cpp ../src/*cpp ../lib/lei/src/*cpp -lrocksdb -ldl -lpthread -lboost_filesystem -lboost_system

int main(){
    RaftNode* raft = new RaftNode("conf/raft00.conf", "rocksdb03/");
    raft->SetAsCandidate();
    //char* buf = "*\tAPPENDENTRIES\t10000000\t127.0.0.1:9000\t0\t0\t0\t";
    //char* buf = "*\tAPPENDENTRIES\t1000000\t127.0.0.1:9000\t0\t0\t1\t11\tPUT\thell1o\tworld\t2\t11\tPUT\thell1o1\tworld1\t3\t11\tDEL\thell1o1\t4\t11\tPUT\thell1o1\tworld2\t0\t";
    char* buf = "*\tREQUESTVOTE\t0\t127.0.0.1:9000\t0\t0\t";
    raft->ServerHandler(buf);  
    // raft->ServerHandler(buf1);  
    //raft->Debug();
    //auto ret = raft->LeaderSendLogEntries("127.0.0.1:9001");
    //std::cout << "first is " << ret.first << std::endl;
    //std::cout << "second is " << ret.second << std::endl;
    //raft->Debug();
    return 0;
}
