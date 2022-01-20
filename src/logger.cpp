#include "logger.h"
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
    auto handle = std::ofstream(fileName);
    handle <<__FILE__ << __LINE__ << __PRETTY_FUNCTION__ << out.str();
    out.clear();
}

