#include <string>

#include "parser.hh"
#include "catch.hh"

TEST_CASE("parser can be initialised properly", "[parser]") {
  std::string source = R"(
    this is a
    multi line
    string
  )";

  Parser p(source); 

  SECTION("parser splits by lines correctly") {
    REQUIRE(p.lines.size() == 3);
    REQUIRE(p.lines[0] == "this is a");
    REQUIRE(p.lines[1] == "multi line");
    REQUIRE(p.lines[2] == "string");
  }

  SECTION("parser initialises position correctly") {
    REQUIRE(p.line == p.lines.begin());
    REQUIRE(p.column == p.lines[0].begin());
  }
}

TEST_CASE("parser can parse numeric literals", "[parser]") {
  SECTION("parser parses simple literals") {
    std::string source = "432";
    Parser p(source);
    auto lit = p.parseLiteral();

    REQUIRE(lit != nullptr);
    REQUIRE(lit->value == 432);
  }

  SECTION("parser parses negative numbers") {
    std::string source = "-2";
    Parser p(source);
    auto lit = p.parseLiteral();

    REQUIRE(lit != nullptr);
    REQUIRE(lit->value == -2);
  }

  SECTION("parser does not parse non-numerics") {
    std::string source = "-abcdef";
    Parser p(source);
    auto it = p.column;
    auto lit = p.parseLiteral();

    REQUIRE(lit == nullptr);
    REQUIRE(p.column == it);
  }

  SECTION("parser parses until it can't") {
    std::string source = "   534abc";
    Parser p(source);
    auto it = p.column;
    auto lit = p.parseLiteral();

    REQUIRE(lit != nullptr);
    REQUIRE(lit->value == 534);
    REQUIRE(*(p.column) == 'a');
  }
}

TEST_CASE("parser can parse boolean literals", "[parser]") {
  SECTION("parser parses true") {
    std::string source = "true";
    Parser p(source);
    auto lit = p.parseBooleanLiteral();

    REQUIRE(lit != nullptr);
    REQUIRE(lit->value == true);
  }

  SECTION("parser parses false") {
    std::string source = "false";
    Parser p(source);
    auto lit = p.parseBooleanLiteral();

    REQUIRE(lit != nullptr);
    REQUIRE(lit->value == false);
  }

  SECTION("parser does not parse non-booleans") {
    std::string source = "notabool123";
    Parser p(source);
    auto lit = p.parseBooleanLiteral();

    REQUIRE(lit == nullptr);
  }

  SECTION("parser parses until it can't") {
    std::string source = "   true1234";
    Parser p(source);
    auto lit = p.parseBooleanLiteral();

    REQUIRE(lit != nullptr);
    REQUIRE(lit->value == true);
    REQUIRE(*(p.column) == '1');
  }
}
