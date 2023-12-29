#ifndef COMPILER_INCLUDE_ASMINSTRUCTION_H
#define COMPILER_INCLUDE_ASMINSTRUCTION_H

#include <string>

class AsmInstruction {
    std::string code;
    std::string arg = "";
    std::string comment = "";
    int k;
public:
    AsmInstruction(const std::string& code, int i_ptr) : code(code), k(i_ptr) {}

    std::string get_as_string() {
        return code + " " + arg + " " + comment + "\n";
    }
};

#endif  // COMPILER_INCLUDE_ASMINSTRUCTION_H