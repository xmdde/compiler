#ifndef COMPILER_INCLUDE_ASMCODE_H
#define COMPILER_INCLUDE_ASMCODE_H

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stack>

#include "Logger.h"

class AsmInstruction {
    std::string code;
    std::string arg = "";
    std::string comment = "";

public:
    int k;

    AsmInstruction(const std::string& code, int i_ptr) : code(code), k(i_ptr) {}

    AsmInstruction(const std::string& code, const std::string& arg, int i_ptr) : code(code), arg(arg), k(i_ptr) {}

    std::string get_as_string() {
        return code + " " + arg + " " + comment + "\n";
    }
};

class AsmCode {
    //logging::Logger logger{"asm_logs.log"};

    std::vector<AsmInstruction> asm_instructions;
    int ins_ptr = 0;
public:
    AsmCode() : asm_instructions() {}

    void add(AsmInstruction instruction) {
        //logger.log("push back instruction");
        asm_instructions.push_back(instruction);
    }

    void print_asm_code() {
        for (auto i : asm_instructions) {
            std::cout << i.get_as_string();
        }
    }

    void create_const_in_reg(int n, const std::string& reg) {
        std::stack<int> bin;
    while (n != 1) {
        bin.push(n%2);
        n /= 2;
    }
    asm_instructions.push_back(AsmInstruction("RST", reg, ins_ptr++));
    asm_instructions.push_back(AsmInstruction("INC", reg, ins_ptr++));
    while (!bin.empty()) {
        if (bin.top() == 0) {
            asm_instructions.push_back(AsmInstruction("SHL", reg, ins_ptr++));
        } else {
            asm_instructions.push_back(AsmInstruction("SHL", reg, ins_ptr++));
            asm_instructions.push_back(AsmInstruction("INC", reg, ins_ptr++));
        }
        bin.pop();
    }
    }

};

#endif  // COMPILER_INCLUDE_ASMCODE_H