//
// Created by maxng on 27/07/2024.
//

#include <gtest/gtest.h>

#include <rebar/string/string_engine.hpp>
#include <rebar/lexical_analysis/lexical_analyzer.hpp>
#include <rebar/semantic_analysis/semantic_analyzer.hpp>
#include <rebar/util/print.hpp>

class semantic_analyzer_test : public testing::Test {
protected:
    rebar::string_engine m_string_engine;
    rebar::lexical_analyzer m_lexical_analyzer;
    rebar::semantic_analyzer m_semantic_analyzer;

    semantic_analyzer_test() :
        m_lexical_analyzer(m_string_engine)
    {}
};

TEST_F(semantic_analyzer_test, binary_operators) {
    {
        rebar::lexical_unit lu("3 + 4");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { addition { INTEGER (3), INTEGER (4), }, }"
        );
    }
    {
        rebar::lexical_unit lu("3 + 4 + 5 + 6");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { addition { addition { addition { INTEGER (3), INTEGER (4), }, INTEGER (5), }, INTEGER (6), }, }"
        );
    }
}

TEST_F(semantic_analyzer_test, unary_operators) {
    {
        rebar::lexical_unit lu("some_identifier++");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { postfix_increment { IDENTIFIER (some_identifier), }, }"
        );
    }
    {
        rebar::lexical_unit lu("++some_identifier");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { prefix_increment { IDENTIFIER (some_identifier), }, }"
        );
    }
}

TEST_F(semantic_analyzer_test, unary_operators_precedence) {
    {
        rebar::lexical_unit lu("++some_identifier");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { prefix_increment { IDENTIFIER (some_identifier), }, }"
        );
    }
    {
        rebar::lexical_unit lu("some_identifier++");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { postfix_increment { IDENTIFIER (some_identifier), }, }"
        );
    }
    {
        rebar::lexical_unit lu("some_identifier++++");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { postfix_increment { postfix_increment { IDENTIFIER (some_identifier), }, }, }"
        );
    }
    {
        rebar::lexical_unit lu("++++some_identifier");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { prefix_increment { prefix_increment { IDENTIFIER (some_identifier), }, }, }"
        );
    }
}

TEST_F(semantic_analyzer_test, enclosed_operators) {
    {
        rebar::lexical_unit lu("some_identifier[3+5]");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { index { IDENTIFIER (some_identifier), addition { INTEGER (3), INTEGER (5), }, }, }"
        );
    }
    {
        rebar::lexical_unit lu("some_identifier[3+5][12]");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { index { index { IDENTIFIER (some_identifier), addition { INTEGER (3), INTEGER (5), }, }, INTEGER (12), }, }"
        );
    }
    {
        rebar::lexical_unit lu("(some_identifier)[3+5][12]");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { index { index { IDENTIFIER (some_identifier), addition { INTEGER (3), INTEGER (5), }, }, INTEGER (12), }, }"
        );
    }
    {
        rebar::lexical_unit lu("(some_identifier + 3)[3+5][12]");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { index { index { addition { IDENTIFIER (some_identifier), INTEGER (3), }, addition { INTEGER (3), INTEGER (5), }, }, INTEGER (12), }, }"
        );
    }
    {
        rebar::lexical_unit lu("(some_identifier + 3)[(3+5)][12]");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { index { index { addition { IDENTIFIER (some_identifier), INTEGER (3), }, addition { INTEGER (3), INTEGER (5), }, }, INTEGER (12), }, }"
        );
    }
    {
        rebar::lexical_unit lu("(some_identifier[2 + 3] + 3)[(3+5)][12]");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { index { index { addition { index { IDENTIFIER (some_identifier), addition { INTEGER (2), INTEGER (3), }, }, INTEGER (3), }, addition { INTEGER (3), INTEGER (5), }, }, INTEGER (12), }, }"
        );
    }
    {
        rebar::lexical_unit lu("some_identifier(23)");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { call { IDENTIFIER (some_identifier), INTEGER (23), }, }"
        );
    }
    {
        rebar::lexical_unit lu("some_identifier(23, 40 + 4, 10)");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { call { IDENTIFIER (some_identifier), sequence { INTEGER (23), addition { INTEGER (40), INTEGER (4), }, INTEGER (10), }, }, }"
        );
    }
    {
        rebar::lexical_unit lu("(some_identifier)((23), (40 + 4), (10))");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { call { IDENTIFIER (some_identifier), sequence { INTEGER (23), addition { INTEGER (40), INTEGER (4), }, INTEGER (10), }, }, }"
        );
    }
    {
        rebar::lexical_unit lu("(some_identifier.id)((23), (40 + 4), (10))");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { call { dot_index { IDENTIFIER (some_identifier), IDENTIFIER (id), }, sequence { INTEGER (23), addition { INTEGER (40), INTEGER (4), }, INTEGER (10), }, }, }"
        );
    }
    {
        rebar::lexical_unit lu("here = somewhere = (some_identifier.id)((23), (40 + 4), (10))");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { assignment { IDENTIFIER (here), assignment { IDENTIFIER (somewhere), call { dot_index { IDENTIFIER (some_identifier), IDENTIFIER (id), }, sequence { INTEGER (23), addition { INTEGER (40), INTEGER (4), }, INTEGER (10), }, }, }, }, }"
        );
    }
}

TEST_F(semantic_analyzer_test, combinations) {
    {
        rebar::lexical_unit lu("some + ++token.some[here.d][4] + \"hello, world\"; another = statement = (here)((4 + 3), 5, 2)(5) + 45;");
        rebar::semantic_unit su;

        m_lexical_analyzer.perform_analysis(lu);
        m_semantic_analyzer.perform_analysis(su, lu);

        EXPECT_EQ(
            su.base_scope().to_string(),
            "scope { addition { addition { IDENTIFIER (some), prefix_increment { index { index { dot_index { IDENTIFIER (token), IDENTIFIER (some), }, dot_index { IDENTIFIER (here), IDENTIFIER (d), }, }, INTEGER (4), }, }, }, STRING (\"hello, world\"), }, assignment { IDENTIFIER (another), assignment { IDENTIFIER (statement), addition { call { call { IDENTIFIER (here), sequence { addition { INTEGER (4), INTEGER (3), }, INTEGER (5), INTEGER (2), }, }, INTEGER (5), }, INTEGER (45), }, }, }, }"
        );
    }
}
