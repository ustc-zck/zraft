#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "conf.h"
#include "log.h"
#include "rocksdb/db.h"
#include "rocksdb/slice.h"
#include "rocksdb/options.h"
#include "utils.h"

class RaftNode{
    public:
        RaftNode(std::string confPath = "conf/raft.conf");
        ~RaftNode();
        //replicate log entries to followers, also used as heartbeart...
        //term: leader term, leaderId: leader id, prevLogIndex: the lastest index of log entry for follower, which is value of nextIndex, prevLogTerm: the term of prevLogEntry, entries: log_entries to followersm empty for heartbeat, leaderCommit: leader's commitIndex
        //prevLogIndex and prevLogTerm are used to match follower's log index and log term...
        std::pair<int, bool> AppendEntries(uint64_t term, int leaderId, uint64_t prevLogIndex, uint64_t prevLogTerm, std::vector<std::string> entries, uint64_t leaderCommit);
        
        std::pair<int, bool> RequestVote(uint64_t term, int candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm);

        //user call...
        std::string Get(std::string key);
        bool Put(std::string key, std::string value);
        bool Del(std::string key);
        bool UpdateConf(std::unordered_map<std::string, std::string> conf);
        //applied data...
        void Apply();
        //run...
        void Run();
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

    private:
        //node id, noted as addr...
        std::string NodeId;

        //raft node role, between leader, follower and candidate...
        static const int LEADER = 0;
        static const int FOLLOWER = 1;
        static const int CANDIDATE = 2;
        int role;


        //peer addrs...
        std::vector<std::string> peers;

        //persistent state on all servers...
        //latest term that server has seen...
        uint64_t currentTerm;
        //candidata id that this node vote for...
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
        //broadcast time out, may range from 0.5ms to 2ms...
        uint64_t broadcastTimeOut;

        //start election time...
        uint64_t startElectionTime;   
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
};