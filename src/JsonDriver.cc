#include "JsonDriver.hh"
#include "JsonParser.hh"

JsonDriver::JsonDriver() :
  trace_scanning(false), trace_parsing(false) {
}

JsonDriver::~JsonDriver() {}

int
JsonDriver::parse(const std::string& s) {
  json = s;
  scan_begin ();
  yy::JsonParser parser(*this);
  parser.set_debug_level(trace_parsing);
  int res = parser.parse();
  scan_end();
  return res;
}

void
JsonDriver::error (const yy::location& l, const std::string& m) {
  std::cerr << l << ": " << m << std::endl;
}

void
JsonDriver::error (const std::string& m) {
  std::cerr << m << std::endl;
}
