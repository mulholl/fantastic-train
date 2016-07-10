#ifndef STRING_FUNCTIONS_HPP
#define STRING_FUNCTIONS_HPP

#include <string>
#include <locale>
#include <iostream>

std::string strToUpper(const std::string str);
void trimLWSpace(std::string &str);
void trimRWSpace(std::string &str);
std::string splitAtFirstWS(std::string &str1);

#endif