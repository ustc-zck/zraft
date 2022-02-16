#include "server.h"
#include <sys/epoll.h>
#include <iostream>
#include <unistd.h>
#include <sys/timerfd.h>

Server::Server(int port){
    efd = epoll_create1(0);
    if (efd == -1) {
        throw("epoll create error");
        abort();
    }
    listen_socket = new Socket();
    if(listen_socket->SetAddrPortReuse() < 0){
        abort();
    }
    if(listen_socket->SetSocketNonBlocking() < 0){
        abort();
    }
    if(listen_socket->Bind(port) < 0){
        abort();
    }
    if(listen_socket->Listen() < 0 ){
        abort();
    }
    //initialize...
    event = (struct epoll_event*) malloc(sizeof(struct epoll_event*));
    event->data.fd = listen_socket->Fd();
    event->events = EPOLLIN | EPOLLET;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, listen_socket->Fd(), event) < 0) {
        abort();
    }
    events = (struct epoll_event*) calloc(MAXEVENTS, sizeof(struct epoll_event*));

    int tfd;
    tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if(tfd < 0){
        abort();
    }
    timer_socket = new Socket(tfd);
    // std::vector<std::thread> ths(MAXTHREADS);
    // threads = std::move(ths);
    for(int i = 0; i < MAXTHREADS; i++){
        Thread* s = new Thread();
        threads.push_back(s);
    }
   
}

int Server::AddTimeEvent(int milliseconds){
    struct itimerspec ts;
    ts.it_value.tv_sec =  milliseconds / 1000;
    ts.it_value.tv_nsec = (milliseconds % 1000) * 1000000;
    ts.it_interval.tv_sec = milliseconds / 1000;
    ts.it_interval.tv_nsec = (milliseconds % 1000) * 1000000;
    
    if(timerfd_settime(timer_socket->Fd(), 0, &ts, NULL) < 0){
        return -1;
    }
    timer_socket->SetSocketNonBlocking();

    event->data.fd = timer_socket->Fd();
    events->events = EPOLLIN | EPOLLET;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, timer_socket->Fd(), event) < 0){
        return -1;
    }
    return 0;
}

int Server::Run(){
    std::cout << "Blocking and wait for epoll event..." << std::endl;
    int n;
    while(1){
        int thread_index = 0;
        //specifying timeout equals 0 causes epoll_wait to  return immediately, even if no events are available.
        //blocking timeout here...
        n = epoll_wait(efd, events, MAXEVENTS, 0);
        for(int i = 0; i < n; i++){
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (events[i].events &  EPOLLRDHUP)){
                //closing  a socket fd causes it to be removed from epoll sets...
                close(events[i].data.fd);
            } else if((events[i].events & EPOLLIN)){
                //connect event...
                if (events[i].data.fd == listen_socket->Fd()){
                    int new_fd;
                    while(1){
                        new_fd = listen_socket->Accept();
                        if (new_fd < 0){
                            //have accepted all connections... 
                            break;
                        }
                        Socket* new_socket = new Socket(new_fd);
                        new_socket->SetSocketNonBlocking();
                        event->data.fd = new_fd;
                        event->events = EPOLLIN | EPOLLET;
                        if(epoll_ctl(efd, EPOLL_CTL_ADD, new_fd, event) < 0){
                            std::cout << "failed to add fd into epoll event" << std::endl;
                        }
                    }
                } else if(events[i].data.fd == timer_socket->Fd()){
                    //timer event...
                    int ret = timer_socket->Recev();
                    if (ret != sizeof(uint64_t)){
                        std::cout << "timer fd error" << std::endl;
                        close(timer_socket->Fd());
                    }
                    TimeHandler();
                }
                else {
                    //Wrapper(events[i].data.fd);
                    // int fd = events[i].data.fd;
                    // //find a joinable thread...
                    // threads[thread_index % MAXTHREADS] = std::thread(&Server::Wrapper, this, fd);
                    // while(!threads[thread_index % MAXTHREADS].joinable()){
                    //     thread_index++;
                    //     threads[thread_index % MAXTHREADS] = std::thread(&Server::Wrapper, this, fd);
                    // }
                    // threads[thread_index % MAXTHREADS].join();
                    threads[i % MAXTHREADS]->Add(events[i].data.fd);
                }
            } else{
                //TODO...
                continue;
            }   
            //run thread pool... 
            for(auto th : threads){
                th->thread = std::thread(&Server::Wrapper, this, th->fds);
            }
            for(auto th : threads){
                th->Join();
            }
            for(auto th : threads){
                th->Clear();
            }
        }
    }
}

Server::~Server(){
    delete listen_socket;
    delete timer_socket;
    delete []events;
    close(efd);
}