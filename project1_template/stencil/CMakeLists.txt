cmake_minimum_required(VERSION 3.11 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 20)

project(teleop-keyboard)

list(APPEND CMAKE_PREFIX_PATH "~/.local")

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(mbot src/mbot/mbot.cpp)
target_link_libraries(mbot m Threads::Threads)
target_include_directories(mbot PRIVATE include/)

add_library(project1 src/rix/ipc/fifo.cpp
    src/rix/ipc/file.cpp
    src/rix/ipc/pipe.cpp
    src/rix/ipc/signal.cpp
    src/rix/util/time.cpp
    src/rix/util/argument_parser.cpp
)
target_include_directories(project1 PRIVATE include/)

add_executable(teleop_keyboard src/teleop_keyboard/teleop_keyboard.cpp src/teleop_keyboard/main.cpp)
target_link_libraries(teleop_keyboard mbot project1)
target_include_directories(teleop_keyboard PRIVATE include/)

add_executable(mbot_driver src/mbot_driver/mbot_driver.cpp src/mbot_driver/main.cpp)
target_link_libraries(mbot_driver mbot project1)
target_include_directories(mbot_driver PRIVATE include/)

# Unit Testing
enable_testing()

add_executable(messages_test tests/messages.cpp)
target_link_libraries(messages_test GTest::gtest_main)
target_include_directories(messages_test PRIVATE include/)

add_executable(serialization_test tests/serialization.cpp)
target_link_libraries(serialization_test GTest::gtest_main)
target_include_directories(serialization_test PRIVATE include/)

add_executable(signal_test tests/signal.cpp)
target_link_libraries(signal_test project1 GTest::gtest_main)
target_include_directories(signal_test PRIVATE include/)

add_executable(file_test tests/file.cpp)
target_link_libraries(file_test project1 GTest::gtest_main)
target_include_directories(file_test PRIVATE include/)

add_executable(fifo_test tests/fifo.cpp)
target_link_libraries(fifo_test project1 GTest::gtest_main)
target_include_directories(fifo_test PRIVATE include/)

add_executable(pipe_test tests/pipe.cpp)
target_link_libraries(pipe_test project1 GTest::gtest_main)
target_include_directories(pipe_test PRIVATE include/)
