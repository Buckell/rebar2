//
// Created by maxng on 7/17/2024.
//

#include <gtest/gtest.h>

#include <rebar/string/string_engine.hpp>
#include <rebar/lexical_analysis/lexical_analyzer.hpp>

class lexical_analyzer_test : public testing::Test {
protected:
    rebar::string_engine m_string_engine;
    rebar::lexical_analyzer m_lexical_analyzer;

    lexical_analyzer_test() :
        m_lexical_analyzer(m_string_engine)
    {}
};

TEST_F(lexical_analyzer_test, basic_string_escape_sequences) {
    auto const initial_string = R"(Hello, \n\tworld!)";

    auto const processed_string = m_lexical_analyzer.process_string(initial_string);

    EXPECT_EQ(processed_string, "Hello, \n\tworld!");
}

TEST_F(lexical_analyzer_test, advanced_string_escape_sequences) {
    // TODO: Implement tests for Unicode, hex, etc. escape sequences.
}

TEST_F(lexical_analyzer_test, integer_parsing) {
    rebar::integer result = rebar::lexical_analyzer::parse_integer("100'000'000");
    EXPECT_EQ(result, 100'000'000);

    result = rebar::lexical_analyzer::parse_integer("123456789");
    EXPECT_EQ(result, 123456789);

    result = rebar::lexical_analyzer::parse_integer("-123456789");
    EXPECT_EQ(result, -123456789);

    result = rebar::lexical_analyzer::parse_integer("-123'456'789");
    EXPECT_EQ(result, -123'456'789);
}

TEST_F(lexical_analyzer_test, number_parsing) {
    rebar::number result = rebar::lexical_analyzer::parse_number("100'000'000");
    EXPECT_EQ(result, 100'000'000);

    result = rebar::lexical_analyzer::parse_number("123456789");
    EXPECT_EQ(result, 123456789);

    result = rebar::lexical_analyzer::parse_number("-123456789");
    EXPECT_EQ(result, -123456789);

    result = rebar::lexical_analyzer::parse_number("-123'456'789");
    EXPECT_EQ(result, -123'456'789);

    result = rebar::lexical_analyzer::parse_number("123'456.789");
    EXPECT_EQ(result, 123'456.789);

    result = rebar::lexical_analyzer::parse_number("-123'456.789");
    EXPECT_EQ(result, -123'456.789);

    result = rebar::lexical_analyzer::parse_number(".123456");
    EXPECT_EQ(result, .123456);

    result = rebar::lexical_analyzer::parse_number("-.123456");
    EXPECT_EQ(result, -.123456);

    result = rebar::lexical_analyzer::parse_number(".123'456");
    EXPECT_EQ(result, .123'456);

    result = rebar::lexical_analyzer::parse_number("-.123'456");
    EXPECT_EQ(result, -.123'456);

    result = rebar::lexical_analyzer::parse_number(".123'''45''6");
    EXPECT_EQ(result, .123'456);

    result = rebar::lexical_analyzer::parse_number("-.1'''23''45'6");
    EXPECT_EQ(result, -.123'456);
}

TEST_F(lexical_analyzer_test, token_parsing_strings) {
    {
        rebar::lexical_unit lu(R"(hello"Hello, world!"goodbye)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 3);
        ASSERT_GT(tokens.size(), 2);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_string());
        EXPECT_EQ(tokens[1].get_string().view(), "Hello, world!");

        ASSERT_TRUE(tokens[2].is_identifier());
        EXPECT_EQ(tokens[2].get_string().view(), "goodbye");
    }
    {
        rebar::lexical_unit lu(R"("Hello,"goodbye"world!")");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 3);
        ASSERT_GT(tokens.size(), 2);

        ASSERT_TRUE(tokens[0].is_string());
        EXPECT_EQ(tokens[0].get_string().view(), "Hello,");

        ASSERT_TRUE(tokens[1].is_identifier());
        EXPECT_EQ(tokens[1].get_string().view(), "goodbye");

        ASSERT_TRUE(tokens[2].is_string());
        EXPECT_EQ(tokens[2].get_string().view(), "world!");
    }
}

TEST_F(lexical_analyzer_test, token_parsing_symbols) {
    {
        rebar::lexical_unit lu(R"(hello+world)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 3);
        ASSERT_GT(tokens.size(), 2);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_symbol());
        EXPECT_EQ(tokens[1].get_symbol(), rebar::symbol::plus);

        ASSERT_TRUE(tokens[2].is_identifier());
        EXPECT_EQ(tokens[2].get_string().view(), "world");
    }
    {
        rebar::lexical_unit lu(R"(hello+=world)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 3);
        ASSERT_GT(tokens.size(), 2);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_symbol());
        EXPECT_EQ(tokens[1].get_symbol(), rebar::symbol::plus_equals);

        ASSERT_TRUE(tokens[2].is_identifier());
        EXPECT_EQ(tokens[2].get_string().view(), "world");
    }
    {
        rebar::lexical_unit lu(R"(truehello+worldtrueworldfalseworld+=worldfalse)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 5);
        ASSERT_GT(tokens.size(), 4);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "truehello");

        ASSERT_TRUE(tokens[1].is_symbol());
        EXPECT_EQ(tokens[1].get_symbol(), rebar::symbol::plus);

        ASSERT_TRUE(tokens[2].is_identifier());
        EXPECT_EQ(tokens[2].get_string().view(), "worldtrueworldfalseworld");

        ASSERT_TRUE(tokens[3].is_symbol());
        EXPECT_EQ(tokens[3].get_symbol(), rebar::symbol::plus_equals);

        ASSERT_TRUE(tokens[4].is_identifier());
        EXPECT_EQ(tokens[4].get_string().view(), "worldfalse");
    }
}

TEST_F(lexical_analyzer_test, token_parsing_integer) {
    {
        rebar::lexical_unit lu(R"(hello+=1'000'000)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 3);
        ASSERT_GT(tokens.size(), 2);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_symbol());
        EXPECT_EQ(tokens[1].get_symbol(), rebar::symbol::plus_equals);

        ASSERT_TRUE(tokens[2].is_integer());
        EXPECT_EQ(tokens[2].get_integer(), 1'000'000);
    }
    {
        rebar::lexical_unit lu(R"(hello + -1'000'000)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 3);
        ASSERT_GT(tokens.size(), 2);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_symbol());
        EXPECT_EQ(tokens[1].get_symbol(), rebar::symbol::plus);

        ASSERT_TRUE(tokens[2].is_integer());
        EXPECT_EQ(tokens[2].get_integer(), -1'000'000);
    }
    {
        rebar::lexical_unit lu(R"(hello 123456)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 2);
        ASSERT_GT(tokens.size(), 1);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_integer());
        EXPECT_EQ(tokens[1].get_integer(), 123456);
    }
}

TEST_F(lexical_analyzer_test, token_parsing_number) {
    {
        rebar::lexical_unit lu(R"(hello+=1'000'000.123)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 3);
        ASSERT_GT(tokens.size(), 2);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_symbol());
        EXPECT_EQ(tokens[1].get_symbol(), rebar::symbol::plus_equals);

        ASSERT_TRUE(tokens[2].is_number());
        EXPECT_EQ(tokens[2].get_number(), 1'000'000.123);
    }
    {
        rebar::lexical_unit lu(R"(hello + -1'000.001)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 3);
        ASSERT_GT(tokens.size(), 2);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_symbol());
        EXPECT_EQ(tokens[1].get_symbol(), rebar::symbol::plus);

        ASSERT_TRUE(tokens[2].is_number());
        EXPECT_EQ(tokens[2].get_number(), -1'000.001);
    }
    {
        rebar::lexical_unit lu(R"(hello .123456)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 2);
        ASSERT_GT(tokens.size(), 1);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_number());
        EXPECT_EQ(tokens[1].get_number(), .123456);
    }
    {
        rebar::lexical_unit lu(R"(hello -.123'456)");

        m_lexical_analyzer.perform_analysis(lu);

        auto const tokens = lu.tokens();

        EXPECT_EQ(tokens.size(), 2);
        ASSERT_GT(tokens.size(), 1);

        ASSERT_TRUE(tokens[0].is_identifier());
        EXPECT_EQ(tokens[0].get_string().view(), "hello");

        ASSERT_TRUE(tokens[1].is_number());
        EXPECT_EQ(tokens[1].get_number(), -.123'456);
    }
}

// TODO: Add more lexical analyzer tests (more symbols, literal combinations).