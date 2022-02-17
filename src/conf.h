#pragma once

#include <iostream>
#include <string>
#include <vector>

class RaftConf{
    public:
        RaftConf(std::string confPath = "../conf/raft.conf");
        std::string addr;
        std::vector<std::string> peers;
        uint64_t electionTimeOut;
        uint64_t broadcastTimeOut;
        uint64_t termTimeOut;
        int port;
};

