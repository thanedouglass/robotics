#include <gtest/gtest.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <atomic>

#include "rix/ipc/fifo.hpp"

using namespace rix::ipc;

class FifoTest : public ::testing::Test {
protected:
    std::string fifo_path = "test_fifo";

    void SetUp() override {
        unlink(fifo_path.c_str());  // Clean slate
    }

    void TearDown() override {
        unlink(fifo_path.c_str());
    }
};

// Test default constructor
TEST_F(FifoTest, DefaultConstructor) {
    Fifo fifo;
    EXPECT_FALSE(fifo.ok());
}

// Test file creation with Mode::READ
TEST_F(FifoTest, ReadModeCreatesFifo) {
    std::thread writer([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Fifo f(fifo_path, Fifo::Mode::WRITE);
        std::string message = "hi";
        f.write(reinterpret_cast<const uint8_t*>(message.c_str()), message.size());
    });

    Fifo f(fifo_path, Fifo::Mode::READ);
    EXPECT_TRUE(f.ok());
    EXPECT_EQ(f.mode(), Fifo::Mode::READ);

    struct stat st;
    EXPECT_EQ(stat(fifo_path.c_str(), &st), 0);
    EXPECT_TRUE(S_ISFIFO(st.st_mode));

    std::vector<uint8_t> buffer(2);
    EXPECT_EQ(f.read(buffer.data(), 2), 2);
    EXPECT_EQ(std::string(buffer.begin(), buffer.end()), "hi");

    writer.join();
}

// Test file creation with Mode::WRITE
TEST_F(FifoTest, WriteModeCreatesFifo) {
    std::thread reader([&] {
        Fifo f(fifo_path, Fifo::Mode::READ);
        std::vector<uint8_t> buffer(5);
        f.read(buffer.data(), buffer.size());
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    Fifo f(fifo_path, Fifo::Mode::WRITE);
    EXPECT_TRUE(f.ok());
    EXPECT_EQ(f.mode(), Fifo::Mode::WRITE);
    std::string message = "hi";
    f.write(reinterpret_cast<const uint8_t*>(message.c_str()), message.size());
    
    reader.join();
}

// Test non-blocking read
TEST_F(FifoTest, NonBlockingReadNoDataReturnsImmediately) {
    Fifo r(fifo_path, Fifo::Mode::READ, true);
    Fifo w(fifo_path, Fifo::Mode::WRITE);
    EXPECT_TRUE(r.is_nonblocking());

    std::vector<uint8_t> buffer(10);
    ssize_t result = r.read(buffer.data(), buffer.size());
    EXPECT_EQ(result, -1);  // Should be EAGAIN/EWOULDBLOCK
}

// Test copy constructor
TEST_F(FifoTest, CopyConstructorPreservesPathAndMode) {
    std::thread dummy_writer([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Fifo w(fifo_path, Fifo::Mode::WRITE);
    });

    Fifo r(fifo_path, Fifo::Mode::READ);
    Fifo copy(r);

    EXPECT_TRUE(copy.ok());
    EXPECT_NE(copy.fd(), r.fd());
    EXPECT_EQ(copy.pathname(), fifo_path);
    EXPECT_EQ(copy.mode(), Fifo::Mode::READ);

    dummy_writer.join();
}

// Test move constructor
TEST_F(FifoTest, MoveConstructorTransfersOwnership) {
    std::thread dummy_writer([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Fifo w(fifo_path, Fifo::Mode::WRITE);
    });

    Fifo r(fifo_path, Fifo::Mode::READ);
    int fd = r.fd();
    Fifo moved(std::move(r));

    EXPECT_EQ(moved.fd(), fd);
    EXPECT_FALSE(r.ok());
    EXPECT_EQ(moved.pathname(), fifo_path);
    EXPECT_EQ(moved.mode(), Fifo::Mode::READ);

    dummy_writer.join();
}

// Test assignment operators
TEST_F(FifoTest, CopyAndMoveAssignment) {
    std::thread dummy_writer([&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        Fifo w(fifo_path, Fifo::Mode::WRITE);
    });

    Fifo a(fifo_path, Fifo::Mode::READ);
    Fifo b;
    b = a;
    EXPECT_TRUE(b.ok());
    EXPECT_EQ(b.pathname(), fifo_path);
    EXPECT_EQ(b.mode(), Fifo::Mode::READ);

    Fifo c;
    c = std::move(a);
    EXPECT_TRUE(c.ok());
    EXPECT_FALSE(a.ok());
    EXPECT_EQ(c.pathname(), fifo_path);
    EXPECT_EQ(c.mode(), Fifo::Mode::READ);

    dummy_writer.join();
}
