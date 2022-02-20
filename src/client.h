#pragma once

#include <string>
#include "../lib/lei/src/socket.h"

class RaftClient{
    public:
        RaftClient(std::string addr);
        ~RaftClient();
        std::string Get(std::string key);
        bool Put(std::string key, std::string val);
        bool Del(std::string key);
    private:
        Socket* s;
};