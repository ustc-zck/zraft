#include <iostream>
#include <vector>

int main(){
    std::vector<int> v;
    v.erase(v.begin(), v.end());
    std::cout << "size of vector is " << v.size() << std::endl;
    for(int i = 0; i < 10; i++){
        v.push_back(i);
    }
    std::cout << "size of vector is " << v.size() << std::endl;

    v.erase(v.begin(), v.begin() + 0);
    std::cout << "size of vector is " << v.size() << std::endl;
    v.erase(v.begin(), v.begin() + 1);
    std::cout << "size of vector is " << v.size() << std::endl;

    std::cout << "last element is " << v.back() << std::endl;
    
    v.erase(v.begin(), v.end());
    std::cout << v.size() << std::endl;
    return 0;
}