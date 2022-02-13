#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

class RaftNode{
    public:
        RaftNode();
        ~RaftNode();
        //replicate log entries to followers, also used as heartbeart...
        //term: leader term, leaderId: leader id, prevLogIndex: the lastest index of log entry for follower, which is value of nextIndex, prevLogTerm: the term of prevLogEntry, entries: log_entries to followersm empty for heartbeat, leaderCommit: leader's commitIndex
        //prevLogIndex and prevLogTerm are used to match follower's log index and log term...
        std::pair<int, bool> AppendEntries(uint64_t term, int leaderId, uint64_t prevLogIndex, uint64_t prevLogTerm, std::vector<std::string> entries, uint64_t leaderCommit);
        
        std::pair<int, bool> RequestVote(uint64_t term, int candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm);

        //applied data...
        void Apply(){
            if(commitIndex > lastApplied){
                std::string entry = log[lastApplied];
                //parse log...
                //TODO...
            }
        } 

        //client api...
        std::string Get(std::string key);

        std::string Put(std::string key, std::string val);

        std::string Del(std::string key);

        int UpdateConf(std::string conf);
        
    private:
        //node id...
        int NodeId;
        //persistent state on all servers...
        //latest term that server has seen...
        uint64_t currentTerm;
        //candidata id that this node vote for...
        int voteFor;
        //log entries, consists of log index + term + command...
        std::vector<std::string> log;

        //volatile state on all servers...
        //index of highest log entry knonw to be committed...
        uint64_t commitIndex;
        //index of hishest log entry known to be applied...
        uint64_t lastApplied;

        //volatile state on leasers, reinitialize after election...
        std::vector<uint64_t> nextIndex;
        std::vector<uint64_t> matchIndex;

        //use hash map to store data...
        std::unordered_map<std::stirng, std::string> kv;

       
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


                                                                                                                                                                                                                                                                                                                                                                       
};