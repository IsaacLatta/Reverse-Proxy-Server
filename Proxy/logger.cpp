#include "logger.h"
#include "Session.h"

int duration_ms(std::chrono::time_point<std::chrono::system_clock>& start_time, std::chrono::time_point<std::chrono::system_clock>& end_time)
{
    return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());
}

std::string formatBytes(long bytes)
{
    if(bytes == 1) return " byte";

    std::string units[] = {" bytes", " KB", " MB", " GB", " TB"};
    int i = 0;
    double double_bytes = static_cast<double>(bytes);
    for(i = 0; i < 5 && double_bytes > 1024; i++)
    {
        double_bytes /= 1024;
    }
    return std::to_string(static_cast<int>(double_bytes)) + units[i];
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

void trim_user_agent(std::string& user_agent)
{
    std::string user_agent_label = "User-Agent: ";
    if (user_agent.find(user_agent_label) == 0) 
    {
        user_agent = user_agent.substr(user_agent_label.length());
    }
}

std::string get_identifier(const std::string& user_agent)
{
    std::string result;
    std::size_t identifier_end = user_agent.find("(");
    if (identifier_end != std::string::npos) 
    {
        result = user_agent.substr(0, identifier_end - 1); 
    }
    return result;
}

std::string get_os(const std::string& user_agent)
{
    std::string result;
    std::size_t os_start = user_agent.find("(");
    std::size_t os_end = user_agent.find(")", os_start);

    if (os_start != std::string::npos && os_end != std::string::npos) 
    {
        std::string os_info = user_agent.substr(os_start + 1, os_end - os_start - 1);
        result += " on " + os_info;
    }
    return result;
}

std::string get_browser(const std::string user_agent)
{   
    std::string result = "";
    std::size_t browser_start = std::string::npos;
    std::string browser_info;
    std::vector<std::string> browsers = {"Chrome/", "Firefox/", "Safari/", "Edge/", "Opera/", "Brave/", "Chromium/"};
    for (const auto& browser : browsers) 
    {
        browser_start = user_agent.find(browser);
        if (browser_start != std::string::npos) 
        {
            std::size_t browser_end = user_agent.find(" ", browser_start);
            browser_info = user_agent.substr(browser_start, browser_end - browser_start);
            break; 
        }
    }
    if (!browser_info.empty()) 
    {
        result += " " + browser_info;
    }
    return result;
}

std::string logger::get_user_agent(const std::vector<char>& buffer)
{
    int ua_start, ua_end;
    std::string buffer_str(buffer.begin(), buffer.end());
    if((ua_start = buffer_str.find("User-Agent")) == std::string::npos)
        return "";
    if((ua_end = buffer_str.find("\r\n", ua_start)) ==std::string::npos)
        return "";

    std::string user_agent =  buffer_str.substr(ua_start, ua_end - ua_start);
    trim_user_agent(user_agent);
    return get_identifier(user_agent) + get_os(user_agent) + get_browser(user_agent);
}

std::string logger::get_header(const std::vector<char>& buffer)
{
    int end;
    std::string header_buffer(buffer.begin(), buffer.end());
    if((end = header_buffer.find("\r\n")) == std::string::npos || end < 0)
        return "";
    return header_buffer.substr(0,end);
}

void logger::log(const std::shared_ptr<Session>& session, std::string type)
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

    if(!session)
    {
        log_msg = "[" + time +"] " + type + "\n";
    }
    else
    {
        log_msg = "[" + time +"] " +  type + " [client " + session->get_socket()->get_IP() + "] " + 
        "\"" + session->req_header + "\" \"" + session->user_agent + 
        "\" [Latency: " + std::to_string(duration_ms(session->start_time, session->end_time)) + 
        "ms, RTT: " + std::to_string(duration_ms(session->RTT_start_time, session->end_time)) + 
        "ms, Size: " + formatBytes(session->bytes_transferred) + "]\n" ;
    }
    
    log_file << log_msg;
    log_file.close();
    std::cout << log_msg; 
}

void logger::debug(std::string type, std::string s1, std::string s2, std::string file, std::size_t line)
{
    std::cout << "[DEBUG " << file << ":" << line << "] " << type << ": " << s1 << ": " << s2 << "\n";
}

