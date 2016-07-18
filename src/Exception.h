#pragma once

#include <exception>
#include <string>

namespace Ki {

class Exception : public std::exception
{
public:
    Exception(const std::string& msg) : m_msg(msg) {}
    Exception(const char* msg) : m_msg(msg) {}
    virtual ~Exception() throw (){}

    virtual const char* what() const throw (){
       return m_msg.c_str();
    }

protected:
    Exception() = default;

    std::string m_msg;
};

}
