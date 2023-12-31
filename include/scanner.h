// ----------------------------------------------------------------------------
//   "THE BEER-WARE LICENSE" (Revision 42):
//   <alex.rom23@mail.ru> wrote this file. As long as you retain this
//   notice you can do whatever you want with this stuff. If we meet some day,
//   and you think this stuff is worth it, you can buy me a beer in return.
// ----------------------------------------------------------------------------

#pragma once

#if !defined(yyFlexLexerOnce)
#undef yyFlexLexer
#define yyFlexLexer graph_FlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL lqvm::parser::symbol_type lqvm::scanner::get_next_token()

#include "graph_parser.h"

namespace lqvm {
class scanner : public yyFlexLexer {
public:
  scanner() {}
  parser::symbol_type get_next_token();
};
} // namespace lqvm