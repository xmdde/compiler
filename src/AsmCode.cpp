#include <stack>

#include "AsmCode.h"

void AsmCode::print_asm_code() {
    for (auto i : asm_instructions) {
        std::cout << i.get_as_string();
    }
}

void AsmCode::create_const_in_reg(int n, const std::string& reg) {
    const std::string nn = std::to_string(n);
    logger.log("|create_const_in_reg| " + nn + " to " + reg);

    if (n == 0) {
        asm_instructions.push_back(AsmInstruction("RST", reg, ins_ptr++, "# make " + nn));
        return;
    }

    std::stack<int> bin;
    while (n != 1) {
        bin.push(n%2);
        n /= 2;
    }
    asm_instructions.push_back(AsmInstruction("RST", reg, ins_ptr++, "# make " + nn));
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

void AsmCode::store_ra_in_p(const int p_id) {
    create_const_in_reg(p_id, "b");
    asm_instructions.push_back(AsmInstruction("STORE", "b", ins_ptr++));
}

void AsmCode::asm_multiply() {
    asm_instructions.push_back(AsmInstruction("RST", "d", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JZERO", std::to_string(ins_ptr+14), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHR", "a", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHL", "a", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("PUT", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JZERO", std::to_string(ins_ptr+4), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "d", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("ADD", "b", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("PUT", "d", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHR", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHL", "b", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JUMP", std::to_string(ins_ptr-13), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JUMPR", "h", ins_ptr++));  // wynik w d, adres powrotu w h
}
