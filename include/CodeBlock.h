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
    // TODO(): check if idx num (decide wheter its even important)
public:
    CondBlock(int id, CondOperatorType type, std::string val1, std::string val2)
        : CodeBlock(id), op(type), val1(val1), val2(val2) {}
    
    CondBlock(int id, CondOperatorType type, std::string val1, std::string val2, std::string val1_idx, std::string val2_idx)
        : CodeBlock(id), op(type), val1(val1), val2(val2), val1_idx(val1_idx), val2_idx(val2_idx) {}

    std::string get_vals_to_logger() {
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

    void translate_to_asm() override {
    }
};

class ProcedureCall : public CodeBlock {
    std::string procedure_name;
    std::vector<std::string> params; //check default empty proc

public:
    ProcedureCall(int id, std::string name) : CodeBlock(id), procedure_name(name) {}

    void translate_to_asm() override {
    }
};

// class KeywordBlock : public CodeBlock {};

#endif  // COMPILER_INCLUDE_CODEBLOCK_H
