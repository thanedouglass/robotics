#include "rix/msg/standard/Time.hpp"
#include "rix/msg/standard/Header.hpp"
#include "rix/msg/standard/UInt32.hpp"
#include "rix/msg/geometry/Twist2D.hpp"
#include "rix/msg/geometry/Twist2DStamped.hpp"

#include <gtest/gtest.h>

using namespace rix::msg::standard;
using namespace rix::msg::geometry;

TEST(Messages, StandardUInt32Test) {
    UInt32 u1;
    u1.data = 1234;

    std::vector<uint8_t> buffer;
    const size_t size = u1.size();
    ASSERT_EQ(size, 4) << "UInt32::size is incorrect.";

    buffer.resize(size);

    size_t offset = 0;
    u1.serialize(buffer.data(), offset);
    ASSERT_EQ(offset, size) << "UInt32::serialize offset is incorrect.";

    UInt32 u2;
    offset = 0;
    u2.deserialize(buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, size) << "UInt32::deserialize offset is incorrect.";

    EXPECT_EQ(u2.data, u1.data);
}

TEST(Messages, StandardTimeTest) {
    Time t1;
    t1.sec = 1234;
    t1.nsec = 5678;

    std::vector<uint8_t> buffer;
    const size_t size = t1.size();
    ASSERT_EQ(size, 8) << "Time::size is incorrect.";

    buffer.resize(size);

    size_t offset = 0;
    t1.serialize(buffer.data(), offset);
    ASSERT_EQ(offset, size) << "Time::serialize offset is incorrect.";

    Time t2;
    offset = 0;
    t2.deserialize(buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, size) << "Time::deserialize offset is incorrect.";

    EXPECT_EQ(t2.sec, t1.sec);
    EXPECT_EQ(t2.nsec, t1.nsec);
}

TEST(Messages, StandardHeaderTest) {
    Header h1;
    h1.frame_id = "Hello, world!";
    h1.seq = 123;
    h1.stamp.sec = 456;
    h1.stamp.nsec = 789;

    std::vector<uint8_t> buffer;
    const size_t size = h1.size();
    ASSERT_EQ(size, 29) << "Header::size is incorrect.";

    buffer.resize(size);

    size_t offset = 0;
    h1.serialize(buffer.data(), offset);
    ASSERT_EQ(offset, size) << "Header::serialize offset is incorrect.";

    Header h2;
    offset = 0;
    h2.deserialize(buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, size) << "Header::deserialize offset is incorrect.";

    EXPECT_EQ(h2.frame_id, h1.frame_id);
    EXPECT_EQ(h2.seq, h1.seq);
    EXPECT_EQ(h2.stamp.sec, h1.stamp.sec);
    EXPECT_EQ(h2.stamp.nsec, h1.stamp.nsec);
}

TEST(Messages, GeometryTwist2DTest) {
    Twist2D tw1;
    tw1.vx = 1.23;
    tw1.vy = 4.56;
    tw1.wz = 7.89;

    std::vector<uint8_t> buffer;
    const size_t size = tw1.size();
    ASSERT_EQ(size, 12) << "Twist2D::size is incorrect.";

    buffer.resize(size);

    size_t offset = 0;
    tw1.serialize(buffer.data(), offset);
    ASSERT_EQ(offset, size) << "Twist2D::serialize offset is incorrect.";

    Twist2D tw2;
    offset = 0;
    tw2.deserialize(buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, size) << "Twist2D::deserialize offset is incorrect.";

    EXPECT_NEAR(tw2.vx, tw1.vx, 1e-6);
    EXPECT_NEAR(tw2.vy, tw1.vy, 1e-6);
    EXPECT_NEAR(tw2.wz, tw1.wz, 1e-6);
}

TEST(Messages, GeometryTwist2DStampedTest) {
    Twist2DStamped tws1;
    tws1.header.frame_id = "Hello, world!";
    tws1.header.seq = 123;
    tws1.header.stamp.sec = 456;
    tws1.header.stamp.nsec = 789;
    tws1.twist.vx = 1.23;
    tws1.twist.vy = 4.56;
    tws1.twist.wz = 7.89;

    std::vector<uint8_t> buffer;
    const size_t size = tws1.size();
    ASSERT_EQ(size, 41) << "Twist2DStamped::size is incorrect.";

    buffer.resize(size);

    size_t offset = 0;
    tws1.serialize(buffer.data(), offset);
    ASSERT_EQ(offset, size) << "Twist2DStamped::serialize offset is incorrect.";

    Twist2DStamped tws2;
    offset = 0;
    tws2.deserialize(buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, size) << "Twist2DStamped::deserialize offset is incorrect.";

    EXPECT_EQ(tws2.header.frame_id, tws1.header.frame_id);
    EXPECT_EQ(tws2.header.seq, tws1.header.seq);
    EXPECT_EQ(tws2.header.stamp.sec, tws1.header.stamp.sec);
    EXPECT_EQ(tws2.header.stamp.nsec, tws1.header.stamp.nsec);
    EXPECT_NEAR(tws2.twist.vx, tws1.twist.vx, 1e-6);
    EXPECT_NEAR(tws2.twist.vy, tws1.twist.vy, 1e-6);
    EXPECT_NEAR(tws2.twist.wz, tws1.twist.wz, 1e-6);
}