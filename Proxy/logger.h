#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>

class Session;

namespace logger
{

void debug(std::string type, std::string s1, std::string s2, std::string file, std::size_t line);

//void log(std::string address, std::string type, std::string s1, std::string s2, std::string file, std::size_t line);
void log(const std::shared_ptr<Session>& session, std::string type);

}

#endif