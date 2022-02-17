#include "utils.h"

std::vector<std::string> SplitStr(std::string str, char delimiter){
    std::vector<std::string> result;
    int i, j = 0;
    for(i = 0; i < str.size(); i++){
        if(str[i] == delimiter){
            std::string item = str.substr(j, i - j);
            if(item.length() > 0){
                result.push_back(item);
            }
            j = i + 1;
        }
    }
    if(str[str.length() - 1] != delimiter){
        result.push_back(str.substr(j));
    }
    return result;
}

uint64_t GetCurrentMillSeconds(){
    uint64_t now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    return now;
}

float GenerateRandomNumber(){
    srand((unsigned)time(NULL) ); 
 
    return (float) rand()/RAND_MAX;
}