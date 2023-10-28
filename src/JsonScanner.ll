%{ /* -*- C++ -*- */
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <sstream>
#include "JsonDriver.hh"
#include "JsonParser.hh"

// Work around an incompatibility in flex (at least versions
// 2.5.31 through 2.5.33): it generates code that does
// not conform to C89.  See Debian bug 333231
// <http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=333231>.

# undef yywrap
# define yywrap() 1

// The location of the current token.
static yy::location loc;
// ro debug add debug to %option below
%}
%option noyywrap nounput batch noinput

digit [0-9]+
blank [ \t]

%{
  // Code run each time a pattern is matched.
  # define YY_USER_ACTION  loc.columns (yyleng);
%}

%%

%{
  // Code run each time yylex is called.
  loc.step ();
%}

{blank}+   loc.step ();
[\n]+      loc.lines (yyleng); loc.step ();
"["      return yy::JsonParser::make_LSQUARE(loc);
"]"      return yy::JsonParser::make_RSQUARE(loc);
"{"      return yy::JsonParser::make_LCURL(loc);
"}"      return yy::JsonParser::make_RCURL(loc);
","      return yy::JsonParser::make_COMMA(loc);
":"      return yy::JsonParser::make_COLON(loc);
"false"  return yy::JsonParser::make_FALSE(loc);
"null"   return yy::JsonParser::make_NULL(loc);
"true"   return yy::JsonParser::make_TRUE(loc);

([0-9]+("."[0-9]+)?)|"."[0-9]+ {
  errno = 0;
  double d = strtod(yytext, nullptr);
  if (errno == ERANGE) {
    driver.error(loc, "double is out of range");
  }
  return yy::JsonParser::make_NUMBER(d, loc);
}

\"[^"]*\" {
    yytext[yyleng - 1] = '\0';
    return yy::JsonParser::make_STRING(yytext + 1, loc);
}
 . {
	std::string message("invalid character: [");
	message += yytext[0];
	message += "]";
	driver.error (loc, message);
 }
<<EOF>>                  return yy::JsonParser::make_END(loc);
%%

void
JsonDriver::scan_begin ()
{
    //yy_flex_debug = trace_scanning;
    if (json.empty()) {
        yyin = stdin;
    } else  if (!(yyin = fopen(json.c_str (), "r"))) {
        std::cerr << "parsing from string" << std::endl;
        yyin = fmemopen((void*)json.c_str(), json.length(), "r");
        if (yyin == nullptr) {
            std::stringstream ss;
            ss << "can not fmemopen " << json << " : " << strerror(errno);
            throw std::runtime_error(ss.str());
        }
    }
}



void
JsonDriver::scan_end ()
{
    fclose(yyin);
}

