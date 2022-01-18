#include "logger.h"

Logger& Logger::get()
{
    if(!instance)
        instance = new Logger();
    return *instance;
}

void Logger::flush()
{
    save();
}


void Logger::save()
{
    auto handle = std::ofstream("fileName");
    handle <<__FILE__ << __LINE__ << __PRETTY_FUNCTION__ << out.str();
    out.clear();
}

