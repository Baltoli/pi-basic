#include <string>
#include <iostream>

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
  SECTION("parser parses and") {
    std::string source = "and";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::And);
  }
  SECTION("parser parses or") {
    std::string source = "or";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Or);
  }
  SECTION("parser doesn't parse invalid operators") {
    std::string source = "fiow";
    Parser p(source);
    auto op = p.parseOperator();
    REQUIRE(op == AST::Invalid);
  }
}

TEST_CASE("parser can parse unary operators", "[parser]") {
  SECTION("parser parses not") {
    std::string source = "not";
    Parser p(source);
    auto op = p.parseUnaryOperator();
    REQUIRE(op == AST::Not);
  }

  SECTION("parser won't parse anything else") {
    std::string source = "pweofj";
    Parser p(source);
    auto op = p.parseUnaryOperator();
    REQUIRE(op == AST::UnaryInvalid);
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
    std::string source = "a";
    Parser p(source);
    auto v = p.parseVariable();
    auto list = p.parseExpressionList();

    REQUIRE(list.size() == 0);
  }

  SECTION("parser can parse multiple expressions") {
    std::string source = "1,  3+4,  9";
    Parser p(source);
    auto list = p.parseExpressionList();

    REQUIRE(list.size() == 3);
    auto first = dynamic_cast<AST::Literal *>(list[0]);
    auto second = dynamic_cast<AST::BinaryOp *>(list[1]);
    auto third = dynamic_cast<AST::Literal *>(list[2]);
    REQUIRE(first != nullptr);
    REQUIRE(second != nullptr);
    REQUIRE(third != nullptr);
    REQUIRE(first->value == 1);
    REQUIRE(second->type == AST::Add);
    REQUIRE(third->value == 9);
  }

  SECTION("parser stops when there are no more expressions") {
    std::string source = "1,";
    Parser p(source);
    auto list = p.parseExpressionList();

    REQUIRE(list.size() == 1);
    auto first = dynamic_cast<AST::Literal *>(list[0]);
    REQUIRE(first->value == 1);
    REQUIRE(*(p.column) == ',');
  }
}

TEST_CASE("parser can parse function calls", "[parser]") {
  SECTION("parser can parse 0-args") {
    std::string source = "func()";
    Parser p(source);
    auto call = p.parseCall();

    REQUIRE(call != nullptr);
    REQUIRE(call->name == "func");
    REQUIRE(call->args.size() == 0);
  }

  SECTION("parser can parse some args") {
    std::string source = "__name__(x, 2)";
    Parser p(source);
    auto call = p.parseCall();

    REQUIRE(call != nullptr);
    REQUIRE(call->name == "__name__");
    REQUIRE(call->args.size() == 2);
    auto first = dynamic_cast<AST::Variable *>(call->args[0]);
    auto second = dynamic_cast<AST::Literal *>(call->args[1]);
    REQUIRE(first->name == "x");
    REQUIRE(second->value == 2);
  }

  SECTION("parser won't parse incomplete") {
    std::string source = "broken(";
    Parser p(source);
    auto call = p.parseCall();

    REQUIRE(call == nullptr);
    REQUIRE(*(p.column) == 'b');
  }
}

TEST_CASE("parser can parse comparisons", "[parser]") {
  SECTION("parser can parse =") {
    std::string source = "name = 13";
    Parser p(source);
    auto co = p.parseComparison();

    REQUIRE(co != nullptr);
    auto left = dynamic_cast<AST::Variable *>(co->left);
    auto type = co->type;
    auto right = dynamic_cast<AST::Literal *>(co->right);
    REQUIRE(left->name == "name");
    REQUIRE(type == AST::Eq);
    REQUIRE(right->value == 13);
  }

  SECTION("parser can parse !=") {
    std::string source = "name != 13";
    Parser p(source);
    auto co = p.parseComparison();

    REQUIRE(co != nullptr);
    auto left = dynamic_cast<AST::Variable *>(co->left);
    auto type = co->type;
    auto right = dynamic_cast<AST::Literal *>(co->right);
    REQUIRE(left->name == "name");
    REQUIRE(type == AST::Neq);
    REQUIRE(right->value == 13);
  }

  SECTION("parser won't try to do arithmetic") {
    std::string source = "name + 13";
    Parser p(source);
    auto co = p.parseComparison();

    REQUIRE(co == nullptr);
  }
}

TEST_CASE("parser can parse boolean factors", "[parser]") {
  SECTION("parser can parse literal factors") {
    std::string source = "true";
    Parser p(source);
    auto fa = dynamic_cast<AST::BooleanLiteral *>(p.parseBooleanFactor());

    REQUIRE(fa != nullptr);
    REQUIRE(fa->value == true);
  }

  SECTION("parser can parse comparison factors") {
    std::string source = "3 = 4";
    Parser p(source);
    auto fa = dynamic_cast<AST::BinaryOp *>(p.parseBooleanFactor());

    REQUIRE(fa != nullptr);
    auto left = dynamic_cast<AST::Literal *>(fa->left);
    auto right = dynamic_cast<AST::Literal *>(fa->right);
    REQUIRE(left->value == 3);
    REQUIRE(right->value == 4);
  }

  SECTION("parser can parse parens factors") {
    std::string source = "( false  )";
    Parser p(source);
    auto fa = dynamic_cast<AST::BooleanLiteral *>(p.parseBooleanFactor());
    
    REQUIRE(fa != nullptr);
    REQUIRE(fa->value == false);
  }

  SECTION("parser can parse negated factors") {
    std::string source = "not false";
    Parser p(source);
    auto fa = dynamic_cast<AST::UnaryOp *>(p.parseBooleanFactor());
    
    REQUIRE(fa != nullptr);
    auto op = dynamic_cast<AST::BooleanLiteral *>(fa->operand);
    REQUIRE(op != nullptr);
    REQUIRE(op->value == false);
    REQUIRE(fa->type == AST::Not);
  }
}

TEST_CASE("parser can parse boolean terms", "[parser]") {
  SECTION("parser can parse a factor term") {
    std::string source = "true";
    Parser p(source);
    auto fa = dynamic_cast<AST::BooleanLiteral *>(p.parseBooleanTerm());

    REQUIRE(fa != nullptr);
    REQUIRE(fa->value == true);
  }

  SECTION("parse can parse an and term") {
    std::string source = "true and false";
    Parser p(source);
    auto te = dynamic_cast<AST::BinaryOp *>(p.parseBooleanTerm());

    REQUIRE(te != nullptr);
    auto left = dynamic_cast<AST::BooleanLiteral *>(te->left);
    auto right = dynamic_cast<AST::BooleanLiteral *>(te->right);
    REQUIRE(left != nullptr);
    REQUIRE(right != nullptr);
    REQUIRE(left->value == true);
    REQUIRE(right->value == false);
    REQUIRE(te->type == AST::And);
  }
}

TEST_CASE("parser can parse booleans", "[parser]") {
  SECTION("parser can parse a term boolean") {
    std::string source = "true";
    Parser p(source);
    auto fa = dynamic_cast<AST::BooleanLiteral *>(p.parseBoolean());

    REQUIRE(fa != nullptr);
    REQUIRE(fa->value == true);
  }

  SECTION("parser can parse an or boolean") {
    std::string source = "true or false";
    Parser p(source);
    auto te = dynamic_cast<AST::BinaryOp *>(p.parseBoolean());

    REQUIRE(te != nullptr);
    auto left = dynamic_cast<AST::BooleanLiteral *>(te->left);
    auto right = dynamic_cast<AST::BooleanLiteral *>(te->right);
    REQUIRE(left != nullptr);
    REQUIRE(right != nullptr);
    REQUIRE(left->value == true);
    REQUIRE(right->value == false);
    REQUIRE(te->type == AST::Or);
  }
}

TEST_CASE("parser can parse assignment", "[parser]") {
  SECTION("parser can parse simple assignment") {
    std::string source = "x <- 34";
    Parser p(source);
    auto as = p.parseAssign();

    REQUIRE(as != nullptr);
    REQUIRE(as->name == "x");
    auto value = dynamic_cast<AST::Literal *>(as->value);
    REQUIRE(value != nullptr);
    REQUIRE(value->value == 34);
  }
}

TEST_CASE("parser can parse statements", "[parser]") {
  SECTION("parser can parse assign statements") {
    std::string source = "ys <- 67";
    Parser p(source);
    auto as = dynamic_cast<AST::Assign *>(p.parseStatement());

    REQUIRE(as != nullptr);
    REQUIRE(as->name == "ys");
    auto val = dynamic_cast<AST::Literal *>(as->value);
    REQUIRE(val->value == 67);
  }

  SECTION("parser can parse if-end statements") {
    std::string source = R"(
      if true
        g <- 7
      end
    )";
    Parser p(source);
    auto as = dynamic_cast<AST::If *>(p.parseStatement());

    REQUIRE(as != nullptr);
    auto list = dynamic_cast<AST::StatementList *>(as->trueBody);
    REQUIRE(list != nullptr);
    REQUIRE(list->statements.size() == 1);
    REQUIRE(as->falseBody == nullptr);
  }

  SECTION("parser can parse if-else statements") {
    std::string source = R"(
      if true
        z <- 3
      else
        y <- 9
        y <- 9
        if x > y
          d <- 89
        end
      end
    )";
    Parser p(source);
    auto as = dynamic_cast<AST::If *>(p.parseStatement());

    REQUIRE(as != nullptr);
    auto list = dynamic_cast<AST::StatementList *>(as->trueBody);
    REQUIRE(list != nullptr);
    REQUIRE(list->statements.size() == 1);
    auto flist = dynamic_cast<AST::StatementList *>(as->falseBody);
    REQUIRE(flist != nullptr);
    REQUIRE(flist->statements.size() == 3);
  }

  SECTION("parser can parse while statements") {
    std::string source = R"(
      while true
        x <- 45
      end
    )";

    Parser p(source);
    auto as = dynamic_cast<AST::WhileLoop *>(p.parseStatement());

    REQUIRE(as != nullptr);
  }
}

TEST_CASE("parser can parse statement lists", "[parser]") {
  SECTION("parser can parse empty statement lists") {
    std::string source = "x";
    Parser p(source);
    auto x = p.parseVariable();
    auto ss = p.parseStatementList();

    REQUIRE(ss->statements.size() == 0);
  }

  SECTION("parse can parse several statements") {
    std::string source = R"(
      x <- 67
      y <- 56
    )";
    Parser p(source);
    auto ss = p.parseStatementList();

    REQUIRE(ss->statements.size() == 2);
  }
}

TEST_CASE("parser can parse argument lists", "[parser]") {
  SECTION("parser can parse some argument list") {
    std::string source = "a, bb, ccc";
    Parser p(source);
    auto al = p.parseArgumentList();

    REQUIRE(al.size() == 3);
  }
}

TEST_CASE("parser can parse function declarations", "[parser]") {
  SECTION("parser can parse function") {
    std::string source = R"(
      function fun(x, y, z)
        x <- 45
        y <- 78
      end
    )";
    Parser p(source);
    auto f = p.parseFunctionDeclaration();

    REQUIRE(f != nullptr);
    REQUIRE(f->name == "fun");
    REQUIRE(f->params.size() == 3);
    auto body = dynamic_cast<AST::StatementList *>(f->body);
    REQUIRE(body != nullptr);
    REQUIRE(body->statements.size() == 2);
  }
}

TEST_CASE("parser can parse function lists", "[parser]") {
  SECTION("parser parses the list correctly") {
    std::string source = R"(
      function fun(x, y, z)
        x <- 45
        y <- 78
      end
      function g(w)
        x <- 432
      end
    )";
    Parser p(source);
    auto f = p.parseFunctionList();

    REQUIRE(f != nullptr);
    REQUIRE(f->functions.size() == 2);
  }
}

TEST_CASE("parser can parse programs", "[program]") {
  SECTION("parser can parse a full program") {
    std::string source = R"(
      function x(y, z)
        x <- 34
        z <- x
      end

      function d(a, c)
        z <- c
      end

      y <- [d(3, 4, 5)]
      x <- 4 * y
      z <- 23 + [x * y]
    )";
    Parser p(source);
    auto prog = p.parseProgram();

    REQUIRE(prog != nullptr);
    auto funcs = dynamic_cast<AST::FunctionList *>(prog->functions);
    auto stmts = dynamic_cast<AST::StatementList *>(prog->body);
    REQUIRE(funcs->functions.size() == 2);
    REQUIRE(stmts->statements.size() == 3);
  }
}
