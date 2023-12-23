#ifndef COMPILER_INCLUDE_VALUE_H
#define COMPILER_INCLUDE_VALUE_H

#include <string>

enum ValType {
    _NUM, _ID, _ARR
};

class Value {
    std::string name;
    ValType type;
    int ID = -1;
    int size = 0;

public:
    Value(int id, ValType type, const std::string& name) : ID(id), type(type), name(name) {}
    Value(int id, const std::string& name, int size) : ID(id), name(name), size(size) {
        type = ValType::_ARR;
    }
    Value(ValType type, const std::string& name) : type(type), name(name) {} //for procedures params

    std::string get_vals_to_logger() {
        std::string data = "ID=" + std::to_string(ID) + ", name = " + name;
        if (type == ValType::_ARR) {
            data += ", ARR size =" + std::to_string(size) + ", end_ID=" + std::to_string(ID+size-1);
        }
        return data;
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
    Expression(ExprOperatorType op, std::string val1, std::string val2, std::string idx1, std::string idx2)
        : op(op), val1(val1), val2(val2), val1_idx(idx1), val2_idx(idx2) {}

    std::string get_vals_to_logger() {
        return "Expression: val1 = " + val1 + ", idx1 = " + val1_idx + ", val2 = " + val2 + ", idx2 = " + val2_idx;
    }
};

#endif  // COMPILER_INCLUDE_VALUE_H
