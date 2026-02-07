
/*
x size_number
x size_string
x size_message
x size_number_array
x size_string_array
x size_message_array
x size_number_vector
x size_string_vector
x size_message_vector

x serialize_number
x serialize_string
x serialize_message
x serialize_number_array
x serialize_string_array
x serialize_message_array
x serialize_number_vector
x serialize_string_vector
x serialize_message_vector

deserialize_number
deserialize_string
deserialize_message
deserialize_number_array
deserialize_string_array
deserialize_message_array
deserialize_number_vector
deserialize_string_vector
deserialize_message_vector
*/

#include "rix/msg/serialization.hpp"

#include <gtest/gtest.h>

#include "rix/msg/message.hpp"

using namespace rix::msg::detail;

class TestMessage : public rix::msg::Message {
public:
    uint32_t value = 0;

    size_t size() const override {
        return 4;
    }
    void serialize(uint8_t *dst, size_t &offset) const override {
        serialize_number(dst, offset, value);
    }
    bool deserialize(const uint8_t* src, size_t size, size_t& offset) override {
        return deserialize_number(value, src, size, offset);
    }
    std::array<uint64_t, 2> hash() const override { return {123, 456}; }

    bool operator==(const TestMessage& other) const {
        return value == other.value;
    }
};

TEST(Size, NumberTest) {
    EXPECT_EQ(size_number<bool>(0), 1) << "size_number for bool incorrect.";
    EXPECT_EQ(size_number<char>(0), 1) << "size_number for char incorrect.";
    EXPECT_EQ(size_number<int8_t>(0), 1) << "size_number for int8_t incorrect.";
    EXPECT_EQ(size_number<uint8_t>(0), 1) << "size_number for uint8_t incorrect.";
    EXPECT_EQ(size_number<int16_t>(0), 2) << "size_number for int16_t incorrect.";
    EXPECT_EQ(size_number<uint16_t>(0), 2) << "size_number for uint16_t incorrect.";
    EXPECT_EQ(size_number<int32_t>(0), 4) << "size_number for int32_t incorrect.";
    EXPECT_EQ(size_number<uint32_t>(0), 4) << "size_number for uint32_t incorrect.";
    EXPECT_EQ(size_number<int64_t>(0), 8) << "size_number for int64_t incorrect.";
    EXPECT_EQ(size_number<uint64_t>(0), 8) << "size_number for uint64_t incorrect.";
    EXPECT_EQ(size_number<float>(0), 4) << "size_number for float incorrect.";
    EXPECT_EQ(size_number<double>(0), 8) << "size_number for double incorrect.";
}

TEST(Size, StringTest) {
    EXPECT_EQ(size_string("Hello, world!"), 17) << "size_string incorrect.";
    EXPECT_EQ(size_string("Goodbye!"), 12) << "size_string incorrect.";
    EXPECT_EQ(size_string("abcdefghijklmnopqrstuvwxyz"), 30) << "size_string incorrect.";
}

TEST(Size, MessageTest) {
    TestMessage msg;
    EXPECT_EQ(size_message(msg), 4) << "size_message incorrect.";
}

TEST(Size, NumberArrayTest) {
    std::array<uint16_t, 12> arr1;
    EXPECT_EQ(size_number_array(arr1), 24) << "size_number_array incorrect";
    std::array<int64_t, 5> arr2;
    EXPECT_EQ(size_number_array(arr2), 40) << "size_number_array incorrect";
}

TEST(Size, StringArrayTest) {
    std::array<std::string, 3> arr1;
    arr1[0] = "Hello!";
    arr1[1] = "ROB320";
    arr1[2] = "robots :)";
    EXPECT_EQ(size_string_array(arr1), 33) << "size_string_array incorrect";
    std::array<std::string, 1> arr2;
    arr2[0] = "mbot<3";
    EXPECT_EQ(size_string_array(arr2), 10) << "size_string_array incorrect";
}

TEST(Size, MessageArrayTest) {
    std::array<TestMessage, 3> msg;
    EXPECT_EQ(size_message_array(msg), 12) << "size_message_array incorrect.";
}

TEST(Size, NumberVectorTest) {
    std::vector<uint16_t> vec1;
    vec1.resize(12);
    EXPECT_EQ(size_number_vector(vec1), 28) << "size_number_vector incorrect.";
    std::vector<int64_t> vec2;
    vec2.resize(5);
    EXPECT_EQ(size_number_vector(vec2), 44) << "size_number_vector incorrect.";
}

TEST(Size, StringVectorTest) {
    std::vector<std::string> vec1;
    vec1.resize(3);
    vec1[0] = "Hello!";
    vec1[1] = "ROB320";
    vec1[2] = "robots :)";
    EXPECT_EQ(size_string_vector(vec1), 37) << "size_string_vector incorrect";
    std::vector<std::string> vec2;
    vec2.resize(1);
    vec2[0] = "mbot<3";
    EXPECT_EQ(size_string_vector(vec2), 14) << "size_string_vector incorrect";
}

TEST(Size, MessageVectorTest) {
    std::vector<TestMessage> vec1;
    vec1.resize(3);
    EXPECT_EQ(size_message_vector(vec1), 16) << "size_message_vector incorrect";
}

TEST(Serialize, NumberTest) {
    std::vector<uint8_t> buffer;
    const size_t expected_size = 44;
    buffer.resize(256);

    int64_t i64 = 4096;
    uint64_t ui64 = 81;
    int32_t i32 = 1024;
    uint32_t ui32 = 512;
    int16_t i16 = 512;
    uint16_t ui16 = 256;
    float f32 = 256;
    double f64 = 2048;
    char c = 'r';
    bool b = true;
    int8_t i8 = 16;
    uint8_t ui8 = 32;

    size_t offset = 0;
    serialize_number(buffer.data(), offset, i64);
    ASSERT_EQ(offset, 8) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, ui64);
    ASSERT_EQ(offset, 16) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, i32);
    ASSERT_EQ(offset, 20) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, ui32);
    ASSERT_EQ(offset, 24) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, i16);
    ASSERT_EQ(offset, 26) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, ui16);
    ASSERT_EQ(offset, 28) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, f32);
    ASSERT_EQ(offset, 32) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, f64);
    ASSERT_EQ(offset, 40) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, c);
    ASSERT_EQ(offset, 41) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, b);
    ASSERT_EQ(offset, 42) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, i8);
    ASSERT_EQ(offset, 43) << "serialize_number offset incorrect.";
    serialize_number(buffer.data(), offset, ui8);
    ASSERT_EQ(offset, 44) << "serialize_number offset incorrect.";

    uint8_t *ptr = buffer.data();
    EXPECT_EQ(*(int64_t *)(ptr), i64) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(uint64_t *)(ptr + 8), ui64) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(int32_t *)(ptr + 16), i32) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(uint32_t *)(ptr + 20), ui32) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(int16_t *)(ptr + 24), i16) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(uint16_t *)(ptr + 26), ui16) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(float *)(ptr + 28), f32) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(double *)(ptr + 32), f64) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(char *)(ptr + 40), c) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(bool *)(ptr + 41), b) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(int8_t *)(ptr + 42), i8) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(*(uint8_t *)(ptr + 43), ui8) << "serialize_number encoded value incorrect.";

    int64_t i64_copy = 0;
    uint64_t ui64_copy = 0;
    int32_t i32_copy = 0;
    uint32_t ui32_copy = 0;
    int16_t i16_copy = 0;
    uint16_t ui16_copy = 0;
    float f32_copy = 0;
    double f64_copy = 0;
    char c_copy = 0;
    bool b_copy = 0;
    int8_t i8_copy = 0;
    uint8_t ui8_copy = 0;

    offset = 0;
    deserialize_number(i64_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 8) << "deserialize_number offset incorrect.";
    deserialize_number(ui64_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 16) << "deserialize_number offset incorrect.";
    deserialize_number(i32_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 20) << "deserialize_number offset incorrect.";
    deserialize_number(ui32_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 24) << "deserialize_number offset incorrect.";
    deserialize_number(i16_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 26) << "deserialize_number offset incorrect.";
    deserialize_number(ui16_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 28) << "deserialize_number offset incorrect.";
    deserialize_number(f32_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 32) << "deserialize_number offset incorrect.";
    deserialize_number(f64_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 40) << "deserialize_number offset incorrect.";
    deserialize_number(c_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 41) << "deserialize_number offset incorrect.";
    deserialize_number(b_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 42) << "deserialize_number offset incorrect.";
    deserialize_number(i8_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 43) << "deserialize_number offset incorrect.";
    deserialize_number(ui8_copy, buffer.data(), buffer.size(), offset);
    ASSERT_EQ(offset, 44) << "deserialize_number offset incorrect.";

    EXPECT_EQ(i64_copy, i64) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(ui64_copy, ui64) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(i32_copy, i32) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(ui32_copy, ui32) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(i16_copy, i16) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(ui16_copy, ui16) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(f32_copy, f32) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(f64_copy, f64) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(c_copy, c) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(b_copy, b) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(i8_copy, i8) << "serialize_number encoded value incorrect.";
    EXPECT_EQ(ui8_copy, ui8) << "serialize_number encoded value incorrect.";
}

TEST(Serialize, StringTest) {
    std::vector<uint8_t> buffer;
    const size_t expected_size = 67;
    buffer.resize(256);

    std::string str1("Hello, world!");
    std::string str2("ROB320 is great");
    std::string str3("Robot Interprocess eXchange");

    size_t offset = 0;
    serialize_string(buffer.data(), offset, str1);
    ASSERT_EQ(offset, 17) << "serialize_string offset incorrect.";
    EXPECT_EQ(*(uint32_t *)(buffer.data()), 13) << "serialize_string encoded size incorrect.";
    EXPECT_TRUE(strcmp((char *)(buffer.data() + 4), str1.data()) == 0) << "serialize_string encoded data incorrect.";
    serialize_string(buffer.data(), offset, str2);
    ASSERT_EQ(offset, 36) << "serialize_string offset incorrect.";
    EXPECT_EQ(*(uint32_t *)(buffer.data() + 17), 15) << "serialize_string encoded size incorrect.";
    EXPECT_TRUE(strcmp((char *)(buffer.data() + 21), str2.data()) == 0) << "serialize_string encoded data incorrect.";
    serialize_string(buffer.data(), offset, str3);
    ASSERT_EQ(offset, 67) << "serialize_string offset incorrect.";
    EXPECT_EQ(*(uint32_t *)(buffer.data() + 36), 27) << "serialize_string encoded size incorrect.";
    EXPECT_TRUE(strcmp((char *)(buffer.data() + 40), str3.data()) == 0) << "serialize_string encoded data incorrect.";
}

TEST(Serialize, MessageTest) {
    std::vector<uint8_t> buffer;
    buffer.resize(256);

    TestMessage msg;
    msg.value = 0x12;
    size_t offset = 0;
    serialize_message(buffer.data(), offset, msg);
    EXPECT_EQ(offset, 4) << "serialize_message offset incorrect.";
    EXPECT_EQ(buffer[0], 0x12);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(buffer[2], 0x00);
    EXPECT_EQ(buffer[3], 0x00);
}

TEST(Serialize, NumberArrayTest) {
    std::vector<uint8_t> buffer;
    buffer.resize(256);

    std::array<double, 4> arr1;
    arr1[0] = 1.2;
    arr1[1] = 2.3;
    arr1[2] = 3.4;
    arr1[3] = 4.5;

    std::array<char, 2> arr2;
    arr2[0] = 'a';
    arr2[1] = 'b';

    std::array<int16_t, 4> arr3;
    arr3[0] = 2;
    arr3[1] = 4;
    arr3[2] = 8;
    arr3[3] = 16;

    size_t offset = 0;
    serialize_number_array(buffer.data(), offset, arr1);
    ASSERT_EQ(offset, 32) << "serialize_number_array offset incorrect.";
    serialize_number_array(buffer.data(), offset, arr2);
    ASSERT_EQ(offset, 34) << "serialize_number_array offset incorrect.";
    serialize_number_array(buffer.data(), offset, arr3);
    ASSERT_EQ(offset, 42) << "serialize_number_array offset incorrect.";

    EXPECT_EQ(*(double *)(buffer.data()), arr1[0]);
    EXPECT_EQ(*(double *)(buffer.data() + 8), arr1[1]);
    EXPECT_EQ(*(double *)(buffer.data() + 16), arr1[2]);
    EXPECT_EQ(*(double *)(buffer.data() + 24), arr1[3]);
    EXPECT_EQ(*(char *)(buffer.data() + 32), arr2[0]);
    EXPECT_EQ(*(char *)(buffer.data() + 33), arr2[1]);
    EXPECT_EQ(*(int16_t *)(buffer.data() + 34), arr3[0]);
    EXPECT_EQ(*(int16_t *)(buffer.data() + 36), arr3[1]);
    EXPECT_EQ(*(int16_t *)(buffer.data() + 38), arr3[2]);
    EXPECT_EQ(*(int16_t *)(buffer.data() + 40), arr3[3]);
}

TEST(Serialize, StringArrayTest) {
    std::vector<uint8_t> buffer;
    buffer.resize(256);

    std::array<std::string, 3> arr;
    arr[0] = "Hello, world!";
    arr[1] = "ROB320 is great";
    arr[2] = "Robot Interprocess eXchange";

    size_t offset = 0;
    serialize_string_array(buffer.data(), offset, arr);
    ASSERT_EQ(offset, 67) << "serialize_string_array offset incorrect.";

    uint32_t size = *(uint32_t *)(buffer.data());
    EXPECT_EQ(size, 13) << "serialize_string_array encoded size incorrect.";
    char *data = (char *)(buffer.data() + 4);
    EXPECT_TRUE(strncmp(data, arr[0].data(), arr[0].size()) == 0) << "serialize_string_array encoded data incorrect.";

    size = *(uint32_t *)(buffer.data() + 17);
    EXPECT_EQ(size, 15) << "serialize_string_array encoded size incorrect.";
    data = (char *)(buffer.data() + 21);
    EXPECT_TRUE(strncmp(data, arr[1].data(), arr[1].size()) == 0) << "serialize_string_array encoded data incorrect.";

    size = *(uint32_t *)(buffer.data() + 36);
    EXPECT_EQ(size, 27) << "serialize_string_array encoded size incorrect.";
    data = (char *)(buffer.data() + 40);
    EXPECT_TRUE(strncmp(data, arr[2].data(), arr[2].size()) == 0) << "serialize_string_array encoded data incorrect.";
}

TEST(Serialize, MessageArrayTest) {
    std::vector<uint8_t> buffer;
    buffer.resize(256);

    std::array<TestMessage, 3> arr;
    arr[0].value = 1;
    arr[1].value = 2;
    arr[2].value = 3;
    size_t offset = 0;
    serialize_message_array(buffer.data(), offset, arr);
    EXPECT_EQ(offset, 12) << "serialize_message offset incorrect.";
    EXPECT_EQ(buffer[0], 0x01);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(buffer[2], 0x00);
    EXPECT_EQ(buffer[3], 0x00);
    EXPECT_EQ(buffer[4], 0x02);
    EXPECT_EQ(buffer[5], 0x00);
    EXPECT_EQ(buffer[6], 0x00);
    EXPECT_EQ(buffer[7], 0x00);
    EXPECT_EQ(buffer[8], 0x03);
    EXPECT_EQ(buffer[9], 0x00);
    EXPECT_EQ(buffer[10], 0x00);
    EXPECT_EQ(buffer[11], 0x00);
}

TEST(Serialize, NumberVectorTest) {
    std::vector<uint8_t> buffer;
    buffer.resize(256);

    std::vector<double> vec1;
    vec1.resize(4);
    vec1[0] = 1.2;
    vec1[1] = 2.3;
    vec1[2] = 3.4;
    vec1[3] = 4.5;

    std::vector<char> vec2;
    vec2.resize(2);
    vec2[0] = 'a';
    vec2[1] = 'b';

    std::vector<int16_t> vec3;
    vec3.resize(4);
    vec3[0] = 2;
    vec3[1] = 4;
    vec3[2] = 8;
    vec3[3] = 16;

    size_t offset = 0;
    serialize_number_vector(buffer.data(), offset, vec1);
    ASSERT_EQ(offset, 36) << "serialize_number_vector offset incorrect.";
    serialize_number_vector(buffer.data(), offset, vec2);
    ASSERT_EQ(offset, 42) << "serialize_number_vector offset incorrect.";
    serialize_number_vector(buffer.data(), offset, vec3);
    ASSERT_EQ(offset, 54) << "serialize_number_vector offset incorrect.";

    EXPECT_EQ(*(uint32_t *)(buffer.data()), 4);
    EXPECT_EQ(*(double *)(buffer.data() + 4), vec1[0]);
    EXPECT_EQ(*(double *)(buffer.data() + 12), vec1[1]);
    EXPECT_EQ(*(double *)(buffer.data() + 20), vec1[2]);
    EXPECT_EQ(*(double *)(buffer.data() + 28), vec1[3]);

    EXPECT_EQ(*(uint32_t *)(buffer.data() + 36), 2);
    EXPECT_EQ(*(char *)(buffer.data() + 40), vec2[0]);
    EXPECT_EQ(*(char *)(buffer.data() + 41), vec2[1]);

    EXPECT_EQ(*(uint32_t *)(buffer.data() + 42), 4);
    EXPECT_EQ(*(int16_t *)(buffer.data() + 46), vec3[0]);
    EXPECT_EQ(*(int16_t *)(buffer.data() + 48), vec3[1]);
    EXPECT_EQ(*(int16_t *)(buffer.data() + 50), vec3[2]);
    EXPECT_EQ(*(int16_t *)(buffer.data() + 52), vec3[3]);
}

TEST(Serialize, StringVectorTest) {
    std::vector<uint8_t> buffer;
    buffer.resize(256);

    std::vector<std::string> vec;
    vec.resize(3);
    vec[0] = "Hello, world!";
    vec[1] = "ROB320 is great";
    vec[2] = "Robot Interprocess eXchange";

    size_t offset = 0;
    serialize_string_vector(buffer.data(), offset, vec);
    ASSERT_EQ(offset, 71) << "serialize_string_vector offset incorrect.";
    uint32_t size = *(uint32_t *)(buffer.data());
    EXPECT_EQ(size, 3) << "serialize_string_vector encoded size incorrect.";

    size = *(uint32_t *)(buffer.data() + 4);
    EXPECT_EQ(size, 13) << "serialize_string_vector encoded size incorrect.";
    char *data = (char *)(buffer.data() + 8);
    EXPECT_TRUE(strncmp(data, vec[0].data(), vec[0].size()) == 0) << "serialize_string_vector encoded data incorrect.";

    size = *(uint32_t *)(buffer.data() + 21);
    EXPECT_EQ(size, 15) << "serialize_string_vector encoded size incorrect.";
    data = (char *)(buffer.data() + 25);
    EXPECT_TRUE(strncmp(data, vec[1].data(), vec[1].size()) == 0) << "serialize_string_vector encoded data incorrect.";

    size = *(uint32_t *)(buffer.data() + 40);
    EXPECT_EQ(size, 27) << "serialize_string_vector encoded size incorrect.";
    data = (char *)(buffer.data() + 44);
    EXPECT_TRUE(strncmp(data, vec[2].data(), vec[2].size()) == 0) << "serialize_string_vector encoded data incorrect.";
}

TEST(Serialize, MessageVectorTest) {
    std::vector<uint8_t> buffer;
    buffer.resize(256);

    std::vector<TestMessage> vec;
    vec.resize(3);
    vec[0].value = 1;
    vec[1].value = 2;
    vec[2].value = 3;
    size_t offset = 0;
    serialize_message_vector(buffer.data(), offset, vec);
    EXPECT_EQ(offset, 16) << "serialize_message offset incorrect.";
    EXPECT_EQ(*(uint32_t*)buffer.data(), 3) << "serialize_message offset incorrect.";
    EXPECT_EQ(buffer[4], 0x01);
    EXPECT_EQ(buffer[5], 0x00);
    EXPECT_EQ(buffer[6], 0x00);
    EXPECT_EQ(buffer[7], 0x00);
    EXPECT_EQ(buffer[8], 0x02);
    EXPECT_EQ(buffer[9], 0x00);
    EXPECT_EQ(buffer[10], 0x00);
    EXPECT_EQ(buffer[11], 0x00);
    EXPECT_EQ(buffer[12], 0x03);
    EXPECT_EQ(buffer[13], 0x00);
    EXPECT_EQ(buffer[14], 0x00);
    EXPECT_EQ(buffer[15], 0x00);
}

TEST(Deserialize, Number_Success) {
    uint32_t input = 0x12345678;
    uint8_t bytes[sizeof(uint32_t)];
    std::memcpy(bytes, &input, sizeof(input));

    uint32_t result;
    size_t offset = 0;
    EXPECT_TRUE(deserialize_number(result, bytes, sizeof(bytes), offset));
    EXPECT_EQ(result, input);
    EXPECT_EQ(offset, sizeof(uint32_t));
}

TEST(Deserialize, Number_Fail_TooShort) {
    uint8_t bytes[2] = {0x12, 0x34};
    uint32_t result;
    size_t offset = 0;
    EXPECT_FALSE(deserialize_number(result, bytes, sizeof(bytes), offset));
}

TEST(Deserialize, String_Success) {
    std::string input = "hello";
    uint32_t len = input.size();
    std::vector<uint8_t> bytes(sizeof(len) + input.size());
    std::memcpy(bytes.data(), &len, sizeof(len));
    std::memcpy(bytes.data() + sizeof(len), input.data(), input.size());

    std::string result;
    size_t offset = 0;
    EXPECT_TRUE(deserialize_string(result, bytes.data(), bytes.size(), offset));
    EXPECT_EQ(result, input);
}

TEST(Deserialize, String_Fail_LengthTooLong) {
    uint32_t len = 100;
    uint8_t bytes[4];
    std::memcpy(bytes, &len, sizeof(len));

    std::string result;
    size_t offset = 0;
    EXPECT_FALSE(deserialize_string(result, bytes, sizeof(bytes), offset));
}

TEST(Deserialize, NumberArray_Success) {
    std::array<uint16_t, 3> input = {1, 2, 3};
    uint8_t bytes[sizeof(input)];
    std::memcpy(bytes, input.data(), sizeof(input));

    std::array<uint16_t, 3> result;
    size_t offset = 0;
    EXPECT_TRUE(deserialize_number_array(result, bytes, sizeof(bytes), offset));
    EXPECT_EQ(result, input);
}

TEST(Deserialize, NumberArray_Fail_TooShort) {
    std::array<uint16_t, 2> result;
    uint8_t bytes[1] = {0};
    size_t offset = 0;
    EXPECT_FALSE(deserialize_number_array(result, bytes, sizeof(bytes), offset));
}

TEST(Deserialize, StringArray_Success) {
    std::array<std::string, 2> input = {"abc", "xyz"};
    std::vector<uint8_t> bytes;

    for (const auto& s : input) {
        uint32_t len = s.size();
        bytes.insert(bytes.end(), reinterpret_cast<uint8_t*>(&len), reinterpret_cast<uint8_t*>(&len) + sizeof(len));
        bytes.insert(bytes.end(), s.begin(), s.end());
    }

    std::array<std::string, 2> result;
    size_t offset = 0;
    EXPECT_TRUE(deserialize_string_array(result, bytes.data(), bytes.size(), offset));
    EXPECT_EQ(result, input);
}

TEST(Deserialize, StringArray_Fail_NotEnoughData) {
    std::array<std::string, 2> result;
    uint8_t bytes[4] = {3, 0, 0, 0}; // Claims a string of 3 bytes, but none follow
    size_t offset = 0;
    EXPECT_FALSE(deserialize_string_array(result, bytes, sizeof(bytes), offset));
}

TEST(DeserializeTest, MessageArray_Success) {
    std::array<TestMessage, 2> input;
    input[0].value = 10;
    input[1].value = 20;
    std::vector<uint8_t> bytes;
    for (const auto& m : input) {
        bytes.insert(bytes.end(),
                     reinterpret_cast<const uint8_t*>(&m.value),
                     reinterpret_cast<const uint8_t*>(&m.value) + sizeof(m.value));
    }

    std::array<TestMessage, 2> result;
    size_t offset = 0;
    EXPECT_TRUE(deserialize_message_array(result, bytes.data(), bytes.size(), offset));
    EXPECT_EQ(result, input);
}

TEST(Deserialize, NumberVector_Success) {
    std::vector<uint32_t> input = {1, 2, 3};
    uint32_t count = input.size();
    std::vector<uint8_t> bytes;
    bytes.insert(bytes.end(), reinterpret_cast<uint8_t*>(&count), reinterpret_cast<uint8_t*>(&count) + sizeof(count));
    bytes.insert(bytes.end(), reinterpret_cast<uint8_t*>(input.data()), reinterpret_cast<uint8_t*>(input.data()) + sizeof(uint32_t) * input.size());

    std::vector<uint32_t> result;
    size_t offset = 0;
    EXPECT_TRUE(deserialize_number_vector(result, bytes.data(), bytes.size(), offset));
    EXPECT_EQ(result, input);
}

TEST(Deserialize, NumberVector_Fail_TooShort) {
    uint8_t bytes[4] = {2, 0, 0, 0}; // Claims 2 elements, but gives none
    std::vector<uint32_t> result;
    size_t offset = 0;
    EXPECT_FALSE(deserialize_number_vector(result, bytes, sizeof(bytes), offset));
}

TEST(Deserialize, StringVector_Success) {
    std::vector<std::string> input = {"one", "two"};
    std::vector<uint8_t> bytes;
    uint32_t count = input.size();
    bytes.insert(bytes.end(), reinterpret_cast<uint8_t*>(&count), reinterpret_cast<uint8_t*>(&count) + sizeof(count));

    for (const auto& s : input) {
        uint32_t len = s.size();
        bytes.insert(bytes.end(), reinterpret_cast<uint8_t*>(&len), reinterpret_cast<uint8_t*>(&len) + sizeof(len));
        bytes.insert(bytes.end(), s.begin(), s.end());
    }

    std::vector<std::string> result;
    size_t offset = 0;
    EXPECT_TRUE(deserialize_string_vector(result, bytes.data(), bytes.size(), offset));
    EXPECT_EQ(result, input);
}

TEST(DeserializeTest, MessageVector_Success) {
    std::vector<TestMessage> input(2);
    input[0].value = 100;
    input[1].value = 200;
    std::vector<uint8_t> bytes;
    uint32_t count = input.size();
    bytes.insert(bytes.end(), reinterpret_cast<uint8_t*>(&count), reinterpret_cast<uint8_t*>(&count) + sizeof(count));
    for (const auto& m : input) {
        bytes.insert(bytes.end(),
                     reinterpret_cast<const uint8_t*>(&m.value),
                     reinterpret_cast<const uint8_t*>(&m.value) + sizeof(m.value));
    }

    std::vector<TestMessage> result;
    size_t offset = 0;
    EXPECT_TRUE(deserialize_message_vector(result, bytes.data(), bytes.size(), offset));
    EXPECT_EQ(result, input);
}
