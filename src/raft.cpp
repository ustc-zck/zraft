#include "raft.h"

RaftNode::RaftNode(std::string confPath){
    //read conf...
    raftConf = new RaftConf(confPath);
    NodeId = raftConf->addr;
    peers = raftConf->peers;
    broadcastTimeOut = raftConf->broadcastTimeOut;
    electionTimeOut = raftConf->electionTimeOut;
    termTimeOut = raftConf->termTimeOut;

    //initialize rocksdb...
    rocksdb::Options options;
    options.IncreaseParallelism();  // Optimize RocksDB...
    options.OptimizeLevelStyleCompaction();
    options.create_if_missing = true;  // create the DB if it's not already present...
    s = rocksdb::DB::Open(options, "rocksdb/", &db);
    if(!s.ok()){
        abort();
    }

    //initialize db...
    log = new RaftLog();

    //initialize raft status...
    logIndex.clear();
    logTerm.clear();
    logCommand.clear();

    voteFor = "NONE";
    commitIndex = 0;
    lastApplied = 0;

    role = FOLLOWER;
}


RaftNode::~RaftNode(){
    delete db;
    delete raftConf;
    delete log;
}

std::string RaftNode::GetRocks(std::string key){
    std::string value;
    s = db->Get(rocksdb::ReadOptions(), key, &value);
    if(s.ok()){
        return value;
    }
    return value;
}

bool RaftNode::PutRocks(std::string key, std::string value){
    s = db->Put(rocksdb::WriteOptions(), key, value);
    if(s.ok()){
        return true;
    }
    return false;
}

bool RaftNode::DelRocks(std::string key){
    s = db->Delete(rocksdb::WriteOptions(), key);
    if(s.ok()){
        return true;
    }
    return false;
}

void RaftNode::UpdateConfDO(std::unordered_map<std::string, std::string> conf){
    for(auto iter = conf.begin(); iter != conf.end(); iter++){
        std::string key = iter->first;
        if(key == "AddNode"){
            peers.push_back(iter->second);
        } else if(key == "DelNode"){
            for(int i = 0; i < peers.size(); i++){
                if(peers[i] == iter->second){
                    peers.erase(peers.begin() + i);
                }
            }
        } else if(key == "BroadcastTimeOut") { 
            broadcastTimeOut = std::stol(iter->second);
        } else if(key == "ElectionTimeOut"){
            electionTimeOut = std::stol(iter->second);
        } else if(key == "TermTimeOut"){
            termTimeOut = std::stol(iter->second);
        } else {
            //TODO...
        }
    }
}

//user call...
std::string RaftNode::Get(std::string key){
    return this->GetRocks(key);
}

bool RaftNode::Put(std::string key, std::string value){
    if(logIndex.size() > MAXLOGENTRY){
        return false;
    }
    uint64_t index;

    if(logIndex.size() == 0){
        index = 1;
    } else{
        index = logIndex.back() + 1;
    }
    logIndex.push_back(index);
    logTerm[index] = currentTerm;
    //splice put command...
    std::string command = "PUT";
    command += "\t";
    command += key;
    command += "\t";
    command += value;
    command += "\t";
    logCommand[index] = command;
  
    return true;
}

bool RaftNode::Del(std::string key){
    if(logIndex.size() > MAXLOGENTRY){
        return false;
    }

    uint64_t index;
    if(logIndex.size() == 0){
        index = 1;
    } else{
        index = logIndex.back() + 1;
    }
    logIndex.push_back(index);
    logTerm[index] = currentTerm;
    //splice del command...
    std::string command = "DEL";
    command += "\t";
    command += key;
    command += "\t";
    logCommand[index] = command;
    return true;
}

//entries of conf includes AddNode, DelNode, BroadcastTimeOut, ElectionTimeOut, TermTimeOut...
bool RaftNode::UpdateConf(std::unordered_map<std::string, std::string> conf){
    if(logIndex.size() > MAXLOGENTRY){
        return false;
    }
    uint64_t index;
    if(logIndex.size() == 0){
        index = 1;
    } else{
        index = logIndex.back() + 1;
    }
    logIndex.push_back(index);
    logTerm[index] = currentTerm;
    //splice uipdate command...
    std::string command = "UPDATE";
    command += "\t";
    for(auto& item : conf){
        command += item.first;
        command += "\t";
        command += item.second;
    }
    logCommand[index] = command;
    return true;
}

void RaftNode::FlushLog(){
    int i;
    for(i = 0; i < logIndex.size(); i++){
        if(logIndex[i] > lastApplied){
            break;
        } else{
            std::string logEntry;
            logEntry += std::to_string(logIndex[i]);
            logEntry += "\t";
            logEntry += std::to_string(logTerm[logIndex[i]]);
            logEntry += "\t";
            logEntry += logCommand[logIndex[i]];
            //write log to file...
            log->Log(logIndex[i], logEntry);
            //erase by key...
            logTerm.erase(logIndex[i]);
            logCommand.erase(logIndex[i]);
        }
    }
    logIndex.erase(logIndex.begin(), logIndex.begin() + i);
}

void RaftNode::Apply(){
    while(true){
        if(commitIndex > lastApplied){
            auto index = logIndex[lastApplied];
            auto command = logCommand[index];
            //parse command...
            auto items = SplitStr(command, '\t');
            if(items[0] == "PUT"){
                auto ret = this->PutRocks(items[1], items[2]);
                if(ret == false){
                    continue;
                }
            }else if(items[0] == "DEL"){
                auto ret = this->DelRocks(items[1]);
                if(ret == false){
                    continue;
                }
            } else if(items[0] == "UPDATE"){
                std::unordered_map<std::string, std::string> args;
                for(int i = 1; i < items.size(); i += 2){
                    args[items[i]] = args[items[i + 1]];                    
                }
                this->UpdateConfDO(args);
            } else{
                //TODO...
            }
            lastApplied++;
        }
    }
}

void RaftNode::ReinitilAfterElection(){
    for(auto peer : peers){
        if(logIndex.size() == 0){
            nextIndex[peer] = 1;
        }else{
            nextIndex[peer] = logIndex.back() + 1;
        }
        matchIndex[peer] = 0;
    }
}

//run in the main()...
void RaftNode::Run(){

}