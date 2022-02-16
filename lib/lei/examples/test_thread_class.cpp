#include "../src/thread.h"
#include <iostream>
#include <vector>

void Test(std::vector<int> fds){
    for(auto fd : fds){
        std::cout << fd << std::endl;
    }
}

// int main(){
    
//     Thread* th = new Thread();
//     th->Proc = &Test;
    
//     for(int i = 0; i < 10; i++){
//         th->Add(i);
//     }
//     th->Join();

//     return 0;
// }

int main(){
    std::vector<Thread*> ths;
    for (int i = 0; i < 1024; i++){
        Thread* s = new Thread();
        ths.push_back(s);
    }
    return 0;
}