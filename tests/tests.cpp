#include <gtest/gtest.h>

#include <thread>
#include <chrono>

namespace
{
    TEST(Short, success)
    {
        EXPECT_TRUE(true);
    }
    
    TEST(Short, error)
    {
        EXPECT_TRUE(false);
    }
    
    TEST(Short, two_errors)
    {
        EXPECT_TRUE(false);
        EXPECT_TRUE(false);
    }
    
    TEST(Short, three_errors)
    {
        EXPECT_TRUE(false);
        EXPECT_TRUE(false);
        EXPECT_TRUE(false);
    }
    
    TEST(Short, DISABLED_not_executing)
    {
        EXPECT_TRUE(false);
    }
    
    TEST(Long, success)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        EXPECT_TRUE(true);
    }
    
    TEST(Long, error)
    {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        EXPECT_TRUE(false);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}

