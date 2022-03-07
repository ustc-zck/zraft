#include "raft.h"

using std::placeholders::_1;

RaftNode::RaftNode(std::string confPath, std::string dataDir){
    //read conf...
    raftConf = new RaftConf(confPath);
    nodeId = raftConf->addr;
    peers = raftConf->peers;
    broadcastTimeOut = raftConf->broadcastTimeOut;
    electionTimeOut = raftConf->electionTimeOut;
    termTimeOut = raftConf->termTimeOut;
    //initialize rocksdb...
    rocksdb::Options options;
    options.IncreaseParallelism();  // Optimize RocksDB...
    options.OptimizeLevelStyleCompaction();
    options.create_if_missing = true;  // create the DB if it's not already present...
    s = rocksdb::DB::Open(options, dataDir, &db);
    if(!s.ok()){
        abort();
    }

    //initialize db...
    log = new RaftLog();

    //initialize raft status...
    logIndex.clear();
    logTerm.clear();
    logCommand.clear();
    nextIndex.clear();
    matchIndex.clear();
    //Not voted for any server...
    votes = 0;
    voteFor = "NONE";
    leaderId = "NONE";

    commitIndex = 0;
    lastApplied = 0;

    role = FOLLOWER;

    lastReceiveLogEntriesTime = GetCurrentMillSeconds();

    server = new Server(raftConf->port);
    //note this...
    logTerm[0] = 0;

    currentTerm = 0;

}

void RaftNode::ReinitilAfterElection(){
    for(auto peer : peers){
        nextIndex[peer] = this->LastLogIndex() + 1;
        matchIndex[peer] = 0;
    }
    voteFor = "NONE";
}

RaftNode::~RaftNode(){
    delete db;
    delete raftConf;
    delete log;
    delete server;
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
        boost::algorithm::to_upper(key);
        if(key == "ADDNODE"){
            peers.push_back(iter->second);
        } else if(key == "DELNODE"){
            for(int i = 0; i < peers.size(); i++){
                if(peers[i] == iter->second){
                    peers.erase(peers.begin() + i);
                }
            }
        } else if(key == "BROADCASTTIMEOUT") { 
            broadcastTimeOut = std::stol(iter->second);
        } else if(key == "ELECTIONTIMEOUT"){
            electionTimeOut = std::stol(iter->second);
        } else if(key == "TERMTIMEOUT"){
            termTimeOut = std::stol(iter->second);
        } else {
            //TODO, update more conf items...
        }
    }
}

//user call...
std::string RaftNode::Get(std::string key){
    //not strong consistency...
    return this->GetRocks(key);
}

bool RaftNode::Put(std::string key, std::string value){
    if(logIndex.size() > MAXLOGENTRY){
        return false;
    }
    uint64_t index = this->LastLogIndex() + 1;

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
    ////std::cout << "log index is " << index << std::endl;
    ////std::cout << "last term is " << logTerm[index] << std::endl;
    ////std::cout << "last command is " << logCommand[index] << std::endl;
    return true;
}

bool RaftNode::Del(std::string key){
    if(logIndex.size() > MAXLOGENTRY){
        return false;
    }
    uint64_t index = this->LastLogIndex() + 1;

    logIndex.push_back(index);
    logTerm[index] = currentTerm;
    //splice del command...
    std::string command = "DEL";
    command += "\t";
    command += key;
    command += "\t";
    logCommand[index] = command;
    ////std::cout << "log index is " << index << std::endl;
    ////std::cout << "current term is " << logTerm[index] << std::endl;
    ////std::cout << "last command is " << logCommand[index] << std::endl;
    return true;
}

//entries of conf includes AddNode, DelNode, BroadcastTimeOut, ElectionTimeOut, TermTimeOut...
bool RaftNode::UpdateConf(std::unordered_map<std::string, std::string> conf){
    if(logIndex.size() > MAXLOGENTRY){
        return false;
    }
    uint64_t index = this->LastLogIndex() + 1;

    logIndex.push_back(index);
    logTerm[index] = currentTerm;
    //splice uipdate command...
    std::string command = "UPDATE";
    command += "\t";
    for(auto& item : conf){
        command += item.first;
        command += "\t";
        command += item.second;
        command += "\t";
    }
    logCommand[index] = command;
    return true;
}

void RaftNode::FlushLog(){
    if(logIndex.size() == 0){
        return;
    }
    int i;
    //keep last element...
    for(i = 0; i < logIndex.size() - 1; i++){
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
    if(i > 0){
        logIndex.erase(logIndex.begin(), logIndex.begin() + i);
    }
}

void RaftNode::Apply(){
    while(true){
        if(commitIndex > lastApplied){
            auto index = logIndex[lastApplied + 1];
            auto command = logCommand[index];
            //parse command...
            auto items = SplitStr(command, '\t');
            if(items[0] == "PUT"){
                auto ret = this->PutRocks(items[1], items[2]);
                if(ret == false){
                    continue;
                }
            } else if(items[0] == "DEL"){
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

//leader --> follower, now only support replicate one entry one time...
std::pair<uint64_t, bool> RaftNode::AppendEntries(uint64_t leaderTerm, std::string leaderId_, uint64_t prevLogIndex, uint64_t prevLogTerm, std::vector<std::tuple<uint64_t, uint64_t, std::string>> entries, uint64_t leaderCommit){
    if(role == CANDIDATE){
        role = FOLLOWER;
        leaderId = leaderId_;
    }

    lastReceiveLogEntriesTime = GetCurrentMillSeconds();
    ////std::cout << "time is " << lastReceiveLogEntriesTime << std::endl;
    if(leaderId == "NONE"){
        leaderId = leaderId_;   
    }

    ////std::cout << "leader term is " << leaderTerm << std::endl;
    ////std::cout << "current term is " << currentTerm << std::endl;
    if(leaderTerm < currentTerm){
        return std::make_pair(currentTerm, false);
    } else if (leaderTerm > currentTerm) {
        currentTerm = leaderTerm;
        voteFor = "NONE";
    } else{

    }
    //default unordered map contains 0 or not???
    ////std::cout << "find pre log index is " << (logTerm.find(prevLogIndex) == logTerm.end()) << std::endl;
    if(logTerm.find(prevLogIndex) == logTerm.end()){
        ////std::cout << "not find" << std::endl;
        return std::make_pair(currentTerm, false);
    }
    ////std::cout << "pre log term is " << logTerm[prevLogIndex] << std::endl;
    ////std::cout << "pre log term is " << prevLogTerm << std::endl;

    if(logTerm[prevLogIndex] != prevLogTerm){
        //conflict...
        return std::make_pair(currentTerm, false);
    } else{
        //not conflict...
        int i; 
        for(i = logIndex.size() - 1; i >=0; i--){
            if(logIndex[i] > prevLogIndex){
                logTerm.erase(logIndex[i]);
                logCommand.erase(logIndex[i]);
                ////std::cout << "erase log term " << std::endl;
            }
        }

        ////std::cout << "size of log index " << logIndex.size() <<  std::endl;
        ////std::cout << "erase log index " << std::endl;
        ////std::cout << "i is " << i << std::endl;
        logIndex.erase(logIndex.begin() + i + 1, logIndex.end());
        ////std::cout << "size of log index " << logIndex.size() <<  std::endl;
    }
    //append new entries...
    for(auto entry : entries){
        auto entryIndex = std::get<0>(entry);
        auto entryTerm = std::get<1>(entry);
        auto entryCommand = std::get<2>(entry);
        logIndex.push_back(entryIndex);
        logTerm[entryIndex] = entryTerm;
        logCommand[entryIndex] = entryCommand;
    }
    //update commitIndex...
    commitIndex = std::min(leaderCommit, this->LastLogIndex());

    // for(int i = 0; i < logIndex.size(); i++){
    //     //std::cout << "log index is " << logIndex[i] << std::endl;
    //     //std::cout << "log term is " << logTerm[logIndex[i]] << std::endl;
    //     //std::cout << "log command is " << logCommand[logIndex[i]] << std::endl;
    // }
    return std::make_pair(currentTerm, true);
}

//candidate --> other servers
std::pair<uint64_t, bool> RaftNode::RequestVote(uint64_t candidateTerm, std::string candidateId, uint64_t candidateLastLogIndex, uint64_t candidateLastLogTerm){
    if(candidateTerm <  currentTerm){
        return std::make_pair(currentTerm, false);
    }
    if((voteFor == "NONE" || voteFor == candidateId) && (candidateLastLogIndex >= this->LastLogIndex() && candidateLastLogTerm >= logTerm[this->LastLogIndex()])){
        voteFor = candidateId;
        return std::make_pair(currentTerm, true);
    }
    return std::make_pair(currentTerm, false);
}

//update commit index...
void RaftNode::UpdateCommitIndex(){
    for(auto peer : peers){
        auto N = matchIndex[peer];
        if(N > commitIndex){
            int count = 0;
            for(auto peer : peers){
                if(matchIndex[peer] >= N){
                    count++;
                }
            }
            if(count > (peers.size() + 1) / 2.0 && logTerm[N] ==  currentTerm){
                commitIndex = N;
            }
        }       
    }
}

std::string RaftNode::ServerHandler(char* buf){
    std::string input(buf);
    std::cout << "receive message " << input << std::endl;
    std::vector<std::string> commands = SplitStr(input, '*');
    for(auto command : commands){
        ////std::cout << "command is: " << command << std::endl;
        std::vector<std::string> items = SplitStr(command, '\t');
        std::string operation = items[0];
        boost::algorithm::to_upper(items[0]);
        if(operation == "PUT"){
            if(role != LEADER){
                RaftClient* cli = new RaftClient(leaderId);
                if (cli->Put(items[1], items[2])){
                    return "OK\t";
                }else {
                    return "ERROR\t";
                }
                delete cli;
            }
            if(items.size() == 3){
                if(this->Put(items[1], items[2])){
                    return "OK\t";
                }
            }
            return "ERROR\t";
        } else if(operation == "GET"){
            if(items.size() == 2){
                std::string val = this->Get(items[1]);
                if(val == ""){
                    return "nil\t";
                }
                return val + "\t";
            }
            return "ERROR\t";
        } else if(operation == "DEL"){
            if(role != LEADER){
                RaftClient* cli = new RaftClient(leaderId);
                if (cli->Del(items[1])){
                    return "OK\t";
                }else {
                    return "ERROR\t";
                }
                delete cli;
            }
            if(items.size() == 2){
                if(this->Del(items[1])){
                    return "OK\t";
                }
            }
            return "ERROR\t";
        } else if(operation == "APPENDENTRIES"){
            uint64_t leaderTerm = std::stoull(items[1]);
            ////std::cout << "leader term is " << leaderTerm << std::endl;
            std::string leaderId_ = items[2];
            ////std::cout << "leader id is " << leaderId_ << std::endl;
            uint64_t prevLogIndex = std::stoull(items[3]);
            ////std::cout << "prevLogIndex is " << prevLogIndex << std::endl;
            uint64_t prevLogTerm = std::stoull(items[4]);
            ////std::cout << "prevLogTerm is " << prevLogTerm << std::endl;
            std::vector<std::tuple<uint64_t, uint64_t, std::string>> entries;
            
            // for(int i = 5; i < items.size() - 1; i += 3){
            //     auto t = std::tuple<uint64_t, uint64_t, std::string>{std::stoull(items[i]), std::stoull(items[i+1]), items[i+2]};
            //     entries.push_back(t);
            // }
            int i = 5;
            while(i < items.size() - 1){
                if(items[i + 2] == "PUT"){
                    std::string command = items[i+2];
                    command += "\t";
                    command += items[i+3];
                    command += "\t";
                    command += items[i+4];
                    command += "\t";
                    auto t = std::tuple<uint64_t, uint64_t, std::string>{std::stoull(items[i]), std::stoull(items[i+1]), command};
                    entries.push_back(t);
                    i += 5;
                } else if(items[i + 2] == "DEL"){
                    std::string command = items[i+2];
                    command += "\t";
                    command += items[i+3];
                    command += "\t";
                    auto t = std::tuple<uint64_t, uint64_t, std::string>{std::stoull(items[i]), std::stoull(items[i+1]), command};
                    entries.push_back(t);
                    i += 4;
                } else {
                    //TODO, support more commands...
                }
            }
            uint64_t leaderCommit = std::stoull(items.back());
            std::cout << "ledar term is " << leaderTerm << std::endl;
            std::cout << "leader id is " << leaderId_ << std::endl;
            std::cout << "prev log index is " << prevLogIndex << std::endl;
            std::cout << "prev log term is " << prevLogTerm << std::endl;
            for(auto entry : entries ){
                std::cout << "commad entry is " << std::get<0>(entry) << " " << std::get<1>(entry) << " "<< std::get<2>(entry) << std::endl;
            }
            std::cout << "leader commit is " << leaderCommit << std::endl; 
            std::pair<uint64_t, bool> ret = this->AppendEntries(leaderTerm, leaderId_, prevLogIndex, prevLogTerm, entries, leaderCommit);
            std::string resp;
            if(ret.second == true){
                resp += std::to_string(ret.first);
                resp += "\t";
                resp += "TRUE";
                resp += "\t";
            }else{
                resp += std::to_string(ret.first);
                resp += "\t";
                resp += "FALSE";
                resp += "\t";
            }
            std::cout << "resp of append entries is " << resp << std::endl;
            // for(int i = 0; i < logIndex.size(); i++){
            //     std::cout << "log index is " << logIndex[i] << std::endl;
            //     std::cout << "term is " << logTerm[logIndex[i]] << std::endl;
            //     std::cout << "command is " << logCommand[logIndex[i]] << std::endl;
            // }
            return resp;
        } else if(operation == "REQUESTVOTE"){
            if(items.size() == 5){
                uint64_t candidateTerm = std::stoull(items[1]);
                std::string candidateId = items[2];
                //std::cout << "candidate id " << candidateId << " request vote" << std::endl;
                uint64_t candidateLastLogIndex = std::stoull(items[3]);
                uint64_t candidateLastLogTerm = std::stoull(items[4]);
                
                std::pair<uint64_t, bool> ret = this->RequestVote(candidateTerm, candidateId, candidateLastLogIndex, candidateLastLogTerm);
                std::string resp;
                if(ret.second == true){
                    resp += std::to_string(ret.first);
                    resp += "\t";
                    resp += "TRUE";
                    resp += "\t";
                }else{
                    resp += std::to_string(ret.first);
                    resp += "\t";
                    resp += "FALSE";
                    resp += "\t";
                }
                std::cout << "request vote resp is " << resp << std::endl; 
                this->Debug();
                return resp;
            }
        }else{
            //TODO, support more command...
        }
    }
    return "ERROR\t";
}

std::pair<uint64_t, bool> RaftNode::LeaderSendLogEntries(std::string peer, int entriesSize){
    if(role != LEADER){
        return std::make_pair(0, false);
    }
  
    std::string requestMsg = "*";
    requestMsg += "\t";
    requestMsg += "APPENDENTRIES";
    requestMsg += "\t";
    requestMsg += std::to_string(currentTerm);
    requestMsg += "\t";
    requestMsg += nodeId;
    requestMsg += "\t";
    uint64_t prevLogIndex = nextIndex[peer] - 1;
    requestMsg += std::to_string(prevLogIndex);
    requestMsg += "\t";
    uint64_t prevLogTerm = logTerm[prevLogIndex];
    requestMsg += std::to_string(prevLogTerm);
    requestMsg += "\t";

    uint64_t i;
    //send at most 100 items at once...
    for(int i = nextIndex[peer] - this->FirstLogIndex(); i < logIndex.size() && i < nextIndex[peer] - this->FirstLogIndex() + 100; i++){
        requestMsg += std::to_string(logIndex[i]);
        requestMsg += "\t";
        requestMsg += std::to_string(logTerm[logIndex[i]]);
        requestMsg += "\t";
        requestMsg += logCommand[logIndex[i]];
        requestMsg += "\t";
    }
    requestMsg += std::to_string(commitIndex);
    requestMsg += "\t";

    std::cout << "leader " << nodeId << " send log entries " << requestMsg << std::endl; 

    auto s = new Socket();
    if(s->Connect(peer) < 0){
        return std::make_pair(0, false);
    }
    if(s->Send(&requestMsg[0]) < 0){
        return std::make_pair(0, false);
    }
    if(s->Recev() > 0){
        auto resp = s->ReadBuf();
        std::cout << "resp is " << resp << std::endl;
        std::vector<std::string> items = SplitStr(resp, '\t');
        if(items[1] == "TRUE"){
            return std::make_pair(std::stoull(items[0]), true);
        } else{
            return std::make_pair(std::stoull(items[0]), false);
        }
    }
    return std::make_pair(0, false);
}

std::pair<uint64_t, bool> RaftNode::CandidataRequestVote(std::string peer){
    if(role != CANDIDATE){
        return std::make_pair(0, false);
    }
    auto s = new Socket();
    if(s->Connect(peer) < 0){
        return std::make_pair(0, false);
    }
    std::string requestMsg = "*";
    requestMsg += "\t";
    requestMsg += "REQUESTVOTE";
    requestMsg += "\t";
    requestMsg += std::to_string(currentTerm);
    requestMsg += "\t";
    requestMsg += nodeId;
    requestMsg += "\t";
    requestMsg += std::to_string(this->LastLogIndex());
    requestMsg += "\t";
    requestMsg += std::to_string(logTerm[this->LastLogIndex()]);
    requestMsg += "\t";

    if(s->Send(&requestMsg[0]) < 0){
        return std::make_pair(0, false);
    }
    ////std::cout << "request vote from " << peer << std::endl;
    ////std::cout << "current term is " << currentTerm << std::endl;

    if(s->Recev() > 0){
        auto resp = std::string(s->ReadBuf());
        std::cout << "resp is " << resp << std::endl;
        std::vector<std::string> items = SplitStr(resp, '\t');
        if(items.size() >= 2){
            if(items[1] == "TRUE"){
                return std::make_pair(std::stoull(items[0]), true);
            } else{
                return std::make_pair(std::stoull(items[0]), false);
            }
        }
    }
    return std::make_pair(0, false);
}

//run on leader node...
void RaftNode::LeaderRun(){
    if(role != LEADER){
        return;
    }
    for(auto peer : peers){
        ////std::cout << "last log index is " << this->LastLogIndex() << std::endl;
        ////std::cout << "next index is " << nextIndex[peer] << std::endl;
        if(this->LastLogIndex() >= nextIndex[peer]){
            ////std::cout << "nodeId " << nodeId << " send log entries " << std::endl;
            //call append entries rpc in paralles...
            auto ret = this->LeaderSendLogEntries(peer);
            if(ret.first > currentTerm){
                currentTerm = ret.first;
                voteFor = "NONE";
                role = FOLLOWER;
            } else if(ret.second == false){
                nextIndex[peer]--;
                if(nextIndex[peer] < 1){
                    nextIndex[peer] = 1;
                }
            } else{
                if(nextIndex[peer] + 100 > this->LastLogIndex() + 1){
                    nextIndex[peer] = this->LastLogIndex() + 1;
                } else{
                    nextIndex[peer] += 100;
                }
                matchIndex[peer] = nextIndex[peer] - 1;
                this->UpdateCommitIndex();
            }
        } else{
            //send heart beat...
            ////std::cout << "nodeId " << nodeId << " send heart beat " << std::endl;
            this->LeaderSendLogEntries(peer);
        }
    }
}

void RaftNode::FollowerRun(){
    if(role != FOLLOWER){
        return;
    }
    if(GetCurrentMillSeconds() - lastReceiveLogEntriesTime > electionTimeOut){
        role = CANDIDATE;
    }
}

void RaftNode::CandidateRun(){
    if(role != CANDIDATE){
        return;
    }
    currentTerm++;
    voteFor = nodeId;
    votes = 1;
    startElectionTime = GetCurrentMillSeconds();
    auto randomSleepTime = uint64_t(GenerateRandomNumber() * electionTimeOut);
    ////std::cout << "sleep random milliseconds " << randomSleepTime << std::endl;
    usleep(randomSleepTime);
    for(auto peer : peers){
        ////std::cout << "request vote from peer " << peer << std::endl;
        ////std::cout << "current term is " << currentTerm << std::endl;

        auto ret = CandidataRequestVote(peer);
        ////std::cout << "result of requst vote is " << ret.first << "\t" << ret.second << std::endl;
        if(ret.first > currentTerm){
            //can not be chosen as leader...
            currentTerm = ret.first;
            //each vote in each term...
            voteFor = "NONE";
            role = FOLLOWER;
            break;
        } else if(ret.second == true){
            votes++;
            //std::cout << "get voted from " << peer << std::endl;
        } else{
            continue;
        }
    }
    if(role == CANDIDATE && votes > (1 + peers.size()) / 2.0){
        role = LEADER;
        ////std::cout << nodeId << " is chosen as leader." << std::endl;
        this->ReinitilAfterElection();
        return;
    }
    // if(role == CANDIDATE){
    //     uint64_t requestVoteCost = GetCurrentMillSeconds() - startElectionTime;
    //     if(randomSleepTime + requestVoteCost < electionTimeOut){
    //         ////std::cout << "sleep time for " << electionTimeOut - randomSleepTime - requestVoteCost << std::endl;
    //         usleep(electionTimeOut - randomSleepTime - requestVoteCost);
    //     } 
    // }
    //for debug...
    return;
}

//todo, change this to timer event...
// void RaftNode::NodeRun(){
//     while(true){
//         if(role == LEADER){
//             LeaderRun();
//         } else if(role == FOLLOWER){
//             FollowerRun();
//         } else if(role == CANDIDATE){
//             CandidateRun();
//         }
//     }
// }

void RaftNode::Handle(){
    server->Handler = std::bind(&RaftNode::ServerHandler, this, _1);
    server->TimerEvent(std::bind(&RaftNode::LeaderRun, this), broadcastTimeOut);
    server->TimerEvent(std::bind(&RaftNode::FollowerRun, this), electionTimeOut);
    server->TimerEvent(std::bind(&RaftNode::CandidateRun, this), electionTimeOut);
    server->TimerEvent(std::bind(&RaftNode::Debug, this), electionTimeOut);
    server->Run();
}

void RaftNode::Run(){
    std::thread handle(&RaftNode::Handle, this);
    std::thread apply(&RaftNode::Apply, this);
    //std::thread run(&RaftNode::NodeRun, this);
    //TODO, add flush log...
    //std::thread flushlog(&RaftNode::FlushLog, this);
    //std::thread debug(&RaftNode::Debug, this);
    handle.join();
    apply.join();
    //run.join();
    //flushlog.join();
    //debug.join();
}