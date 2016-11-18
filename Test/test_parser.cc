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

TEST_CASE("parser can parse variable names", "[parser]") {
  SECTION("parser parses simple name") {
    std::string source = "variable";
    Parser p(source);
    auto var = p.parseVariable();
    
    REQUIRE(var != nullptr);
    REQUIRE(var->name == "variable");
  }

  SECTION("parser parses name with underscores") {
    std::string source = "_some_variable_name";
    Parser p(source);
    auto var = p.parseVariable();
    
    REQUIRE(var != nullptr);
    REQUIRE(var->name == "_some_variable_name");
  }

  SECTION("parser stops at invalid characters") {
    std::string source = "   variable-name";
    Parser p(source);
    auto var = p.parseVariable();
    
    REQUIRE(var != nullptr);
    REQUIRE(var->name == "variable");
    REQUIRE(*(p.column) == '-');
  }

  SECTION("parser fails if starting with invalid character", "[parser]") {
    std::string source = "?-?variable-name";
    Parser p(source);
    auto var = p.parseVariable();

    REQUIRE(var == nullptr);
    REQUIRE(*(p.column) == '?');
  }
}

TEST_CASE("parser can parse operators", "[parser]") {
  SECTION("parser parses +") {
    std::string source = "+";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Add);
  }
  SECTION("parser parses -") {
    std::string source = "-";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Subtract);
  }
  SECTION("parser parses *") {
    std::string source = "*";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Multiply);
  }
  SECTION("parser parses /") {
    std::string source = "/";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Divide);
  }
  SECTION("parser parses %") {
    std::string source = "%";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Mod);
  }
  SECTION("parser parses =") {
    std::string source = "=";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Eq);
  }
  SECTION("parser parses !=") {
    std::string source = "!=";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Neq);
  }
  SECTION("parser parses >") {
    std::string source = ">";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Gt);
  }
  SECTION("parser parses <") {
    std::string source = "<";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Lt);
  }
  SECTION("parser parses >=") {
    std::string source = ">=";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::GtEq);
  }
  SECTION("parser parses <=") {
    std::string source = "<=";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::LtEq);
  }
  SECTION("parser doesn't parse invalid operators") {
    std::string source = "fiow";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Invalid);
  }
}

TEST_CASE("parser can parse factors", "[parser]") {
  SECTION("parser can parse a literal factor") {
    std::string source = "6781";
    Parser p(source);
    auto fa = dynamic_cast<AST::Literal *>(p.parseFactor());

    REQUIRE(fa != nullptr);
    REQUIRE(fa->value == 6781);
  }

  SECTION("parser can parse a variable factor") {
    std::string source = "xyz";
    Parser p(source);
    auto fa = dynamic_cast<AST::Variable *>(p.parseFactor());

    REQUIRE(fa != nullptr);
    REQUIRE(fa->name == "xyz");
  }

  SECTION("parser can parse a parens factor") {
    std::string source = "( 1 )";
    Parser p(source);
    auto fa = dynamic_cast<AST::Literal *>(p.parseFactor());

    REQUIRE(fa != nullptr);
    REQUIRE(fa->value == 1);
  }

  SECTION("parser can parse a deref factor") {
    std::string source = "[ 123 ]";
    Parser p(source);
    auto fa = dynamic_cast<AST::Deref *>(p.parseFactor());

    REQUIRE(fa != nullptr);
    auto address = dynamic_cast<AST::Literal *>(fa->address);
    REQUIRE(address != nullptr);
    REQUIRE(address->value == 123);
  }

  SECTION("parser can parse a call factor") {
    std::string source = "f(123, b)";
    Parser p(source);
    auto fa = dynamic_cast<AST::Call *>(p.parseFactor());

    REQUIRE(fa != nullptr);
    REQUIRE(fa->name == "f");
    REQUIRE(fa->args.size() == 2);
  }
}

TEST_CASE("parser can parse terms", "[parser]") {
  SECTION("parser can parse a lone factor term") {
    std::string source = "-89";
    Parser p(source);
    auto te = dynamic_cast<AST::Literal *>(p.parseTerm());

    REQUIRE(te != nullptr);
    REQUIRE(te->value == -89);
  }

  SECTION("parser can parse a mul-op term") {
    std::string source = "34 *   67";
    Parser p(source);
    auto te = dynamic_cast<AST::BinaryOp *>(p.parseTerm());

    REQUIRE(te != nullptr);
    REQUIRE(te->type == AST::Multiply);
  }

  SECTION("parser won't parse an add-op") {
    std::string source = "34+67";
    Parser p(source);
    auto te = dynamic_cast<AST::Literal *>(p.parseTerm());

    REQUIRE(te != nullptr);
    REQUIRE(te->value == 34);
    REQUIRE(*(p.column) == '+');
  }
}

TEST_CASE("parser can parse expressions", "[parser]") {
  SECTION("parser can parse a lone literal expression") {
    std::string source = "-89";
    Parser p(source);
    auto te = dynamic_cast<AST::Literal *>(p.parseExpression());

    REQUIRE(te != nullptr);
    REQUIRE(te->value == -89);
  }

  SECTION("parser can parse an add-op term") {
    std::string source = "34 +   67";
    Parser p(source);
    auto te = dynamic_cast<AST::BinaryOp *>(p.parseExpression());

    REQUIRE(te != nullptr);
    REQUIRE(te->type == AST::Add);
  }

  SECTION("parser will also parse a mul-op") {
    std::string source = "34*67";
    Parser p(source);
    auto te = dynamic_cast<AST::BinaryOp *>(p.parseExpression());

    REQUIRE(te != nullptr);
    REQUIRE(te->type == AST::Multiply);
  }
}

TEST_CASE("parser can parse a dereference", "[parser]") {
  SECTION("parser can parse simple deref") {
    std::string source = "[ 34  ]";
    Parser p(source);
    auto de = p.parseDeref();
    
    REQUIRE(de != nullptr);
    auto lit = dynamic_cast<AST::Literal *>(de->address);
    REQUIRE(lit != nullptr);
    REQUIRE(lit->value == 34);
  }

  SECTION("parser sets the cursor correctly") {
    std::string source = "[ 34  ]a";
    Parser p(source);
    auto de = p.parseDeref();
    
    REQUIRE(*(p.column) == 'a');
  }

  SECTION("parser fails if unbalanced") {
    std::string source = "[ 34";
    Parser p(source);
    auto de = p.parseDeref();
    
    REQUIRE(de == nullptr);
    REQUIRE(*(p.column) == '[');
  }
}

TEST_CASE("parser can parse expression lists", "[parser]") {
  SECTION("parser can parse an empty expression list") {
    std::string source = "";
    Parser p(source);
    auto list = p.parseExpressionList();

    REQUIRE(list.size() == 0);
  }
}
