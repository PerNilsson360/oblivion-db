#ifndef JSONDRIVER_HH
#define JSONDRIVER_HH
#include <string>
#include <memory>
#include "Json.hh"
#include "JsonParser.hh"
// Tell Flex the lexer's prototype ...
# define YY_DECL                                            \
    yy::JsonParser::symbol_type yylex (JsonDriver& driver)
// ... and declare it for the parser's sake.
YY_DECL;
// Conducting the whole scanning and parsing of Calc++.
class JsonDriver
{
public:
    JsonDriver ();
    virtual ~JsonDriver ();

    // Handling the scanner.
    void scan_begin ();
    void scan_end ();
    // Run the parser on file F.
    // Return 0 on success.
    int parse(const std::string& json);
    // Used later to pass the file name to the location tracker.
    std::unique_ptr<Value> result;
    std::string json;
    // Whether parser traces should be generated.
    bool trace_parsing;
    bool trace_scanning;
    // Error handling.
    void error (const yy::location& l, const std::string& m);
    void error (const std::string& m);
};
#endif // ! JSONDRIVER_HH
