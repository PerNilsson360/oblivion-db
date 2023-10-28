#include <sstream>
#include <stdexcept>
#include <iostream>
#include "Json.hh"
#include "Schema.hh"

namespace {
    constexpr const char* TYPE = "type";
    constexpr const char* NULL_STR = "null";
    constexpr const char* BOOLEAN = "boolean";
    constexpr const char* NUMBER = "number";
    constexpr const char* STRING = "string";
    constexpr const char* OBJECT = "object";
    constexpr const char* ARRAY = "array";
    constexpr const char* PROPERTIES = "properties";
    constexpr const char* ITEMS = "items";
}

bool
isStringEqual(const Value& val, const std::string& s) {
    return val.getType() == Value::String && val.getString() == s;
}

void
ensureType(const Value& val, Value::Type type) {
    if (val.getType() != type) {
        std::stringstream ss;
        ss << "Json::Schema::ensureType value should be of type: " << type;
        throw std::runtime_error(ss.str());
    }
}

const SchemaNode* parse(const Value& val) {
    if (val.getType() != Value::Object) {
        throw std::runtime_error("Json::Schema::parse top level must be an object");
    }
    const SchemaNode* result = nullptr;
    const Value& type = val[TYPE];
    if (!isStringEqual(type, OBJECT)) {
        throw std::runtime_error("Json::Schema::parse top level type must be object");
    }
    return new Object(val);
}


// SchemaNode
SchemaNode*
SchemaNode::parse(const Value& val) {
    enum Value::Type t = val.getType();
    std::cerr << "SchemaNode::parse " << val << " type: " << t << std::endl;
    if (t != Value::Object) {
        std::stringstream ss;
        ss << "SchemaNode::parse value must be an object type: " << t; 
        throw std::runtime_error(ss.str());
    }
    const Value& type = val[TYPE];
    if (isStringEqual(type, NULL_STR)) {
        return new Null(val);
    } else if (isStringEqual(type, BOOLEAN)) {
        return new Boolean(val);
    } else if (isStringEqual(type, NUMBER)) {
        return new Number(val);
    } else if (isStringEqual(type, STRING)) {
        return new String(val);
    } else if (isStringEqual(type, OBJECT)) {
        return new Object(val);
    } else if (isStringEqual(type, ARRAY)) {
        return new Array(val);
    } else {
        std::stringstream ss;
        ss << "SchemaNode::parse value unkown type: " << type; 
        throw std::runtime_error(ss.str());
    }
}

// Null
Null::Null(const Value& val) {}

bool
Null::validate(const Value& val) const {
    std::cerr << "Null::validate: "<< val << " type: "<< val.getType() << std::endl;
    return val.getType() == Value::Null;
}

// Boolean
Boolean::Boolean(const Value& val) {}

bool
Boolean::validate(const Value& val) const {
    return val.getType() == Value::Boolean;
}

// Number
Number::Number(const Value& val) {}

bool
Number::validate(const Value& val) const {
    return val.getType() == Value::Number;
}

// String
String::String(const Value& val) {}

bool
String::validate(const Value& val) const {
    return val.getType() == Value::String;
}

// Object
Object::Object(const Value& val) {
    const Value& props = val[PROPERTIES];
    ensureType(props, Value::Object);
    for (const auto& prop : props.getObject()) {
        _props[prop.first] = SchemaNode::parse(*prop.second);
    }
}

bool
Object::validate(const Value& val) const {
    bool result(true);
    std::cerr << "Object::validate: " << val << std::endl;    
    for (const auto& prop : val.getObject()) {
        std::map<std::string, SchemaNode*>::const_iterator i = _props.find(prop.first);
        if (i != _props.end()) {
            result &= i->second->validate(*prop.second);
            std::cerr << "result:"  << result << " name: " << i->first << std::endl;

        }
    }
    return result;
}

// Array
Array::Array(const Value& val) {
    const Value& items = val[ITEMS];
    ensureType(items, Value::Object);
    const Value& type = items[TYPE];
    _items = SchemaNode::parse(type);
}

bool
Array::validate(const Value& val) const {
    bool result(true);
    for (const Value* v : val.getArray()) {
        result &= _items->validate(*v);
    }
    return result;
}

