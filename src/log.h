#pragma once

#include <iostream>
#include <string> 
#include <fstream>

class RaftLog{
    public:
        RaftLog(std::string logDir_ = "log/");
        ~RaftLog();
        void Log(uint64_t index, std::string msg);
    private:
        std::ofstream log;
        uint64_t logFileNum;
        std::string logDir;
        static const uint64_t MAXLOGENTRYNUM = 786432;
};