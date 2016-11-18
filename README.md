# πBasic

A small language and runtime inspired by legacy Basic implementations.

## Language and Features

The abstract machine (for now) of this language is a simple imperative one with
a comparative lack of features. Initial implementation will be sort of untyped -
only data type will initially be 32-bit integers. Add in more + some kind of a
type system later on.

### Variables & Assignment

Variables will be assigned to by the syntax `x <- value`.

### Arithmetic & Booleans

Normal arithmetic ops `+ * - / %` all supported, along with `= /= > < >= <=` for
conditionals.

### Loops & Conditionals

One loop construct:
  
    while cond do
      code
    end

Conditionals:

    if cond do
      code
    end

    if cond do
      code
    else
      code'
    end

### Memory

4K bytes of word-addressable memory. Only data type is currently 32-bit integer,
so use them as pointers as well:

    x <- [address]

Only values 0..1023 will be valid addresses to access - crash and print a nice
error message if out of range.

### Multi-file & Standard Lib

Future features. Currently just implement a `print` statement that prints a
single integer to the console.

## Grammar

    variable ::=
        valid_name    // design later

    number ::= 
        [0-9]+
    
    literal ::= 
        '-' number 
      | number
    
    factor ::=
        literal
      | variable
      | '(' expr ')'  // parens
      | '[' expr ']'  // deref
      | call

    term ::=
        factor
      | factor '*' factor
      | factor '/' factor

    expr ::=
        term
      | term '+' term
      | term '-' term

    comp ::=
      | expr '=' expr
      | expr '/=' expr
      | expr '>' expr
      | expr '<' expr
      | expr '>=' expr
      | expr '<=' expr

    boolf ::=
        true
      | false
      | comp
      | bool 'or' bool
      | 'not' bool
      | '(' bool ')'

    bool ::=
      | boolf
      | boolf 'and' boolf

    statement ::=
        variable '<-' expr
      | while
      | if

    statements ::=
        statement
      | statement '\n' statements

    while ::=
        'while' bool 'do\n' statements '\nend'

    if ::=
        'if' bool 'do\n' statements '\nend'
      | 'if' bool 'then\n' statements '\nelse\n' statements '\nend'

    variables ::=
        variable
      | variable ',' variables

    args ::=
        '(' variables ')'
      | '()'

    function ::=
        'function' variable args '\n' statements '\n' end

    exprs ::=
        expr
      | expr ',' exprs
      | '()'

    call ::=
        variable '(' exprs ')'

    functions ::=
        function
      | function '\n' functions

    program ::=
      functions '\n' statements
