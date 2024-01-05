#ifndef COMPILER_INCLUDE_CODEBLOCK_H
#define COMPILER_INCLUDE_CODEBLOCK_H

#include <string>
#include <memory>

#include "AsmCode.h"
#include "Value.h"

class CodeBlock {
public:
    int ID;
    int next_TRUE = -1;
    int next_FALSE = -1;
    AsmCode* asm_code;

    int first_instruction_k = -1;
    int procedure_num = -1;

    CodeBlock(int id, AsmCode* code) : ID(id) {
        asm_code = code;
    }

    virtual void translate_to_asm() = 0;
    virtual std::string get_vals_to_logger() = 0;

    virtual std::string get_nbhs_to_logger() {
        return "In Block ID=" + std::to_string(ID) + " | next_TRUE ID = " + std::to_string(next_TRUE) + ", next_FALSE ID = " + std::to_string(next_FALSE);
    }

    virtual void set_next_true(int id) {
        next_TRUE = id;
    }

    virtual void set_next_false(int id) {
        next_FALSE = id;
    }
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
    // TODO(): check if idx num (decide if it's even important)
public:
    CondBlock(int id, CondOperatorType type, std::string val1, std::string val2, std::string val1_idx, std::string val2_idx, AsmCode* code)
        : CodeBlock(id, code), op(type), val1(val1), val2(val2), val1_idx(val1_idx), val2_idx(val2_idx) {}

    std::string get_vals_to_logger() override {
        return "ID=" + std::to_string(ID) + ", val1 = " + val1 + ", idx1 = " + val1_idx + ", val2 = " + val2 + ", idx2 = " + val2_idx;
    }

    void translate_to_asm() override {
    }
};

class AssignBlock : public CodeBlock {
    std::string val;
    std::string val_idx = "";  // TODO(): check if idx num
    Expression expr;

public:
    AssignBlock(int id, std::string val, Expression expression, AsmCode* code) : CodeBlock(id, code), val(val), expr(expression) {}
    AssignBlock(int id, std::string val, std::string idx, Expression expression, AsmCode* code) : CodeBlock(id, code), val(val), val_idx(idx), expr(expression) {}

    std::string get_vals_to_logger() override {
        return "ID=" + std::to_string(ID) + ", val = " + val + ", idx = " + val_idx + ", " + expr.get_vals_to_logger();
    }

    void translate_to_asm() override {
    }
};

class ProcedureCall : public CodeBlock {
    std::string procedure_name;
    std::vector<std::string> params; //check default empty proc

public:
    ProcedureCall(int id, std::string name, std::vector<std::string> params, AsmCode* code) : CodeBlock(id, code), procedure_name(name), params(params) {}

    std::string get_vals_to_logger() override {
        std::string v = "ID=" + std::to_string(ID) + ", name = " + procedure_name + ", params: ";
        for (auto p : params)
            v += p + ", ";
        return v;
    }

    void translate_to_asm() override {
    }
};

enum Keyword {
    _WRITE, _READ, _EMPTY
};

class KeywordBlock : public CodeBlock {
    Keyword type;
    std::string val = "";
    std::string val_idx = "";

public:
    KeywordBlock(int id, Keyword type, AsmCode* code) : CodeBlock(id, code), type(type) {}
    KeywordBlock(int id, Keyword type, const std::string& val, const std::string& idx, AsmCode* code) : CodeBlock(id, code), type(type), val(val), val_idx(idx) {}

    std::string get_vals_to_logger() override {
        std::string log_info = "ID=" + std::to_string(ID);
        switch (type) {
            case Keyword::_WRITE:
                log_info += ", _WRITE, val = " + val + ", idx = " + val_idx;
            break;
            case Keyword::_READ:
                log_info += ", _READ, val = " + val + ", idx = " + val_idx;
            break;
            case Keyword::_EMPTY:
                log_info += ", _EMPTY";
            break;
        }
        return log_info;
    }

    void translate_to_asm() override {
    }
};

#endif  // COMPILER_INCLUDE_CODEBLOCK_H
