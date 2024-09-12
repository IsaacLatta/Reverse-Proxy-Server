#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>
#include <cmath>

class Session;

namespace logger
{

void debug(std::string type, std::string s1, std::string s2, std::string file, std::size_t line);

void log(const std::shared_ptr<Session>& session, std::string type);

std::string get_header(const std::vector<char>& buffer);
std::string get_user_agent(const std::vector<char>& buffer);

}

#endif