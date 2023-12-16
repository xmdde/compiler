#ifndef COMPILER_INCLUDE_VALUE_H
#define COMPILER_INCLUDE_VALUE_H

#include <string>

enum ValType {
    _NUM, _ID, _ARR
};

class Value {
    std::string name;
    ValType type;
    int ID;
    int size = 0;

public:
    Value(int id, ValType type, std::string name) : ID(id), type(type), name(name) {}
    Value(int id, std::string name, int size) : ID(id), name(name), size(size) {
        type = ValType::_ARR;
    }
};

enum ExprOperatorType {
    _PLUS, _MIN, _MUL, _DIV, _MOD
};

class Expression {
    ExprOperatorType op;
    std::string val1;
    std::string val2 = "";
    std::string val1_idx = "";
    std::string val2_idx = "";

public:
    Expression(ExprOperatorType op, std::string val1) : op(op), val1(val1) {}
    Expression(ExprOperatorType op, std::string val1, std::string val2) : op(op), val1(val1), val2(val2) {}
    Expression(ExprOperatorType op, std::string val1, std::string val2, std::string idx1, std::string idx2)
        : op(op), val1(val1), val2(val2), val1_idx(idx1), val2_idx(idx2) {}
};

#endif  // COMPILER_INCLUDE_VALUE_H
