#pragma once

#include <string>
#include <sstream>
#include <fstream>

class Logger
{
  public:
    static Logger& log();
    
    void flush();
    
    template<typename T>
    Logger& operator<< (const T& str)
    {
        out << str;
        std::size_t found = out.str().find("\n");
        if(found != std::string::npos)
            save();
        return *this;
    }

    

  private:
    void save();

  private:
    const std::string fileName = "Hoermann.log";
    static Logger* instance;
    std::ostringstream out;

};
