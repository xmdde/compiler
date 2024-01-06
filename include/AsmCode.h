#ifndef COMPILER_INCLUDE_ASMCODE_H
#define COMPILER_INCLUDE_ASMCODE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "Logger.h"

class AsmInstruction {
    std::string code;
    std::string arg = "";
    std::string comment = "";

public:
    int k;

    AsmInstruction(const std::string& code, int i_ptr) : code(code), k(i_ptr) {}
    AsmInstruction(const std::string& code, const std::string& arg, int i_ptr) : code(code), arg(arg), k(i_ptr) {}
    AsmInstruction(const std::string& code, const std::string& arg, int i_ptr, const std::string& comm) : code(code), arg(arg), k(i_ptr), comment(comm) {}

    std::string get_as_string() {
        return code + " " + arg + "   " + comment + "\n";
    }
};

class AsmCode {
    logging::Logger logger = logging::Logger("asm_logs.log");
    int ins_ptr = 0;
    std::vector<AsmInstruction> asm_instructions;

public:
    AsmCode() : asm_instructions() {}

    int get_k() {
        return ins_ptr;
    }

    void add(AsmInstruction instruction) {
        logger.log("push back instruction");
        asm_instructions.push_back(instruction);
    }

    void add(const std::string& code, const std::string& arg) {
        asm_instructions.push_back(AsmInstruction(code, arg, ins_ptr++));
    }

    void add(const std::string& code, const std::string& arg, const std::string& com) {
        asm_instructions.push_back(AsmInstruction(code, arg, ins_ptr++, com));
    }

    void print_asm_code();
    void create_const_in_reg(int n, const std::string& reg);
    void store_ra_in_p(const int p_id);
    void asm_multiply();

    void place_id_in_ra(int id, int idx_id);
};

#endif  // COMPILER_INCLUDE_ASMCODE_H
