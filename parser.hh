#pragma once

#include <string>

#include "ast.hh"

namespace Parser {

/**
 * Top-level parsing function takes a string as a parameter - internally we'll
 * take a reference to the beginning of the string and pass it around as part of
 * a parser state.
 */
AST::Node *parse(std::string str);

AST::Node *parseNumericLiteral(std::string::iterator &it);

}
