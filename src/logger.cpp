#include "logger.h"
#include <chrono>  // chrono::system_clock
#include <ctime>   // localtime
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <string>  // string


Logger* Logger::instance = nullptr; // or NULL, or nullptr in c++11

Logger& Logger::log()
{
    if(!Logger::instance)
        Logger::instance = new Logger();
    return *Logger::instance;
}

void Logger::flush()
{
    save();
}


void Logger::save()
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto handle = std::ofstream(fileName, std::ios_base::app);
    handle << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << " | " << out.str();
    out.clear();
}




class Logger {
    private:
        ostringstream oss;
    public:
        template <typename T>
        Logger& operator<<(T a);

    Logger& operator<<( std::ostream&(*f)(std::ostream&) )
    {
        if( f == std::endl )
        {
            std::cout << "12-09-2009 11:22:33" << oss.str() << std::endl;   
            oss.str("");
        }
        return *this;
    }
};

template <typename T>
Logger& Logger::operator<<(T a) {
    oss << a;
    return *this;
}

void functionTest(void) {
    Logger log;
    log << "Error: " << 5 << " seen" << std::endl;
}

int main()
{
    functionTest();
}


