
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <iomanip>



// Content-Length: 42
size_t get_content_len(char* buffer, std::size_t buffer_size)
{
    std::string header(buffer, buffer_size);
    std::string content_len_str = "Content-Length:";
    size_t content_len = 0;

    size_t pos = header.find(content_len_str);
    if(pos != std::string::npos)
    {
        pos += content_len_str.length();
        size_t end_pos = header.find("\r\n", pos);
        if(end_pos != std::string::npos)
        {
            std::string content_len_val = header.substr(pos, end_pos - pos);
            content_len = atoi(content_len_val.data());
        }
    }

    return content_len;
}

size_t get_header_size(char* buffer, std::size_t buffer_size)
{
    std::string header(buffer, buffer_size);

    size_t pos = header.find("\r\n\r\n");
    if(pos != std::string::npos)
    {
        return pos + 4;
    }
    return 0;
}


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

void log_it(std::string address, std::string type, std::string s1, std::string s2)
{
    std::string time = getTime();
    std::cout << "[" << time << "] " << std::setw(2) <<  "[" << address << "] " << std::setw(2) << type << ": " << s1 << ": " << s2 << "\n"; 
}

int main()
{
    /*
    std::string http_header("HTTP/1.1 200 OK\r\nContent-Length: 42\r\nConnection: close\r\n\r\n");
    size_t content_len = get_content_len(http_header.data(), http_header.length());
    std::cout << http_header << "Content Length Detected: " << content_len << "\n";
    std::cout << "Header size: " << get_header_size(http_header.data(), http_header.length()) 
    << "\nString size: " << http_header.length() << "\n";
    std::cout << "Threads Supported: " << std::thread::hardware_concurrency() << "\n";
    */
    std::string time = getTime();
    std::cout << time << "\n" << "\n" << "proxy-" << getDate(time) << ".log"<< "\n";
    log_it("201.176.67.72", "INFO", "received bytes", std::to_string(536));
    log_it("204.136.1.72", "INFO", "client connected", "");
    log_it("182.100.1.72", "ERROR", "async_read", "bad file descriptor");
    return 0;
}