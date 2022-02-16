#include "thread.h"

Thread::Thread(){
}

Thread::~Thread(){
}

void Thread::Add(int fd){
    fds.push_back(fd);
}

void Thread::Join(){
    thread.join();
}

void Thread::Clear(){
    fds.clear();
}