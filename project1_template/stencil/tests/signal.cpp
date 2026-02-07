#include "rix/ipc/signal.hpp"
#include <thread>

#include <gtest/gtest.h>

using namespace rix::ipc;

volatile sig_atomic_t flag = 0;

static void handle(int signum) { flag = 1; }

TEST(SignalDeathTest, TestDestructor) {
    {
        Signal sig1(SIGINT);
        EXPECT_EQ(sig1.signum(), SIGINT);
        EXPECT_FALSE(sig1.wait(rix::util::Duration(0)));
        EXPECT_TRUE(sig1.raise());
        EXPECT_TRUE(sig1.wait(rix::util::Duration(0)));
    }

    {
        Signal sig2(SIGINT);
        EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    }

    ASSERT_DEATH(::raise(SIGINT), "");
}

TEST(SignalDeathTest, TestDestructor2) {
    {
        Signal sig1(SIGINT);
        EXPECT_EQ(sig1.signum(), SIGINT);
        EXPECT_FALSE(sig1.wait(rix::util::Duration(0)));
        EXPECT_TRUE(sig1.raise());
    }

    {
        Signal sig2(SIGINT);
        EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    }

    ASSERT_DEATH(::raise(SIGINT), "");
}

TEST(SignalTest, TestConstructorDuplicate) {
    ASSERT_THROW({
        Signal sig1(SIGINT); 
        Signal sig2(SIGINT);
    }, std::invalid_argument);
}

TEST(SignalTest, TestConstructorFail) {
    ASSERT_THROW({
        Signal sig1(33);
    }, std::invalid_argument);
    
    ASSERT_THROW({
        Signal sig1(-1);
    }, std::invalid_argument);
}

TEST(SignalTest, TestConstructor) {
    Signal sig(SIGINT);
    EXPECT_EQ(sig.signum(), SIGINT);
    EXPECT_FALSE(sig.wait(rix::util::Duration(0)));
    EXPECT_TRUE(sig.raise());
    EXPECT_TRUE(sig.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig.wait(rix::util::Duration(0)));
    EXPECT_TRUE(sig.kill(getpid()));
    EXPECT_TRUE(sig.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig.wait(rix::util::Duration(0)));
}

TEST(SignalTest, TestMoveConstructor) {
    Signal sig1(SIGPIPE);
    Signal sig2(std::move(sig1));

    EXPECT_LT(sig1.signum(), 0) << "Object that has been moved should have invalid signum.";
    EXPECT_FALSE(sig1.wait(rix::util::Duration(0))) << "Signal::wait should fail immediately with invalid signum.";
    EXPECT_FALSE(sig1.raise()) << "Signal::raise should fail immediately with invalid signum.";
    EXPECT_FALSE(sig1.kill(getpid())) << "Signal::kill should fail immediately with invalid signum.";

    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    EXPECT_TRUE(sig2.raise());
    EXPECT_FALSE(sig1.wait(rix::util::Duration(0))) << "Signal::wait should fail immediately with invalid signum.";
    EXPECT_TRUE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    EXPECT_TRUE(sig2.kill(getpid()));
    EXPECT_TRUE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
}

TEST(SignalDeathTest, TestMoveAssignmentOperator) {
    Signal sig1(SIGPIPE);
    Signal sig2(SIGINT);
    sig2 = std::move(sig1);

    ASSERT_DEATH(::raise(SIGINT), "");
    ASSERT_NO_FATAL_FAILURE(::raise(SIGPIPE));

    EXPECT_LT(sig1.signum(), 0) << "Object that has been moved should have invalid signum.";
    EXPECT_FALSE(sig1.wait(rix::util::Duration(0))) << "Signal::wait should fail immediately with invalid signum.";
    EXPECT_FALSE(sig1.raise()) << "Signal::raise should fail immediately with invalid signum.";
    EXPECT_FALSE(sig1.kill(getpid())) << "Signal::kill should fail immediately with invalid signum.";

    EXPECT_TRUE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    EXPECT_TRUE(sig2.raise());
    EXPECT_FALSE(sig1.wait(rix::util::Duration(0))) << "Signal::wait should fail immediately with invalid signum.";
    EXPECT_TRUE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    EXPECT_TRUE(sig2.kill(getpid()));
    EXPECT_TRUE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
}

TEST(SignalTest, TestMultiple) {
    Signal sig1(SIGINT);
    Signal sig2(SIGPIPE);
    Signal sig3(SIGIO);

    ::raise(SIGINT);
    EXPECT_TRUE(sig1.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig3.wait(rix::util::Duration(0)));

    EXPECT_FALSE(sig1.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig3.wait(rix::util::Duration(0)));

    ::raise(SIGPIPE);
    EXPECT_FALSE(sig1.wait(rix::util::Duration(0)));
    EXPECT_TRUE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig3.wait(rix::util::Duration(0)));

    EXPECT_FALSE(sig1.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig3.wait(rix::util::Duration(0)));

    ::raise(SIGIO);
    EXPECT_FALSE(sig1.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    EXPECT_TRUE(sig3.wait(rix::util::Duration(0)));

    EXPECT_FALSE(sig1.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig2.wait(rix::util::Duration(0)));
    EXPECT_FALSE(sig3.wait(rix::util::Duration(0)));
}

TEST(SignalTest, TestWaitNonzero) {
    Signal sig(SIGINT);
    EXPECT_FALSE(sig.wait(rix::util::Duration(0)));

    bool status = false;
    std::thread sig_thr([&sig, &status](){
        status = sig.wait(rix::util::Duration(10));
    });

    ::raise(SIGINT);
    sig_thr.join();
    EXPECT_TRUE(status);
    EXPECT_FALSE(sig.wait(rix::util::Duration(0)));
}

TEST(SignalTest, TestWaitNonzeroTimed) {
    Signal sig(SIGINT);
    EXPECT_FALSE(sig.wait(rix::util::Duration(0)));

    rix::util::Duration wait_time(0);
    bool status = false;
    std::thread sig_thr([&sig, &status, &wait_time](){
        rix::util::Timer timer;
        timer.start();
        status = sig.wait(rix::util::Duration(10));
        timer.stop();
        wait_time = timer.get();
    });

    rix::util::sleep_for(rix::util::Duration(2));

    ::raise(SIGINT);
    sig_thr.join();
    EXPECT_TRUE(status);
    EXPECT_NEAR(wait_time.to_milliseconds(), 2000, 100);
    EXPECT_FALSE(sig.wait(rix::util::Duration(0)));
}

TEST(SignalTest, TestRaise) {
    Signal sig(SIGINT);
    ASSERT_EQ(flag, 0);
    ::signal(SIGINT, handle);
    EXPECT_TRUE(sig.raise());
    ASSERT_EQ(flag, 1);
    flag = 0;
    ::signal(SIGINT, SIG_DFL);
}

TEST(SignalTest, TestKill) {
    Signal sig(SIGINT);
    ASSERT_EQ(flag, 0);
    ::signal(SIGINT, handle);
    EXPECT_TRUE(sig.kill(getpid()));
    ASSERT_EQ(flag, 1);
    flag = 0;
    ::signal(SIGINT, SIG_DFL);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::FLAGS_gtest_death_test_style = "threadsafe";
  return RUN_ALL_TESTS();
}