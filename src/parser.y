// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------

%skeleton "lalr1.cc"
%require "3.5"

%defines

%define api.token.raw
%define api.parser.class { parser }
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { lqvm }

%code requires {

#include "ReducibleGraph.h"

#include <iostream>
#include <string>
#include <vector>
#include <optional>

namespace lqvm{
  class scanner;
  class driver;
}

using namespace lqvm;

}

%code top
{

#include "driver.h"
#include "scanner.h"
#include "parser.h"

#include <iostream>
#include <string>

static lqvm::parser::symbol_type yylex(lqvm::scanner &p_scanner, lqvm::driver &p_driver) {
  return p_scanner.get_next_token();
}

}