#include <memory>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string.h>

#include "Json.hh"

void
testNullProperty() {
    std::unique_ptr<Value> o(Json::parse(R"({"a":null})"));
    const Value& obj = *(o.get());
    const Value& a = obj["a"];
    assert(a.getType() == Value::Null);
}

int 
main(int argc, char **argv)
{
    // std::unique_ptr<Value> r(Json::parse(R"({"a":{"b":["foo","bar"]}})"));
    // std::cout << "##:" << *r << std::endl;
    // r.reset(Json::parse(R"({"a":[1,2,3]})"));

    // std::cout << "##:" << *r << std::endl;
    // r.reset(Json::parse(R"({"a":false,"b":null,"c": true})"));
    // std::cout << "##:" << *r << std::endl;
    // r.reset(Json::parse(R"({"a":false,"b":null,"c": true, "c": 1.5, "d" :[1,2,3,4], "e":{}})"));
    // std::cout << "##:" << *r << std::endl;
    // std::unique_ptr<Value> r(Json::parse("/home/per/prg/xslt/data.json"));
    // std::cout << "##:" << *r << std::endl;g
    // std::ifstream ifs("/home/per/prg/xslt/data.json");
    // if (!ifs.good()) {
    //     std::cerr << "jstr: could not open file." << std::endl;
    //     return -1;
    // }
    // nlohmann::json s = nlohmann::json::parse(ifs);
    //std::cout << "##:" << *r << std::endl;
    testNullProperty();
    return 0;
}
