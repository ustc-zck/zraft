#pragma once

#include <vector>
#include <chrono>
#include <time.h> 
#include <string>

std::vector<std::string> SplitStr(std::string str, char delimiter);

uint64_t GetCurrentMillSeconds();

float GenerateRandomNumber();