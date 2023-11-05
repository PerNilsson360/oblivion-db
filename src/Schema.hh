#ifndef _SCHEMA_HH_
#define _SCHEMA_HH_

#include <optional>
#include "Json.hh"

class SchemaNode {
public:
    static SchemaNode* parse(const Value& val);
    virtual bool validate(const Value& val) const = 0;
private:
};

class Atom : public SchemaNode {
};

class Null : public Atom {
public:
    Null(const Value& val);
    bool validate(const Value& val) const override;
private:
};

class Boolean : public Atom {
public:
    Boolean(const Value& val);
    bool validate(const Value& val) const override;
private:
};

class Number : public Atom {
public:
    Number(const Value& val);
    bool validate(const Value& val) const override;    
private:
    std::optional<double> _minimum;
    std::optional<bool> _exclusiveMinimum;
    std::optional<double> _maximum;
    std::optional<bool> _exclusiveMaximum;
    std::optional<double> _mulitpleOf;
};

class String : public Atom {
public:
    String(const Value& val);
    bool validate(const Value& val) const override;
private:
};

class Object : public SchemaNode {
public:
    Object(const Value& val);
    bool validate(const Value& val) const override;
private:
    std::map<std::string, SchemaNode*> _props;
};

class Array : public SchemaNode {
public:
    Array(const Value& val);
    bool validate(const Value& val) const override;
private:
    SchemaNode* _items;
};

const SchemaNode* parse(const Value& value);

#endif
