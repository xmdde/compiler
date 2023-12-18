#ifndef COMPILER_INCLUDE_CODEBLOCK_H
#define COMPILER_INCLUDE_CODEBLOCK_H

#include <string>
#include "Value.h"

class CodeBlock {
public:
    int next_TRUE;
    int next_FALSE;
    int ID;

    CodeBlock(int id) : ID(id) {}

    virtual void translate_to_asm() = 0;
    virtual std::string get_vals_to_logger() = 0;
};

enum CondOperatorType {
    _EQ, _NEQ, _LLESS, _LMORE, _LLEQ, _LMEQ
};

class CondBlock : public CodeBlock {
    CondOperatorType op;
    std::string val1;
    std::string val2;
    std::string val1_idx = "";
    std::string val2_idx = "";
    // TODO(): check if idx num (decide whether its even important)
public:
    CondBlock(int id, CondOperatorType type, std::string val1, std::string val2, std::string val1_idx, std::string val2_idx)
        : CodeBlock(id), op(type), val1(val1), val2(val2), val1_idx(val1_idx), val2_idx(val2_idx) {}

    std::string get_vals_to_logger() override {
        return "val1 = " + val1 + ", idx1 = " + val1_idx + ", val2 = " + val2 + ", idx2 = " + val2_idx;
    }

    void translate_to_asm() override {
    }
};

class AssignBlock : public CodeBlock {
    std::string val;
    std::string val_idx = "";  // TODO(): check if idx num
    Expression expr;

public:
    AssignBlock(int id, std::string val, Expression expression) : CodeBlock(id), val(val), expr(expression) {}
    AssignBlock(int id, std::string val, std::string idx, Expression expression) : CodeBlock(id), val(val), val_idx(idx), expr(expression) {}

    std::string get_vals_to_logger() override {
        return "val = " + val + ", idx = " + val_idx + ", " + expr.get_vals_to_logger();
    }

    void translate_to_asm() override {
    }
};

class ProcedureCall : public CodeBlock {
    std::string procedure_name;
    std::vector<std::string> params; //check default empty proc

public:
    ProcedureCall(int id, std::string name, std::vector<std::string> params) : CodeBlock(id), procedure_name(name), params(params) {}

    std::string get_vals_to_logger() override {
        return "";
    }

    void translate_to_asm() override {
    }
};

enum Keyword {
    _WRITE, _READ, _PROC_END, _EMPTY
};

class KeywordBlock : public CodeBlock {
    Keyword type;
    std::string val = "";
    std::string val_idx = "";

public:
    KeywordBlock(int id, Keyword type) : CodeBlock(id), type(type) {}
    KeywordBlock(int id, Keyword type, const std::string& val, const std::string& idx) : CodeBlock(id), type(type), val(val), val_idx(idx) {}

    std::string get_vals_to_logger() override {
        std::string log_info;
        switch (type) {
            case Keyword::_WRITE:
                log_info = "_WRITE, val = " + val + ", idx = " + val_idx;
            break;
            case Keyword::_READ:
                log_info = "_READ, val = " + val + ", idx = " + val_idx;
            break;
            case Keyword::_PROC_END:
                log_info = "_PROC_END";
            break;
            case Keyword::_EMPTY:
                log_info = "_EMPTY";
            break;
        }
        return log_info;
    }

    void translate_to_asm() override {
    }
};

#endif  // COMPILER_INCLUDE_CODEBLOCK_H
