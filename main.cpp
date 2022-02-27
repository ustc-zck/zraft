#include "src/raft.h"

//g++ -o main main.cpp src/*cpp lib/lei/src/*cpp -lrocksdb -ldl -lpthread -lboost_filesystem -lboost_system

int main(){
    RaftNode* raft = new RaftNode();
    //raft->Run();
    //char* buf = "*\tAPPENDENTRIES\t57\t127.0.0.1:9000\t0\t0\t0\t";
    // raft->ServerHandler(buf);
    // std::vector<std::tuple<uint64_t, uint64_t, std::string>> entries;
    // auto t = std::tuple<uint64_t, uint64_t, std::string>{1, 1, "PUT\ta\tb\t"};
    // entries.push_back(t);
    // raft->AppendEntries(57, "127.0.0.1:9000", 0, 0, entries, 0);
    char* buf = "*\tAPPENDENTRIES\t10\t127.0.0.1:9000\t0\t0\t1\t11\tPUT\thell1o\tworld\t2\t11\tDEL\thell1o\t3\t11\tPUT\thell1o\tworld\t0\t";
    raft->ServerHandler(buf);
    return 0;
}