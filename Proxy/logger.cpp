#include "logger.h"


std::string getTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm local_time = *std::localtime(&time);

    std::ostringstream oss;
    oss << std::put_time(&local_time, "%Y-%m-%d %H:%M");
    return oss.str();
}

std::string getDate(std::string fmt_time)
{
    return fmt_time.substr(0, fmt_time.find(" "));
}

void logger::log(std::string address, std::string type, std::string s1, std::string s2, std::string file, std::size_t line)
{
    std::fstream log_file;
    std::string log_msg;
    std::string time = getTime();
    std::string file_name = "proxy-" + getDate(time) + ".log";
    log_file.open(file_name, std::fstream::app);
    if(!log_file.is_open())
    {
        logger::debug("ERROR", "fstream.open", "failed to open " + file_name, __FILE__, __LINE__ );
        return;
    }
    log_msg = "[" + time +"] " + "[" + address + "]" + type + ": " + s1 + ": " + s2 + "\n";
    log_file << log_msg;
    log_file.close();

    log_msg = "[" + file + ":" + std::to_string(line) + "] " + log_msg;
    std::cout << log_msg; 
}

void logger::debug(std::string type, std::string s1, std::string s2, std::string file, std::size_t line)
{
    std::cout << "[LOG " << file << ":" << line << "] " << type << ": " << s1 << ": " << s2 << "\n";
}
