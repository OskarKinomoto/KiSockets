#include "Socket.h"

#include <iostream>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <stdexcept>
#include <unistd.h>

#include "AckException.h"
#include "ErrnoException.h"
#include "Log.h"

#if EAGAIN == EWOULDBLOCK
#define case_EAGAIN case EAGAIN
#else
#define case_EAGAIN case EAGAIN: case EWOULDBLOCK
#endif

namespace Ki {

namespace Socket {

namespace {

constexpr uint16_t ackCode = 0xACDC;

}

std::ostream *Socket::io_err = &std::cerr;

Socket::Socket(Domain domain, Type type, int protocol) : m_domain(domain),
    m_type(type), m_protocol(protocol), m_bind_path("")
{
    m_fd = socket(static_cast<int>(domain), static_cast<int>(type), protocol);

    if (m_fd == -1)
        throw ErrnoException("socket creation failed: ");

    LOGD("Created socket <" << info() << ">");
}

Socket::~Socket()
{
    close();
    if (!m_bind_path.empty()) {
        LOGD("Removing socket '" << m_bind_path << "' from filesystem");
        unlink(m_bind_path.c_str());
    }

}

void Socket::bind(const std::string &path)
{
    sockaddr_un local;
    socklen_t length;
    int result;

    if (path.size() >= sizeof(local.sun_path))
        throw std::invalid_argument("Socket path is too long");

    local.sun_family = static_cast<int>(m_domain);
    strcpy(local.sun_path, path.c_str());

    length = sizeof(local.sun_family) + path.size() + 1;

    result = TEMP_FAILURE_RETRY(::bind(m_fd,
                                       reinterpret_cast<sockaddr*>(&local),
                                       length));

    if (result == -1)
        throw ErrnoException("Binding socket '" + std::to_string(m_fd) +
                             "' to '" + path + "'' failed");

    m_bind_path = path;
    LOGD("Bind socket <" << info() << ">");
}

void Socket::listen(int backlog)
{
    int result = ::listen(m_fd, backlog);
    if (result == -1)
        throw ErrnoException("Listening on socket '" + std::to_string(m_fd) +
                             "' failed");
    LOGD("Listen socket <" << info() << ">");
}

Socket Socket::accept(sockaddr *addr, socklen_t *addlen)
{
    int result;
    result = TEMP_FAILURE_RETRY(::accept(m_fd, addr, addlen));
    if (result == -1) {
        switch (errno) {
case_EAGAIN:
        break;
        default:
            throw ErrnoException("Accepting on socket '"+ std::to_string(m_fd) +
                                 "' failed");
        }
    }
    Socket s = Socket(m_domain, m_type, m_protocol, result);
    LOGD("Accepted socket <" << s.info() << "> on socket <" << info() << ">");
    return s;
}

void Socket::connect(const std::string &path)
{
    sockaddr_un remote;
    socklen_t length;
    int result;

    if (path.size() >= sizeof(remote.sun_path))
        throw std::invalid_argument("Socket path is too long");

    remote.sun_family = static_cast<int>(m_domain);
    strcpy(remote.sun_path, path.c_str());

    length = sizeof(remote.sun_family) + path.size() + 1;

    result = TEMP_FAILURE_RETRY(::connect(m_fd,
                                          reinterpret_cast<sockaddr*>(&remote),
                                          length));

    if (result == -1)
        throw ErrnoException("Connecting socket '" + std::to_string(m_fd) +
                             "' to '" + path + "' failed");
}

bool Socket::close()
{
    int result = 0;
    if (isValid()) {
        result = TEMP_FAILURE_RETRY(::close(m_fd));
        if (result == -1) {
            int err = errno;
            LOGE("Closing socket <" << info() << "> failed: " <<
                 Ki::ErrnoException::getErrnoString(err));
        } else {
            LOGD("Closed socket <" << info() << ">");
        }

        m_fd = -1;
    }
    return result == 0;
}

bool Socket::isValid()
{
    return m_fd != -1;
}

void Socket::send(const void *data, size_t size, int flags)
{
    int result;
    size_t dataSend = 0;

    while (dataSend < size) {
        result = ::send(m_fd, static_cast<const char*>(data) + dataSend,
                        size - dataSend, flags);
        if (result == -1)
            throw ErrnoException("Sending data to socket <" +
                                 info() + "> failed");
        dataSend += result;
        LOGD("Send " << dataSend << "/" << size << " on socket <" << info() <<
             ">");
    }

}

void Socket::recv(void *data, size_t size, int flags)
{
    int result;
    size_t dataRecieved = 0;

    while(dataRecieved < size) {
        result = ::recv(m_fd, static_cast<char*>(data) + dataRecieved,
                        size - dataRecieved, flags);

        if (result == -1)
            throw ErrnoException("Recieving data from socket <" +
                                 info() + "> failed");

        dataRecieved += result;

        LOGD("Recieved " << dataRecieved << "/" << size << " on socket <" <<
             info() << ">");
    }
}

void Socket::sendAck()
{
    send(&ackCode, sizeof(ackCode));
    LOGD("Send ack " << ackCode << " to socket <" << info() << ">");
}

void Socket::recvAck()
{
    auto ack = decltype(ackCode)(0);
    recv(&ack, sizeof(ack));

    if (ack != ackCode) {
        LOGD("Recieved incorrect ack " << ack << "instead of " << ackCode <<
             " from socket <" << info() << ">");
        throw AckException();
    }

    LOGD("Recieved ack " << ackCode << " from socket <" << info() << ">");
}

Socket::Socket(Domain domain, Type type, int protocol, int fd) :
    m_domain(domain), m_type(type), m_protocol(protocol), m_fd(fd),
    m_bind_path("")
{
    LOGD("Constructed socket <" << info() << ">");
}

std::string Socket::info()
{
    std::stringstream s;
#ifdef BUILD_TYPE_DEBUG
    s << "fd " << m_fd << " | ptr " << reinterpret_cast<size_t>(this)
      << " | path '" << m_bind_path << "'";
#else
    s << m_fd;
#endif
    return s.str();
}

}

}
