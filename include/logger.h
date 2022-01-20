#pragma once

#include <string>
#include <sstream>
#include <fstream>

class Logger
{
  public:
    static Logger& get();
    
    void flush();
    
    template<typename T>
    Logger& operator<< (const T& str)
    {
        out << str;
        if(out.str().length() > 100)
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
