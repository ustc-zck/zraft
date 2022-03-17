#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <unordered_map>
#include <thread>
#include <unistd.h>
#include <boost/algorithm/string.hpp>
#include <functional>
#include "conf.h"
#include "log.h"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "utils.h"
#include "../lib/lei/src/server.h"
#include "client.h"
#include <chrono>


class RaftNode{
    public:
        RaftNode(std::string confPath = "conf/raft.conf", std::string dataDir = "rockdsb/");
        ~RaftNode();
        //replicate log entries to followers, also used as heartbeart...
        //term: leader term, leaderId: leader id, prevLogIndex: the lastest index of log entry for follower, which is value of nextIndex, prevLogTerm: the term of prevLogEntry, entries: log_entries to followersm empty for heartbeat, leaderCommit: leader's commitIndex
        //prevLogIndex and prevLogTerm are used to match follower's log index and log term...
        std::pair<uint64_t, bool> AppendEntries(uint64_t leaderTerm, std::string leaderId_, uint64_t prevLogIndex, uint64_t prevLogTerm, std::vector<std::tuple<uint64_t, uint64_t, std::string>> entries, uint64_t leaderCommit);
        
        std::pair<uint64_t, bool> RequestVote(uint64_t candidateTerm, std::string candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm);

        //user call...
        std::string Get(std::string key);
        bool Put(std::string key, std::string value);
        bool Del(std::string key);
        bool UpdateConf(std::unordered_map<std::string, std::string> conf);

        //run...
        void LeaderRun();
        void FollowerRun();
        void CandidateRun();
        void NodeRun();
        void Run();

        std::string ServerHandler(char* buf);
        // std::string TestHanlder(char* buf){
        //     std::cout << "server recevd msg:" << buf << std::endl;
        //     return buf;
        // }
        void Handle();

        std::pair<uint64_t, bool> LeaderSendLogEntries(std::string peer, int entriesSize = 100);
        std::pair<uint64_t, bool> CandidateRequestVote(std::string peer);

        void SetAsLeader(){
            role = LEADER;
            currentTerm++;
            voteFor = "NONE";
            this->ReinitilAfterElection();
            leaderId = nodeId;
        }

        void SetAsFollower(){
            role = FOLLOWER;
        }

        void SetAsCandidate(){
            role = CANDIDATE;
        }

        void Debug(){
            while(true){
                std::cout << "role is " << role << std::endl;
                std::cout << "leader is " << leaderId << std::endl;
                std::cout << "voted for " << voteFor << std::endl;
                std::cout << "current term is " << currentTerm << std::endl;
                std::cout << "last log index is " << this->LastLogIndex() << std::endl;
                std::cout << "last log term is " << logTerm[this->LastLogIndex()] << std::endl;
                std::cout << "last log command is " << logCommand[this->LastLogIndex()] << std::endl;
                if(role == LEADER){
                    for(auto peer : peers){
                        std::cout << "prev log index of " << peer << " is " << nextIndex[peer] - 1 << "\t";
                        std::cout << "prev log term of " << peer  << " is " << logTerm[nextIndex[peer] - 1] << std::endl;
                        std::cout << "next log index of  " << peer << " is " << nextIndex[peer] << std::endl; 
                        std::cout << "match index of " << peer << " is " << matchIndex[peer] << std::endl;
                    }
                }
                std::cout << "commit index is " << commitIndex << std::endl;
                std::cout << "last applied index is " << lastApplied << std::endl;
                std::cout << "votes is " << votes << std::endl;
                std::cout << "\n" << std::endl;
                sleep(1);
            }
        }

    private:
        //db operation of rocksdb, not used for client...
        std::string GetRocks(std::string key);
        bool PutRocks(std::string key, std::string val);
        bool DelRocks(std::string key);
        //return true for success, false for failure...
        void UpdateConfDO(std::unordered_map<std::string, std::string> conf);    
        void FlushLog(); 

        //after elected as ledaer, rinitialize....
        void ReinitilAfterElection();
        //applied data...
        void Apply();
        uint64_t LastLogIndex(){
            if(logIndex.size() == 0){
                    return 0;
            }else{
                return logIndex.back();
            }
        }
        uint64_t FirstLogIndex(){
            if(logIndex.size() == 0){
                return 0;
            } else{
                return logIndex.front();
            }
        }
        void UpdateCommitIndex();


    private:
        //node id, noted as addr...
        std::string nodeId;
        //leader id...
        std::string leaderId;

        //raft node role, between leader, follower and candidate...
        static const int LEADER = 0;
        static const int FOLLOWER = 1;
        static const int CANDIDATE = 2;
        int role;

        //peer addrs...
        std::vector<std::string> peers;
        //std::unordered_map<std::string, uint64_t> fds;

        //persistent state on all servers...
        //latest term that server has seen...
        uint64_t currentTerm;
        //candidata id that this node vote for...
        int votes;
        std::string voteFor;

        //log entries, consists of log index + term + command...
        static const int MAXLOGENTRY = 786432;
        std::vector<uint64_t> logIndex;
        std::unordered_map<uint64_t, uint64_t> logTerm;
        
        //format is such as "PUT\tKEY\tVALUE\t\n"...
        std::unordered_map<uint64_t, std::string> logCommand;

        //volatile state on all servers...
        //index of highest log entry knonw to be committed...
        uint64_t commitIndex;
        //index of hishest log entry known to be applied...
        uint64_t lastApplied;

        //volatile state on leasers, reinitialize after election...
        //key is follower addr, value is index....
        std::unordered_map<std::string, uint64_t> nextIndex;
        std::unordered_map<std::string, uint64_t> matchIndex;

        //broadcastTime ≪ electionTimeout ≪ MTBF     
        //last receive log entries time....
        uint64_t lastReceiveLogEntriesTime;

        //start election time...
        uint64_t startElectionTime;
        //broadcast time out, may range from 0.5ms to 2ms...
        uint64_t broadcastTimeOut;

        //election time out, may range from 100ms to 500ms...
        uint64_t electionTimeOut;

        //term time out, period of term...
        uint64_t termTimeOut;     

        //raft conf...
        RaftConf* raftConf;

        //use rocksdb as storage engine...
        rocksdb::Status s;
        rocksdb::DB* db;

        //logger...
        RaftLog* log;             

        //server...
        Server* server;
        //thread pool...
        std::vector<std::thread> threads;                                                                                                                                                                                                                                                                                             
};