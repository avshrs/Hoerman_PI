#pragma once

#include <string>
#include <sstream>
#include <fstream>

class ToolLogger
{
  public:

    // standard operator<< //
    template<typename T>
    ToolLogger& operator<< (const T& str)
    {
        out << str;
        return *this;
    }

    // operator<< for taking the std::endl manipulator //
    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
    typedef CoutType& (*StandardEndLine)(CoutType&);
    ToolLogger& operator<<(StandardEndLine manip)
    {
        // save fileName, line and function to the file //
        // and all what is already in stringstream //
        // clear stringstream //
        return *this;
    }

    static std::string fileName;
    static int line;
    static std::string function;

  private:

    std::ofstream file;
    std::stringstream out;
};

std::string ToolLogger::fileName;
int ToolLogger::line;
std::string ToolLogger::function;