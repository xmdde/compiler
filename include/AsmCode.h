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
    int block_id; //ew id

public:
    int k;
    bool jump_to_resolve = false;
    bool where_jump;  // true: next_TRUE / false: next_FALSE

    AsmInstruction(const std::string& code, int i_ptr) : code(code), k(i_ptr) {}
    AsmInstruction(const std::string& code, const std::string& arg, int i_ptr) : code(code), arg(arg), k(i_ptr) {}
    AsmInstruction(const std::string& code, const std::string& arg, int i_ptr, const std::string& comm) : code(code), arg(arg), k(i_ptr), comment(comm) {}
    AsmInstruction(const std::string& code, int i_ptr, bool where_jump, int b) 
        : code(code), where_jump(where_jump), k(i_ptr), block_id(b) {
        jump_to_resolve = true;
    }

    std::string get_as_string() {
        return code + " " + arg + "   " + comment + "\n";
    }

    void complete_jump(int ins_num) {
        if (ins_num == -1) {
            arg = std::to_string(k+1);
        } else {
            arg = std::to_string(ins_num);
        }
    }

    bool get_where_jump() {
        return where_jump;
    }
};

class AsmCode {
    logging::Logger logger = logging::Logger("asm_logs.log");
    int ins_ptr = 0;
    std::vector<AsmInstruction> asm_instructions;

public:
    int mul_k;
    int div_k;

    AsmCode() : asm_instructions() {}

    int get_k() {
        return ins_ptr;
    }

    void add(AsmInstruction instruction) {
        asm_instructions.push_back(instruction);
    }

    void add(const std::string& code, const std::string& arg) {
        asm_instructions.push_back(AsmInstruction(code, arg, ins_ptr++));
    }

    void add(const std::string& code, const std::string& arg, const std::string& com) {
        asm_instructions.push_back(AsmInstruction(code, arg, ins_ptr++, com));
    }

    void asm_multiply();
    void asm_divide();

    void print_asm_code();
    void create_const_in_reg(int n, const std::string& reg);
    void store_ra_in_p(const int p_id);
    void place_id_in_ra(int id, int idx_id);  // TODO(me): place_id_in_ra(int id, int idx_id, bool if_id_param, bool if_idx_param);
    void indirect_load_put(const std::string& reg);
    void get_ins_to_complete(std::vector<int>& ins_to_resolve);
    bool get_where_jump_type(const int idx);
    void complete_jump(const int idx, const int k);
};

#endif  // COMPILER_INCLUDE_ASMCODE_H
