#include <thread>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <unistd.h>

using namespace std;

void doSomething(int n) {
    sleep(10 - n);
    cout << n  << "\n";
}

/**
 * Spawns n threads
 */

int main()
{
    int n = 10;
     std::vector<thread> threads(n);
    // spawn n threads:
    while(1){
        std::cout << "into loop..." << std::endl;
         for (int i = 0; i < n; i++) {
            threads[i] = std::thread(doSomething, i + 1);
            threads[i].join(); 
        }
        // for (auto& th : threads) {
        //     std::cout << std::string(th.get_id()) << std::endl;
        //     th.join();
        //     std::cout << th.get_id() << std::endl;
        // }
        // std::cout << "out of loop..." << std::endl;
    }

    std::cout << "program end" << std::endl;

    return 0;
}