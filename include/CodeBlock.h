#ifndef COMPILER_INCLUDE_CODEBLOCK_H
#define COMPILER_INCLUDE_CODEBLOCK_H

#include <string>
#include <memory>

#include "AsmCode.h"
#include "Value.h"

enum Keyword {
    _WRITE, _READ, _EMPTY
};

enum CondOperatorType {
    _EQ, _NEQ, _LLESS, _LMORE, _LLEQ, _LMEQ
};

class CodeBlock {
public:
    int ID;
    int next_TRUE = -1;
    int next_FALSE = -1;
    AsmCode* asm_code;
    int first_instruction_k = -1;
    int procedure_num = -1;
    int depth;
    std::string block_type;
    bool repeat_begin = false;
    bool if_else_end = false;

    CodeBlock(int id, AsmCode* code) : ID(id) {
        asm_code = code;
    }

    virtual std::string get_vals_to_logger() = 0;

    virtual std::string get_nbhs_to_logger() {
        return "In Block ID=" + std::to_string(ID) + " | next_TRUE ID = " + std::to_string(next_TRUE) +
               ", next_FALSE ID = " + std::to_string(next_FALSE);
    }

    virtual void set_next_true(int id) {
        next_TRUE = id;
    }

    virtual void set_next_false(int id) {
        next_FALSE = id;
    }

    virtual void set_depth(const int loop_depth) {
        depth = loop_depth;
    }

    virtual int get_depth() {
        return depth;
    }

    virtual std::string get_val() {
        return "";
    }

    virtual std::string get_val_idx() {
        return "";
    }

    virtual std::string get_val2() {
        return "";
    }

    virtual std::string get_val2_idx() {
        return "";
    }

    virtual Keyword get_keyword() {
        return Keyword::_EMPTY;
    }

    virtual CondOperatorType get_cond() {
        return CondOperatorType::_EQ;
    }

    virtual Expression* get_expression() {
        return nullptr;
    }

    virtual std::string get_proc_name() {
        return "";
    }

    virtual std::vector<std::string>* get_params() {
        return nullptr;
    }

    virtual std::string get_empty_type() {
        return "";
    }
};

class CondBlock : public CodeBlock {
    CondOperatorType op;
    std::string val1;
    std::string val2;
    std::string val1_idx = "";
    std::string val2_idx = "";

public:
    CondBlock(int id, CondOperatorType type, std::string val1, std::string val2, std::string val1_idx, std::string val2_idx, AsmCode* code)
        : CodeBlock(id, code), op(type), val1(val1), val2(val2), val1_idx(val1_idx), val2_idx(val2_idx) {
        block_type = "CondBlock";
    }

    std::string get_vals_to_logger() override {
        return "ID=" + std::to_string(ID) + ", val1 = " + val1 + ", idx1 = " + val1_idx + ", val2 = " + val2 + ", idx2 = " + val2_idx;
    }

    virtual std::string get_val() override {
        return val1;
    }

    virtual std::string get_val_idx() override {
        return val1_idx;
    }

    virtual std::string get_val2() override {
        return val2;
    }

    virtual std::string get_val2_idx() override {
        return val2_idx;
    }

    virtual CondOperatorType get_cond() override {
        return op;
    }
};

class AssignBlock : public CodeBlock {
    std::string val;
    std::string val_idx = "";
    Expression expr;

public:
    AssignBlock(int id, std::string val, Expression expression, AsmCode* code) : CodeBlock(id, code), val(val), expr(expression) {
        block_type = "AssignBlock";
    }

    AssignBlock(int id, std::string val, std::string idx, Expression expression, AsmCode* code)
        : CodeBlock(id, code), val(val), val_idx(idx), expr(expression) {
        block_type = "AssignBlock";
    }

    std::string get_vals_to_logger() override {
        return "ID=" + std::to_string(ID) + ", val = " + val + ", idx = " + val_idx + ", " + expr.get_vals_to_logger();
    }

    virtual std::string get_val() override {
        return val;
    }

    virtual std::string get_val_idx() override {
        return val_idx;
    }

    virtual Expression* get_expression() override {
        return &expr;
    }
};

class ProcedureCall : public CodeBlock {
    std::string procedure_name;
    std::vector<std::string> params;

public:
    ProcedureCall(int id, std::string name, std::vector<std::string> params, AsmCode* code)
        : CodeBlock(id, code), procedure_name(name), params(params) {
        block_type = "ProcedureCall";
    }

    std::string get_vals_to_logger() override {
        std::string v = "ID=" + std::to_string(ID) + ", name = " + procedure_name + ", params: ";
        for (auto p : params)
            v += p + ", ";
        return v;
    }

    virtual std::string get_proc_name() override {
        return procedure_name;
    }

    virtual std::vector<std::string>* get_params() override {
        return &params;
    }
};

class KeywordBlock : public CodeBlock {
    Keyword type;
    std::string val = "";
    std::string val_idx = "";

public:
    KeywordBlock(int id, Keyword type, AsmCode* code) : CodeBlock(id, code), type(type) {
        block_type = "KeywordBlock";
    }

    KeywordBlock(int id, Keyword type, const std::string& val, const std::string& idx, AsmCode* code)
        : CodeBlock(id, code), type(type), val(val), val_idx(idx) {
        block_type = "KeywordBlock";
    }

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

    virtual std::string get_val() override {
        return val;
    }

    virtual std::string get_val_idx() override {
        return val_idx;
    }

    virtual Keyword get_keyword() override {
        return type;
    }
};

#endif  // COMPILER_INCLUDE_CODEBLOCK_H
