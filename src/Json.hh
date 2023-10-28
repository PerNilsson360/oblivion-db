#ifndef _JSON_HH_
#define _JSON_HH_

#include <string>
#include <vector>
#include <map>
#include <ostream>


class Value {
public:
    typedef std::map<std::string, Value*> ObjectImpl;
    typedef std::vector<Value*> ArrayImpl;
    enum Type {
        Null,
        Boolean,
        Number,
        String,
        Object,
        Array,
    };
    Value();
    explicit Value(enum Type t);
    explicit Value(bool b);
    explicit Value(double d);
    explicit Value(const std::string& s);
    Value(const Value& val);
    Type getType() const;
    bool getBoolean() const;
    double getNumber() const;
    const std::string& getString() const;
    const ObjectImpl& getObject() const;
    const ArrayImpl& getArray() const;
    size_t getSize();
    Value& operator[](size_t i);
    const Value& operator[](size_t i) const;
    Value& operator[](const std::string& key);
    const Value& operator[](const std::string& key) const;
    /**
     * Memory is owned by this container.
     */
    void insert(const std::string& key, Value* value);
    void insert(Value* value);
    Value* release(const std::string& key);
    Value* release(size_t i);
    //Value(Value&& val);
    Value& operator=(const Value& val);
    //Value& operator=(Value&& val);
    ~Value();
private:
    void setParent(Value* val);
    void ensureType(Type type) const;
    void clear();
    void assign(const Value& val);
    Type _type;
    Value* _parent;
    union {
        bool b;
        double d;
        std::string* s;
        ObjectImpl* props;
        ArrayImpl* vals;
    } _data;
};

inline
std::ostream&
operator<<(std::ostream& os, const Value& val) {
    switch (val.getType()) {
    case Value::Null:
        os << "null";
        break;
    case Value::Boolean:
        os << (val.getBoolean() ? "true" : "false");
        break;
    case Value::Number:
        os << val.getNumber();;
        break;
    case Value::String:
        os << "\"" << val.getString() << "\"";
        break;
    case Value::Object: {
        os << "{";
        bool first(true);
        for (const auto& prop : val.getObject()) {
            if (!first) {
                os << ",";
            }
            os << "\"" << prop.first << "\":" << *prop.second;
            first = false;
        }
        os << "}";
    }
        break;
    case Value::Array: {
        os << "[";
        bool first(true);
        for (const Value* val : val.getArray()) {
            if (!first) {
                os << ",";
            }
            os << *val;
            first = false;
        }
        os << "]";
    }
        break;
    default:
        break;
    }
    return os;
}


inline
std::ostream&
operator<<(std::ostream& os, const Value::ArrayImpl* vals) {
    return os;
}

inline
std::ostream&
operator<<(std::ostream& os, const std::pair<std::string, Value*>& p) {
    return os;
}

inline
std::ostream&
operator<<(std::ostream& os, const Value::ObjectImpl* keyVals) {
    return os;
}

namespace Json {
    Value* parse(const std::string& json);
}

#endif
