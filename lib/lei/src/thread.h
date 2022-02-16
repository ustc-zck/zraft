#include <thread>
#include <string>
#include <vector>
#include <algorithm>

class Thread{
    public:
        Thread();
        ~Thread();
        void (*Proc)(std::vector<int>);
        void Add(int fd);
        void Clear();
        void Join();
        std::thread thread;
        std::vector<int> fds;
    private:
        //todo...
};