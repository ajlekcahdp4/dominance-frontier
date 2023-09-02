// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------

%{
#include <iostream>
#include <numeric>

#include "scanner.h"

#define yyterminate() 

using namespace lqvm;
%}

%option noyywrap nounput noinput nodefault 
%option c++
%option prefix="graph_"
%option yyclass="scanner"

number      (0|[1-9][0-9]*)

%%

"--"            { return parser::make_LINE(); }
{number}        { return parser::make_UNSIGNED(std::stoi(yytext)); }
[ \t\n\r]       { }
.               { throw parser::syntax_error{"Unexpected symbol"}; }
<<EOF>>         { return circuits::parser::make_EOF(); }

%%