#include <gtest/gtest.h>

#include <Socket.h>
#include <ErrnoException.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <random>

#include <iostream>


using namespace Ki::Socket;

const std::string path = "/tmp/KiTest.socket";

class socket : public ::testing::Test {
public:
    void SetUp() {
        unlink(path.c_str());
    }
};

namespace Ki {

std::random_device rd;
std::default_random_engine e1(rd());
std::uniform_int_distribution<int> uniform_dist;
int rand() {
    return uniform_dist(e1);
}

}

TEST_F(socket, test_unix_domain_creation) {
    Socket s(Domain::Unix, Type::Stream);
    ASSERT_TRUE(s.isValid());
}

TEST_F(socket, test_unix_domain_creation_or) {
    Socket s(Domain::Unix, Type::Stream | Type::NonBlock);
    ASSERT_TRUE(s.isValid());
}

TEST_F(socket, test_unix_domain_creation_or2) {
    Socket s(Domain::Unix, Type::Stream | Type::NonBlock | Type::CloseOnExec);
    ASSERT_TRUE(s.isValid());
}

TEST_F(socket, test_unix_domain_creation_or3) {
    Socket s(Domain::Unix, Type::NonBlock | Type::CloseOnExec | Type::Stream);
    ASSERT_TRUE(s.isValid());
}

TEST_F(socket, bind) {
    Socket s(Domain::Unix, Type::Stream);
    ASSERT_TRUE(s.isValid());
    s.bind(path);
}

TEST_F(socket, listen) {
    Socket s(Domain::Unix, Type::Stream);
    ASSERT_TRUE(s.isValid());
    s.bind(path);
    s.listen(10);
}

TEST_F(socket, connect) {
    Socket s(Domain::Unix, Type::Stream);
    ASSERT_TRUE(s.isValid());

    s.bind(path);
    s.listen(10);

    int pid = fork();
    ASSERT_NE(pid, -1);
    if (pid) {
        Socket d = s.accept();
        ASSERT_TRUE(d.isValid());
    } else {
        Socket c(Domain::Unix, Type::Stream);
        ASSERT_TRUE(c.isValid());
        c.connect(path);
        _exit(0);
    }
}

TEST_F(socket, send_and_recieve_char) {
    char data = (Ki::rand() & 126) + 1;

    Socket s(Domain::Unix, Type::Stream);
    s.bind(path);
    s.listen(10);

    auto pid = fork();
    ASSERT_NE(pid, -1);

    if (pid) {
        Socket d = s.accept();
        ASSERT_TRUE(d.isValid());
        char buf = '\0';
        d.recv(&buf, sizeof(data));
        ASSERT_EQ(data, buf);
    } else {
        Socket c(Domain::Unix, Type::Stream);
        c.connect(path);
        c.send(&data, sizeof(data));
        _exit(0);
    }
}

TEST_F(socket, send_and_recieve_int) {
    int data = Ki::rand();

    Socket s(Domain::Unix, Type::Stream);
    s.bind(path);
    s.listen(10);

    auto pid = fork();
    ASSERT_NE(pid, -1);

    if (pid) {
        Socket d = s.accept();
        ASSERT_TRUE(d.isValid());

        int buf = 0;
        d.recv(&buf, sizeof(data));
        ASSERT_EQ(data, buf);
    } else {
        Socket c(Domain::Unix, Type::Stream);
        c.connect(path);
        c.send(&data, sizeof(data));
        _exit(0);
    }
}

TEST_F(socket, send_and_recieve_4kb) {
    const size_t len = 4 * 1024;
    char data[len];
    for (int i = 0; i < len; ++i)
        data[i] = Ki::rand();

    Socket s(Domain::Unix, Type::Stream);
    s.bind(path);
    s.listen(10);

    auto pid = fork();
    ASSERT_NE(pid, -1);

    if (pid) {
        Socket d = s.accept();
        ASSERT_TRUE(d.isValid());

        char buf[len];
        memset(buf, 0, len);
        d.recv(&buf, len);

        for (int i = 0; i < len; ++i)
            ASSERT_EQ(data[i], buf[i]);

    } else {
        Socket c(Domain::Unix, Type::Stream);
        c.connect(path);
        c.send(&data, len);
        _exit(0);
    }
}

TEST_F(socket, send_and_recieve_2mb) {
    const size_t len = 1024 * 1024 * 2;
    char data[len];
    for (size_t i = 0; i < len; ++i)
        data[i] = Ki::rand();

    Socket s(Domain::Unix, Type::Stream);
    s.bind(path);
    s.listen(10);

    auto pid = fork();
    ASSERT_NE(pid, -1);

    if (pid) {
        Socket d = s.accept();
        ASSERT_TRUE(d.isValid());

        char buf[len];
        memset(buf, 0, len);
        d.recv(&buf, len);

        for (size_t i = 0; i < len; ++i)
            ASSERT_EQ(data[i], buf[i]);

    } else {
        Socket c(Domain::Unix, Type::Stream);
        c.connect(path);
        c.send(&data, len);
        _exit(0);
    }
}

TEST_F(socket, ack) {
    const size_t len = 1024 * 1024 * 2;
    char data[len];
    for (size_t i = 0; i < len; ++i)
        data[i] = Ki::rand();

    Socket s(Domain::Unix, Type::Stream);
    s.bind(path);
    s.listen(10);

    auto pid = fork();
    ASSERT_NE(pid, -1);

    if (pid) {
        Socket d = s.accept();
        ASSERT_TRUE(d.isValid());
        d.recvAck();
    } else {
        Socket c(Domain::Unix, Type::Stream);
        c.connect(path);
        c.sendAck();
        _exit(0);
    }
}
