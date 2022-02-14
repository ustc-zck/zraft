#include "log.h"
#include <filesystem>
#include <boost/filesystem.hpp>
#include <fstream>

RaftLog::RaftLog(std::string logDir_){
    logFileNum = 0;
    logDir = logDir_;
    for(auto iter : boost::filesystem::directory_iterator(logDir)){
        logFileNum++;
    }
    //std::cout << "file num is " << logFileNum << std::endl;
    std::string logPath = logDir;
}

RaftLog::~RaftLog(){
    log.close();
}

void RaftLog::Log(uint64_t index, std::string msg){
    if(index % MAXLOGENTRYNUM == 1){
        logFileNum++;
        std::string logPath = logDir;
        for(int i = 0; i < 8 - std::to_string(logFileNum).length(); i++){
            logPath += "0";
        }
        logPath += std::to_string(logFileNum);
        logPath +=  ".log";
        //std::cout << "log path is " << logPath << std::endl;
        std::ofstream log_(logPath);
        log = std::move(log_);
    }
    msg += "\n";
    log << msg;
}

