#pragma once

#include <string>
#include <sstream>
#include <fstream>

class Logger
{
  public:
    static void log();
    
    void flush();
    
    template<typename T>
    Logger& operator<< (const T& str)
    {
        out << str;
        return this.log();
    }

    

  private:
    void save();

  private:
    const std::string fileName = "Hoermann.log";
    std::ostringstream out;

};
