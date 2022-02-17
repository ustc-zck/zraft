// unordered_map limits
#include <iostream>
#include <unordered_map>
#include <string> 
#include <queue>

int main ()
{
  std::unordered_map<std::string, uint64_t> map;
  map["a"] = 0;
  std::cout << (map.find("a") == map.end())<< std::endl;
  map.erase("a");
  std::cout << (map.find("a") == map.end())<< std::endl;
  return 0;
}