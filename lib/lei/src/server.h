#include "socket.h"
#include <string>
#include <vector>
#include <functional>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "thread.h"
#include <iostream>

//to be optimized...
const int MAXEVENTS = 4096;
const int MAXTHREADS = 1024;

class Server{
    public:
        Server(int port = 8080);
        ~Server();
        //IO Handle func...
        std::function<std::string(char* buf)> Handler;

        //IO Timer func...
        // int (*TimeHandler)();
        std::function<void(void)> TimeHandler;
        //Add time event...
        int AddTimeEvent(int millisceonds);
        //wrapper...
        void Wrapper(std::vector<int> fds){
            for(auto fd : fds){
                Socket* s = new Socket(fd);
                //about the buf size, there is a balance here...
                int n = s->Recev();
                //readble but can not recev data, close fd...
                if (n <= 0){
                    close(fd);
                    return;
                }   
                char* buf = s->ReadBuf();
                buf[n] = '\0';
                //std::cout << "receive msg is: " << buf << std::endl;
                //call Handler...
                auto toWrite = Handler(buf);
                //error happened, close fd...
                if(s->Send(&toWrite[0]) < 0){
                    close(fd);
                }
            }
        }    
        //run server...
        int Run();
    private:
        //event fd...
        int efd;
        int Port;
        struct epoll_event* event;
        struct epoll_event *events;
        Socket* listen_socket;
        Socket* timer_socket;
        //thread pool...
        //std::vector<std::thread> threads;
        std::vector<Thread*> threads;
};