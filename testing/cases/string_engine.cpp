//
// Created by maxng on 7/17/2024.
//

#include <gtest/gtest.h>

#include <rebar/string/string_engine.hpp>
#include <rebar/string/string.hpp>

class string_engine_test : public testing::Test {
protected:
    rebar::string_engine m_string_engine;
};

TEST_F(string_engine_test, creation_lifetime) {
    EXPECT_FALSE(m_string_engine.string_exists("Hello, world!"));

    // Test creation and end-of-scope cleanup (garbage collection).
    {
        auto const str1 = m_string_engine.str("Hello, world!");

        EXPECT_TRUE(m_string_engine.string_exists("Hello, world!"));
    }

    EXPECT_FALSE(m_string_engine.string_exists("Hello, world!"));
}

TEST_F(string_engine_test, equality) {
    auto const str1 = m_string_engine.str("Hello, world!");
    auto const str2 = m_string_engine.str("Hello, world!");

    EXPECT_EQ(str1, str2);
}

