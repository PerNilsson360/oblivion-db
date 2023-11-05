#include <sstream>
#include <stdexcept>

#include "JsonDriver.hh"
#include "Json.hh"

namespace Json {
Value* parse(const std::string& json) {
    JsonDriver driver;
    int rc = driver.parse(json);
    if (rc != 0) {
        throw std::runtime_error("Json::parse: parse error");
    }
    return driver.result.release();
}
    
}

Value::Value() : _type(Null), _parent(nullptr) {
    _data.props = nullptr;

}

Value::Value(enum Type t) : _type(t), _parent(nullptr) {
    switch (_type) {
    case Null:
        _data.props = nullptr;
        break;
    case Boolean:
        _data.b = true;
        break;
    case Number:
        _data.d = 0;
        break;
    case String:
        _data.s = new std::string();
        break;
    case Object:
        _data.props = new ObjectImpl();
        break;
    case Array:
        _data.vals = new ArrayImpl();
        break;
    default:
        break;
    }
}

Value::Value(bool b) : _type(Boolean), _parent(nullptr) {
    _data.b = b;
}

Value::Value(double d) : _type(Number), _parent(nullptr) {
    _data.d = d;
}

Value::Value(const std::string& s) : _type(String), _parent(nullptr) {
    _data.s = new std::string(s);
}

Value::Value(const Value& val) {
    clear();
    assign(val);
}

Value::Type
Value::getType() const {
    return _type;
}

bool
Value::getBoolean() const {
    ensureType(Boolean);
    return _data.b;
}

double
Value::getNumber() const {
    ensureType(Number);
    return _data.d;
}

const std::string&
Value::getString() const {
    ensureType(String);
    return *_data.s;
}

const Value::ObjectImpl&
Value::getObject() const {
    ensureType(Object);
    return *_data.props;
}

const Value::ArrayImpl&
Value::getArray() const {
    ensureType(Array);
    return *_data.vals;
}

size_t
Value::getSize() {
    ensureType(Array);
    return _data.vals->size();
}

Value&
Value::operator[](size_t i) {
    ensureType(Array);
    return *_data.vals->at(i);
}


const Value&
Value::operator[](size_t i) const {
    ensureType(Array);
    return *_data.vals->at(i);

}

bool Value::contains(const std::string& key) const {
    if (_type != Object) {
        return false;
    }
    ObjectImpl::const_iterator i = _data.props->find(key);
    return i != _data.props->end();
}

Value&
Value::operator[](const std::string& key) {
    ensureType(Object);
    return *(*_data.props)[key];
}

const Value&
Value::operator[](const std::string& key) const {
    ensureType(Object);
    return *(*_data.props)[key];
}

std::string
Value::getPath() const {
    return "Value::getPath() missing impl";
}

void
Value::insert(const std::string& key, Value* value) {
    ensureType(Object);
    ObjectImpl::iterator i = _data.props->find(key);
    if (i != _data.props->end()) {
        delete i->second;
    }
    _data.props->erase(key);
    value->setParent(this);
    _data.props->insert(std::make_pair<>(key, value));
}

void
Value::insert(Value* value) {
    ensureType(Array);
    value->setParent(this);
    _data.vals->push_back(value);
}

Value*
Value::release(const std::string& key) {
    ObjectImpl::iterator i = _data.props->find(key);
    if (i == _data.props->end()) {
        std::stringstream ss;
        ss << "Value::release can nod find key: " << key;
        throw std::runtime_error(ss.str());
    }
    Value* result = i->second;
    _data.props->erase(i);
    return result;
}

Value*
Value::release(size_t i) {
    Value* result = _data.vals->at(i);
    _data.vals->erase(_data.vals->begin() + i);
    return result;
}

//Value::Value(Value&& value) {
//}

Value&
Value::operator=(const Value& val) {
    clear();
    assign(val);
    return *this;
}

//Value& Value::operator=(Value&& xd) {
//    return *this;
//}

Value::~Value() {
    clear();
}

void
Value::setParent(Value* val) {
    _parent = val;
}

void
Value::ensureType(Type type) const { 
    if (_type != type) {
        std::stringstream ss;
        ss << "Value::ensureType type missmatch expected: " << type << " actual: " << _type;
        throw std::runtime_error(ss.str());
    }
}

void
Value::clear() {
    switch (_type) {
    case String:
        delete _data.s;
        _data.s = nullptr;
        break;
    case Object:
        for (auto& item : *_data.props) {
            delete item.second;
        }
        delete _data.props;
        _data.props = nullptr;
        break;
    case Array:
        for (Value* val : *_data.vals) {
            delete val;
        }
        delete _data.vals;
        _data.props = nullptr;
        break;
    default:
        break;
    }
}

void
Value::assign(const Value& val) {
    clear();
    _type = val._type;
    switch (_type) {
    case Null:
        _data.props = nullptr;
        break;
    case Boolean:
        _data.b = val._data.b;
        break;
    case Number:
        _data.d = val._data.d;
        break;
    case String:
        _data.s = new std::string(*val._data.s);
        break;
    case Object:
        _data.props = new ObjectImpl(*val._data.props);
        break;
    case Array:
        _data.vals = new ArrayImpl(*val._data.vals);
        break;
    default:
        break;
    }
}
