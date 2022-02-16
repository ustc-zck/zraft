#include "../src/utils.h"
#include <iostream>

//g++ -o test_utils test_utils.cpp ../src/utils.cpp 

int main(){
    //auto result = SplitStr("\tPUT\tHELLO\tWORLD\t", '\t');
    auto result = SplitStr("test", '\t');

    //auto result = SplitStr("PUT\tHELLO\tWORLD\t", '\t');
    for(auto item : result){
        std::cout << item << "\t";
    }
    std::cout << std::endl;
    
    return 0;
}