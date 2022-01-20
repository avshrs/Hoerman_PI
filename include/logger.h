#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

template <typename T>

class Logger {
    private:
        std::ostringstream oss;
    public:
        template <typename T>
            Logger& Logger::operator<<(T a) {
                oss << a;
                return *this;
            }


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

