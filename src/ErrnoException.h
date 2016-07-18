#pragma once

#include <cstring>

#include <Exception.h>

namespace Ki {

class ErrnoException : public Exception
{
public:
    static std::string getErrnoString(int err = errno) {
        const size_t buflen = 1024;
        char buf[buflen];
        char *err_msg;
#if (_POSIX_C_SOURCE >= 200112L) && !  _GNU_SOURCE
        int res = strerror_r(err, nuf, buflen);
        if (res != 0) {
            strcpy(buf, "strerror_r error");
        }
        err_msg = buf;
#else
        err_msg = strerror_r(err, buf, buflen);
#endif
        return err_msg;
    }

    ErrnoException(const std::string& msg, int err = errno) {
        m_msg = msg + ": " + std::to_string(err) + " – " + getErrnoString(err);
    }
};

}
