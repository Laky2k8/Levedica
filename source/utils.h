#pragma once

#include <vector>
#include <string>
#include <sstream>

std::vector<std::string> split(std::string str, char delimiter)
{
    std::stringstream ss(str);
    std::vector<std::string> res;
    std::string token;
    while (std::getline(ss, token, delimiter)) 
	{
        res.push_back(token);
    }
    return res;
}