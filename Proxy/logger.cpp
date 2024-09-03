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
        log_msg = "[" + time +"] " +  type + " [client " + session->clientIP() + "] " + " Latency: " + 
        std::to_string(duration_ms(session->start_time, session->end_time)) + "ms, RTT: " + 
        std::to_string(duration_ms(session->RTT_start_time, session->end_time))  + "ms, Data Transferred: " + formatBytes(session->bytes_transferred) + "\n";
    }
    
    log_file << log_msg;
    log_file.close();
    std::cout << log_msg; 
}

void logger::debug(std::string type, std::string s1, std::string s2, std::string file, std::size_t line)
{
    std::cout << "[LOG " << file << ":" << line << "] " << type << ": " << s1 << ": " << s2 << "\n";
}
