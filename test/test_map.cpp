// unordered_map limits
#include <iostream>
#include <unordered_map>
#include <string> 
#include <queue>

int main ()
{
  std::unordered_map<uint64_t, uint64_t> map;
  std::cout << map[0] << std::endl;
  return 0;
}