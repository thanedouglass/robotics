#include <gtest/gtest.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include <vector>

#include "rix/ipc/file.hpp"

using namespace rix::ipc;

class FileTest : public ::testing::Test {
protected:
    std::string temp_filename = "test_file.tmp";

    void SetUp() override {
        std::ofstream(temp_filename) << "Hello, File!";
    }

    void TearDown() override {
        unlink(temp_filename.c_str());
    }
};

// Test default constructor
TEST_F(FileTest, DefaultConstructor) {
    File f;
    EXPECT_FALSE(f.ok());
}

// Test constructor with file path
TEST_F(FileTest, OpenFileConstructor) {
    File f(temp_filename, O_RDONLY);
    EXPECT_TRUE(f.ok());
}

// Test remove
TEST_F(FileTest, RemoveFile) {
    ASSERT_TRUE(File::remove(temp_filename));
    ASSERT_FALSE(access(temp_filename.c_str(), F_OK) == 0);
}

// Test fd-based constructor
TEST_F(FileTest, FdConstructor) {
    int fd = open(temp_filename.c_str(), O_RDONLY);
    ASSERT_GE(fd, 0);
    File f(fd);
    EXPECT_TRUE(f.ok());
    EXPECT_EQ(f.fd(), fd);
    // Don't close fd manually; File will manage it
}

// Test copy constructor
TEST_F(FileTest, CopyConstructor) {
    File original(temp_filename, O_RDONLY);
    File copy(original);
    EXPECT_TRUE(copy.ok());
    EXPECT_NE(copy.fd(), original.fd());  // should be a different fd
}

// Test move constructor
TEST_F(FileTest, MoveConstructor) {
    File original(temp_filename, O_RDONLY);
    int original_fd = original.fd();
    File moved(std::move(original));
    EXPECT_TRUE(moved.ok());
    EXPECT_FALSE(original.ok());
    EXPECT_EQ(moved.fd(), original_fd);
}

// Test copy assignment
TEST_F(FileTest, CopyAssignment) {
    File a(temp_filename, O_RDONLY);
    File b;
    b = a;
    EXPECT_TRUE(b.ok());
    EXPECT_NE(a.fd(), b.fd());
}

// Test move assignment
TEST_F(FileTest, MoveAssignment) {
    File a(temp_filename, O_RDONLY);
    int a_fd = a.fd();
    File b;
    b = std::move(a);
    EXPECT_TRUE(b.ok());
    EXPECT_FALSE(a.ok());
    EXPECT_EQ(b.fd(), a_fd);
}

// Test read
TEST_F(FileTest, ReadFile) {
    File f(temp_filename, O_RDONLY);
    std::vector<uint8_t> buffer(5);
    ssize_t bytes = f.read(buffer.data(), buffer.size());
    ASSERT_EQ(bytes, 5);
    EXPECT_EQ(std::string(buffer.begin(), buffer.end()), "Hello");
}

// Test write
TEST_F(FileTest, WriteFile) {
    std::string writable_file = "write_test.tmp";
    {
        File f(writable_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        std::string data = "WriteTest";
        ssize_t bytes = f.write(reinterpret_cast<const uint8_t*>(data.data()), data.size());
        EXPECT_EQ(bytes, data.size());
    }

    std::ifstream in(writable_file);
    std::string result;
    in >> result;
    EXPECT_EQ(result, "WriteTest");

    unlink(writable_file.c_str());
}

// Test non-blocking mode toggling
TEST_F(FileTest, NonBlockingToggle) {
    File f(temp_filename, O_RDONLY);
    f.set_nonblocking(true);
    EXPECT_TRUE(f.is_nonblocking());

    f.set_nonblocking(false);
    EXPECT_FALSE(f.is_nonblocking());
}

// Test wait for readable — likely always true for small file
TEST_F(FileTest, WaitForReadable) {
    File f(temp_filename, O_RDONLY);
    rix::util::Duration timeout(0.1);  // 100 ms
    EXPECT_TRUE(f.wait_for_readable(timeout));
}

// Test wait for writable
TEST_F(FileTest, WaitForWritable) {
    std::string writable_file = "write_wait.tmp";
    File f(writable_file, O_WRONLY | O_CREAT, 0644);
    rix::util::Duration timeout(0.1);  // 100 ms
    EXPECT_TRUE(f.wait_for_writable(timeout));
    unlink(writable_file.c_str());
}
