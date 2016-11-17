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
