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

    std::vector<AsmInstruction> asm_instructions;
public:
    int ins_ptr = 0;

    AsmCode() : asm_instructions() {}

    void add(AsmInstruction instruction) {
        logger.log("push back instruction");
        asm_instructions.push_back(instruction);
    }

    void print_asm_code();
    void create_const_in_reg(int n, const std::string& reg);
    void store_ra_in_p(const int p_id);
    void asm_multiply();
};

#endif  // COMPILER_INCLUDE_ASMCODE_H
