#ifndef COMPILER_INCLUDE_VALUE_H
#define COMPILER_INCLUDE_VALUE_H

#include <string>

enum ValType {
    _NUM, _ID, _ARR
};

class Value {
    std::string name;
    ValType type;
    int ID = -1;  // aka memory cell
    int size = 0;
    bool if_param = false;
    bool initialized = false;

public:
    Value(int id, ValType type, const std::string& name) : ID(id), type(type), name(name) {}

    Value(int id, ValType type, const std::string& name, bool param) : ID(id), type(type), name(name), if_param(param) {}

    Value(int id, const std::string& name, int size) : ID(id), name(name), size(size) {
        type = ValType::_ARR;
    }

    Value(ValType type, const std::string& name) : type(type), name(name) {}  // for procedures params

    std::string get_vals_to_logger() {
        std::string data = "ID=" + std::to_string(ID) + ", name = " + name + ", if_param=" + std::to_string(if_param);
        if (type == ValType::_ARR) {
            data += ", ARR size=" + std::to_string(size) + ", end_ID=" + std::to_string(ID+size-1);
        }
        return data;
    }

    int get_id() {
        return ID;
    }

    std::string get_name() {
        return name;
    }

    ValType get_type() {
        return type;
    }

    void initialize() {
        initialized = true;
    }

    bool is_initialized() {
        return initialized;
    }
};

enum ExprOperatorType {
    _PLUS, _MIN, _MUL, _DIV, _MOD, _NOOP
};

class Expression {
    ExprOperatorType op;
    std::string val1;
    std::string val2;
    std::string val1_idx;
    std::string val2_idx;

public:
    Expression(ExprOperatorType op, std::string val1, std::string val2, std::string idx1, std::string idx2)
        : op(op), val1(val1), val2(val2), val1_idx(idx1), val2_idx(idx2) {}

    std::string get_vals_to_logger() {
        return "Expression: val1 = " + val1 + ", idx1 = " + val1_idx + ", val2 = " + val2 + ", idx2 = " + val2_idx;
    }

    std::string get_val1() {
        return val1;
    }

    std::string get_val2() {
        return val2;
    }

    std::string get_val1_idx() {
        return val1_idx;
    }

    std::string get_val2_idx() {
        return val2_idx;
    }

    ExprOperatorType get_operator() {
        return op;
    }
};

#endif  // COMPILER_INCLUDE_VALUE_H
