#pragma once

#include <sstream>
#include <syslog.h>

namespace Ki {

extern int _log_level;

}

#ifdef SYSTEMD_LOGGER
#define _LOGGER(LEVEL, msg) ??(LEVEL, msg)
#elif defined(CERR_LOGGER)
#include <iostream>
#define _LOGGER(LEVEL, msg) std::cerr << msg << std::endl
#else
#define _LOGGER(LEVEL, msg) syslog(LEVEL, msg)
#endif

#ifndef NO_LOGS
#define _LOG(log_level, msg) \
    do { \
        if (log_level <= _log_level) { \
            std::stringstream stream; \
            stream << msg; \
            _LOGGER(log_level, stream.str().c_str()); \
        } \
    } while (false)

#else
#define _LOG(...)
#endif

#define LOGA(msg) _LOG(LOG_ALERT, msg)
#define LOGC(msg) _LOG(LOG_CRIT, msg)
#define LOGE(msg) _LOG(LOG_ERR, msg)

#ifndef LOG_ONLY_ERR

#define LOGW(msg) _LOG(LOG_WARNING, msg)
#define LOGN(msg) _LOG(LOG_NOTICE, msg)
#define LOGI(msg) _LOG(LOG_INFO, msg)
#define LOGD(msg) _LOG(LOG_DEBUG, msg)

#else

#define LOGW(msg)
#define LOGN(msg)
#define LOGI(msg)
#define LOGD(msg)

#endif
