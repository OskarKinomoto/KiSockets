#pragma once

#include <cassert>
#include <ostream>
#include <string>

#include <sys/socket.h>
#include <sys/types.h>

#include <Exception.h>

namespace Ki {

namespace Socket {

enum class Domain : int {
    Unix = AF_UNIX,
    Local = AF_LOCAL,
    INet = AF_INET,
    INet6 = AF_INET6,
    IPX = AF_IPX,
    NetLink = AF_NETLINK,
    X25 = AF_X25,
    AX25 = AF_AX25,
    ATMPVC = AF_ATMPVC,
    AppleTalk = AF_APPLETALK,
    Packet = AF_PACKET,
    ALG = AF_ALG,
};

enum class Type : int {
    Stream = SOCK_STREAM,
    Datagram = SOCK_DGRAM,
    SequencePacket = SOCK_SEQPACKET,
    RAW = SOCK_RAW,
    RDM = SOCK_RDM,
    NonBlock = SOCK_NONBLOCK,
    CloseOnExec = SOCK_CLOEXEC
};

inline int operator &(Type a, Type b) {
    return static_cast<int>(a) & static_cast<int>(b);
}

inline Type operator |(Type a, Type b) {
    return static_cast<Type>(static_cast<int>(a) | static_cast<int>(b));
}

class Socket
{
public:
    Socket(Domain domain, Type type, int protocol = 0);
    Socket(const Socket&) = default;
    Socket(Socket&&) = default;
    ~Socket();

    void bind(const std::string& path);
    void listen(int backlog);
    Socket accept(sockaddr *addr = nullptr,
                socklen_t *addlen = nullptr);
    void connect(const std::string& path);

    bool close();

    bool isValid();

    void send(const void *data, size_t size, int flags = 0);
    void recv(void *data, size_t size, int flags = 0);

    void sendAck();
    void recvAck();

    std::string info();

protected:
    int m_fd;
    Domain m_domain;
    Type m_type;
    int m_protocol;

    std::string m_bind_path;

    static std::ostream *io_err;
    Socket(Domain domain, Type type, int protocol, int fd);
};

}

}
