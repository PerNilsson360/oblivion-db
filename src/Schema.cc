#include <cmath>
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
    constexpr const char* MINIMUM = "minimum";
    constexpr const char* MAXIMUM = "maximum";
    constexpr const char* EXCLUSIVE_MINIMUM = "exclusiveMinimum";
    constexpr const char* EXCLUSIVE_MAXIMUM = "exclusiveMaximum";
    constexpr const char* MULTIPLEOF = "multipleOf";
    constexpr const char* STRING = "string";
    constexpr const char* OBJECT = "object";
    constexpr const char* PROPERTIES = "properties";
    constexpr const char* ARRAY = "array";
    constexpr const char* ITEMS = "items";
}

bool
isStringEqual(const Value& val, const std::string& s) {
    return val.getType() == Value::String && val.getString() == s;
}

double
getNumber(const Value& val) {
    if (val.getType() != Value::Number) {
        std::stringstream ss;
        ss << "Schema::getNumber(): value is not a number path: " << val.getPath(); 
        throw std::runtime_error(ss.str());
    }
    return val.getNumber();
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
    return SchemaNode::parse(val);
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
    if (!val.contains(TYPE)) {
        throw std::runtime_error("Json::Schema::parse missing type property");
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
Number::Number(const Value& val) {
    // TODO check valid key set
    if (val.contains(MINIMUM)) {
        _minimum = getNumber(val[MINIMUM]);
    }
    if (val.contains(EXCLUSIVE_MINIMUM)) {
        _exclusiveMinimum = getNumber(val[EXCLUSIVE_MINIMUM]);
    }
    if (_minimum.has_value() && _exclusiveMinimum.has_value()) {
        throw std::runtime_error("Number::Number both minimum and exclusiveMinimum is set");
    }
    if (val.contains(MAXIMUM)) {
        _maximum = getNumber(val[MAXIMUM]);
    }
    if (val.contains(EXCLUSIVE_MAXIMUM)) {
        _exclusiveMaximum = getNumber(val[EXCLUSIVE_MAXIMUM]);
    }
    if (_maximum.has_value() && _exclusiveMaximum.has_value()) {
        throw std::runtime_error("Number::Number both maximum and exclusiveMaxmum is set");
    }
    if (val.contains(MULTIPLEOF)) {
        _mulitpleOf = getNumber(val[MULTIPLEOF]);
    }
}

bool
Number::validate(const Value& val) const {
    if (val.getType() != Value::Number) {
        return false;
    }
    double n = getNumber(val);
    if (_minimum.has_value() && n < _minimum.value()) {
        return false;
    }
    if (_exclusiveMinimum.has_value() && n <= _exclusiveMinimum.value()) {
        return false;
    }
    if (_maximum.has_value() && n > _maximum.value()) {
        return false;
    }
    if (_exclusiveMaximum.has_value() && n >= _exclusiveMaximum.value()) {
        return false;
    }
    if (_mulitpleOf.has_value()) {
        double m = std::fmod(n, _mulitpleOf.value());
        // TODO FLT_EPSILON comparison
        return m == 0;
    }
    return true;
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

        } else {                // not present in model
            result = false;
        }
    }
    return result;
}

// Array
Array::Array(const Value& val) : _items(nullptr) {
    if (val.contains(ITEMS)) {
        const Value& items = val[ITEMS];
        ensureType(items, Value::Object);
        if (!items.contains(TYPE)) {
            throw std::runtime_error("Array::Array missing type in items");
        }
        _items = SchemaNode::parse(items);
    }
}

bool
Array::validate(const Value& val) const {
    bool result(true);
    if (_items != nullptr) {
        if (val.getType() == Value::Array) {
            for (const Value* v : val.getArray()) {
                result &= _items->validate(*v);
            }
        } else {
            result = false;
        }
    }
    return result;
}

