%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define parser_class_name {JsonParser}
%define api.token.constructor
%define api.value.type variant
%define parse.assert
%code requires
{
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>    
#include "Json.hh"
class JsonDriver;
inline std::string stripLiteral(const std::string& s) {
	size_t len = s.size();
	return len == 2 ?  ""  : s.substr(1, len - 2);
}
}
// The parsing context.
%param { JsonDriver& driver }
%locations
%initial-action
{
  // Initialize the initial location.
  // @$.begin.filename = @$.end.filename = &driver.file;
};
%define parse.error verbose
%code
{
# include "JsonDriver.hh"
}
%define api.token.prefix {TOK_}
%token
  END  0       "end of file"
  LSQUARE      "["
  RSQUARE      "]"
  LCURL        "{."
  RCURL        "}"
  COMMA        ","
  COLON        ":"
  FALSE        "false"
  NULL         "null"
  TRUE         "true"
;
%token <std::string> STRING "string"
%token <double> NUMBER "number"

%type  <Value*> Value
%type  <Value*> Object
%type  <Value*> Array
%type  <Value*> Members
%type  <Value*> Values

%printer { yyoutput << $$; } <*>;
%%
%start Text;

// JSON-text = ws value ws
Text:
  Value                                         { driver.result.reset($1); }

// value = false / null / true / object / array / number / string
Value :
  FALSE                                         { $$ = new Value(false); }
| NULL                                          { $$ = new Value(); }
| TRUE                                          { $$ = new Value(true); }
| Object                                        { $$ = $1; }
| Array                                         { $$ = $1; }
| "number"                                      { $$ = new Value($1); }
| "string"                                      { $$ = new Value($1); }

// object = begin-object [ member *( value-separator member ) ] end-object
// member = string name-separator value      
Object :
  LCURL RCURL                                   { $$ = new Value(Value::Object); }
| LCURL Members RCURL                           { $$ = $2; }


Members :
  "string" COLON Value                          { $$ = new Value(Value::Object); $$->insert($1, $3); }
| "string" COLON Value COMMA Members            { $$ = $5; $$->insert($1, $3); }

// array = begin-array [ value *( value-separator value ) ] end-array
Array :
  LSQUARE RSQUARE                               { $$ = new Value(Value::Array); }
| LSQUARE Values RSQUARE                        { $$ = $2; }

Values :      
  Value                                         { $$ = new Value(Value::Array); $$->insert($1); }
| Values COMMA Value                            { $$ = $1; $$->insert($3); }
      
%%
void
yy::JsonParser::error (const location_type& l, const std::string& m)
{
  driver.error (l, m);
}
