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
%define api.namespace { lqvm }

%code requires {

#include "ReducibleGraph.h"

#include <iostream>
#include <string>
#include <vector>
#include <optional>

namespace lqvm{
  class scanner;
  class Driver;
}

using namespace lqvm;

}

%code top
{

#include "driver.h"
#include "scanner.h"
#include "graph_parser.h"

#include <iostream>
#include <string>

static lqvm::parser::symbol_type yylex(lqvm::scanner &p_scanner, lqvm::Driver &p_driver) {
  return p_scanner.get_next_token();
}

}

%lex-param { lqvm::scanner &Scanner }
%lex-param { lqvm::Driver &Driver }

%parse-param { lqvm::scanner &Scanner }
%parse-param { lqvm::Driver &Driver }

%define parse.trace
%define parse.error verbose
%define api.token.prefix {TOKEN_}

%token LINE     "--"
%token <unsigned> UNSIGNED "unsigned"
%token EOF 0 "end of file"

%type <lqvm::GraphTy<lqvm::Node>> graph
%type <std::pair<unsigned, unsigned>> edge

%start all

%%

all: graph { Driver.G = std::move($1); }

graph: graph edge { 
    $$ = std::move($1);
    auto *From = $$.GetOrInsertNode($2.first);
    auto *To = $$.GetOrInsertNode($2.second);
    From->adoptChild(To);
  }
  | %empty { $$ = lqvm::GraphTy<Node>(); }

edge: UNSIGNED LINE UNSIGNED { $$ = std::make_pair($1, $3 ); }

%%

void lqvm::parser::error(const std::string &message) {
  throw std::runtime_error{message};
}