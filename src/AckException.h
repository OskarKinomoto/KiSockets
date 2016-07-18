#pragma once

#include <Exception.h>

namespace Ki {

class AckException : public Exception
{
public:
    AckException(const std::string& msg = "AckCode incorrect") {
        m_msg = msg;
    }
};

}
