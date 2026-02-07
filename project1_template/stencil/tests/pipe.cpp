#include <gtest/gtest.h>
#include <vector>
#include <thread>
#include <chrono>

#include "rix/ipc/pipe.hpp"

using namespace rix::ipc;

// Test default constructor
TEST(PipeTest, DefaultConstructor) {
    Pipe pipe;
    EXPECT_FALSE(pipe.ok());
}

// Test factory method and basic communication
TEST(PipeTest, PipeFactoryCreatesValidPair) {
    auto [reader, writer] = Pipe::create();
    
    EXPECT_TRUE(reader.ok());
    EXPECT_TRUE(writer.ok());

    EXPECT_TRUE(reader.is_read_end());
    EXPECT_TRUE(writer.is_write_end());

    const std::string msg = "pipe test";
    writer.write(reinterpret_cast<const uint8_t*>(msg.data()), msg.size());

    std::vector<uint8_t> buffer(msg.size());
    ssize_t bytes = reader.read(buffer.data(), buffer.size());

    EXPECT_EQ(bytes, msg.size());
    EXPECT_EQ(std::string(buffer.begin(), buffer.end()), msg);
}

// Test read blocks until data is written (with thread)
TEST(PipeTest, PipeBlocksUntilDataAvailable) {
    auto [reader, writer] = Pipe::create();
    const std::string msg = "block";

    std::thread writer_thread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        writer.write(reinterpret_cast<const uint8_t*>(msg.data()), msg.size());
    });

    std::vector<uint8_t> buffer(msg.size());
    ssize_t bytes = reader.read(buffer.data(), buffer.size());

    EXPECT_EQ(bytes, msg.size());
    EXPECT_EQ(std::string(buffer.begin(), buffer.end()), msg);

    writer_thread.join();
}

// Test move constructor
TEST(PipeTest, MoveConstructorTransfersOwnership) {
    auto [reader, writer] = Pipe::create();
    int fd = reader.fd();

    Pipe moved(std::move(reader));
    EXPECT_TRUE(moved.ok());
    EXPECT_EQ(moved.fd(), fd);
    EXPECT_FALSE(reader.ok());
    EXPECT_TRUE(moved.is_read_end());
}

// Test move assignment
TEST(PipeTest, MoveAssignmentTransfersOwnership) {
    auto [reader, writer] = Pipe::create();
    Pipe moved;
    moved = std::move(writer);
    EXPECT_TRUE(moved.ok());
    EXPECT_FALSE(writer.ok());
    EXPECT_TRUE(moved.is_write_end());
}

// Test copy constructor
TEST(PipeTest, CopyConstructorDuplicatesFd) {
    auto [reader, writer] = Pipe::create();
    Pipe copy(reader);

    EXPECT_TRUE(copy.ok());
    EXPECT_NE(copy.fd(), reader.fd());
    EXPECT_TRUE(copy.is_read_end());
}

// Test copy assignment
TEST(PipeTest, CopyAssignmentDuplicatesFd) {
    auto [reader, writer] = Pipe::create();
    Pipe another;
    another = reader;

    EXPECT_TRUE(another.ok());
    EXPECT_NE(another.fd(), reader.fd());
    EXPECT_TRUE(another.is_read_end());
}

// Test unidirectional communication enforcement (read from write-end)
TEST(PipeTest, WriteEndCannotRead) {
    auto [reader, writer] = Pipe::create();
    std::vector<uint8_t> buffer(10);
    ssize_t result = writer.read(buffer.data(), buffer.size());
    EXPECT_EQ(result, -1);  // Should fail or return 0
}

// Test unidirectional communication enforcement (write to read-end)
TEST(PipeTest, ReadEndCannotWrite) {
    auto [reader, writer] = Pipe::create();
    const std::string msg = "bad write";
    ssize_t result = reader.write(reinterpret_cast<const uint8_t*>(msg.data()), msg.size());
    EXPECT_EQ(result, -1);  // Should fail
}
