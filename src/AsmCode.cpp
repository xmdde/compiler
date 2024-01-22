#include "AsmCode.h"

#include <stack>

void AsmCode::print_asm_code() {
    for (auto i : asm_instructions) {
        std::cout << i.get_as_string();
    }
}

void AsmCode::create_const_in_reg(long long n, const std::string& reg) {
    const std::string nn = std::to_string(n);
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

void AsmCode::place_id_in_ra(const int id, const int idx_id, const bool is_id_param, const bool is_idx_param) {  // for arrays
    create_const_in_reg(id, "e");
    if (is_id_param) {
        indirect_load_put("e");
    }
    create_const_in_reg(idx_id, "f");
    if (is_idx_param) {
        indirect_load_put("f");
    }
    asm_instructions.push_back(AsmInstruction("LOAD", "f", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("ADD", "e", ins_ptr++));  // mamy adres w ra
}

void AsmCode::place_id_in_ra_idx_num(const int id, const long long idx, const bool is_id_param) {
    create_const_in_reg(id, "e");
    create_const_in_reg(idx, "f");
    if (is_id_param) {
        indirect_load_put("e");
        asm_instructions.push_back(AsmInstruction("ADD", "f", ins_ptr++));  // mamy adres w ra
    } else {
        asm_instructions.push_back(AsmInstruction("GET", "f", ins_ptr++));
        asm_instructions.push_back(AsmInstruction("ADD", "e", ins_ptr++));  // mamy adres w ra
    }
}

void AsmCode::store_ra_in_p(const int p_id) {
    create_const_in_reg(p_id, "g");
    asm_instructions.push_back(AsmInstruction("STORE", "g", ins_ptr++));
}

// [ID]rx --> [p_ID]rx
void AsmCode::indirect_load_put(const std::string& reg) {
    asm_instructions.push_back(AsmInstruction("LOAD", reg, ins_ptr++));
    if (reg != "a") {
        asm_instructions.push_back(AsmInstruction("PUT", reg, ins_ptr++));  // if a to nic sie nie dzieje
    }
}

void AsmCode::get_ins_to_complete(std::vector<int>& ins_to_resolve) {
    for (int i = 0; i < asm_instructions.size(); i++) {
        if (asm_instructions[i].jump_to_resolve)
            ins_to_resolve.push_back(i);
    }
}

void AsmCode::complete_jump(const int idx, const int k) {
    asm_instructions[idx].complete_jump(k);
}

void AsmCode::jump_to_next_block(const int block_id) {
    asm_instructions.push_back(AsmInstruction("JUMP", ins_ptr++, true, block_id));
}

bool AsmCode::get_where_jump_type(const int idx) {
    return asm_instructions[idx].where_jump;
}

int AsmCode::get_block_id(const int idx) {
    return asm_instructions[idx].get_block_id();
}

void AsmCode::cond__lless(const std::string& r1, const std::string& r2, const int block_id) {
    asm_instructions.push_back(AsmInstruction("GET", r2, ins_ptr++, "# check <"));
    asm_instructions.push_back(AsmInstruction("SUB", r1, ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JPOS", ins_ptr++, true, block_id));
    asm_instructions.push_back(AsmInstruction("JUMP", ins_ptr++, false, block_id));
}

void AsmCode::cond__lleq(const std::string& r1, const std::string& r2, const int block_id) {
    asm_instructions.push_back(AsmInstruction("GET", r1, ins_ptr++, "# check <="));
    asm_instructions.push_back(AsmInstruction("SUB", r2, ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JPOS", ins_ptr++, false, block_id));
    asm_instructions.push_back(AsmInstruction("JUMP", ins_ptr++, true, block_id));
}

void AsmCode::cond__eq(const int block_id) {  // b,c
    asm_instructions.push_back(AsmInstruction("GET", "b", ins_ptr++, "# check ="));
    asm_instructions.push_back(AsmInstruction("SUB", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JPOS", ins_ptr++, false, block_id));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "b", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JPOS", ins_ptr++, false, block_id));
    asm_instructions.push_back(AsmInstruction("JUMP", ins_ptr++, true, block_id));
}

void AsmCode::cond__neq(const int block_id) {  // b,c
    asm_instructions.push_back(AsmInstruction("GET", "b", ins_ptr++, "# check !="));
    asm_instructions.push_back(AsmInstruction("SUB", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JPOS", ins_ptr++, true, block_id));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "b", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JPOS", ins_ptr++, true, block_id));
    asm_instructions.push_back(AsmInstruction("JUMP", ins_ptr++, false, block_id));
}

void AsmCode::cond_eq_zero(const int block_id) { // b
    asm_instructions.push_back(AsmInstruction("GET", "b", ins_ptr++, "# check ="));
    asm_instructions.push_back(AsmInstruction("JZERO", ins_ptr++, true, block_id));
    asm_instructions.push_back(AsmInstruction("JUMP", ins_ptr++, false, block_id));
}

void AsmCode::cond_neq_zero(const int block_id) { // b
    asm_instructions.push_back(AsmInstruction("GET", "b", ins_ptr++, "# check !="));
    asm_instructions.push_back(AsmInstruction("JPOS", ins_ptr++, true, block_id));
    asm_instructions.push_back(AsmInstruction("JUMP", ins_ptr++, false, block_id));
}

// MULTIPLY b * c (result in d), uses: a,b,c,d,e + instruction ptr in h
void AsmCode::asm_multiply() {
    mul_k = ins_ptr;
    asm_instructions.push_back(AsmInstruction("RST", "d", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    int k_tmp = ins_ptr + 14;
    asm_instructions.push_back(AsmInstruction("JZERO", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHR", "a", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHL", "a", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("PUT", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "e", ins_ptr++));
    k_tmp = ins_ptr + 4;
    asm_instructions.push_back(AsmInstruction("JZERO", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "d", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("ADD", "b", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("PUT", "d", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHR", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHL", "b", ins_ptr++));
    k_tmp = ins_ptr - 13;
    asm_instructions.push_back(AsmInstruction("JUMP", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("INC", "h", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("INC", "h", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JUMPR", "h", ins_ptr++));  // wynik w d, adres powrotu w h
}

// DIVIDE b/c (result in d, modulo in b), uses: a,b,c,d,e,f + instruction ptr in h
void AsmCode::asm_divide() {
    div_k = ins_ptr;
    asm_instructions.push_back(AsmInstruction("RST", "d", ins_ptr++, "# div"));
    asm_instructions.push_back(AsmInstruction("RST", "e", ins_ptr++, "# div"));
    asm_instructions.push_back(AsmInstruction("RST", "f", ins_ptr++, "# div"));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    int k_tmp = ins_ptr + 3;
    asm_instructions.push_back(AsmInstruction("JPOS", std::to_string(k_tmp), ins_ptr++)); // 22
    asm_instructions.push_back(AsmInstruction("RST", "b", ins_ptr++));
    k_tmp = ins_ptr + 21;
    asm_instructions.push_back(AsmInstruction("JZERO", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "b", ins_ptr++));
    k_tmp = ins_ptr + 18;
    asm_instructions.push_back(AsmInstruction("JPOS", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("PUT", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("RST", "f", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("INC", "f", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "b", ins_ptr++));
    k_tmp = ins_ptr + 10;
    asm_instructions.push_back(AsmInstruction("JPOS", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "b", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("PUT", "b", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "d", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("ADD", "f", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("PUT", "d", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHL", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHL", "f", ins_ptr++));
    k_tmp = ins_ptr - 11;
    asm_instructions.push_back(AsmInstruction("JUMP", std::to_string(k_tmp), ins_ptr++));
    k_tmp = ins_ptr - 19;
    asm_instructions.push_back(AsmInstruction("JUMP", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("INC", "h", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("INC", "h", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JUMPR", "h", ins_ptr++));  // wynik w d, mod w b
}

// b MOD c (result in b)
void AsmCode::asm_modulo() {
    mod_k = ins_ptr;
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    int k_tmp = ins_ptr + 3;
    asm_instructions.push_back(AsmInstruction("JPOS", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("RST", "b", ins_ptr++));
    k_tmp = ins_ptr + 18;
    asm_instructions.push_back(AsmInstruction("JUMP", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "b", ins_ptr++));
    k_tmp = ins_ptr + 15;
    asm_instructions.push_back(AsmInstruction("JPOS", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "c", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("PUT", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("RST", "f", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("INC", "f", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "b", ins_ptr++));
    k_tmp = ins_ptr + 7;
    asm_instructions.push_back(AsmInstruction("JPOS", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("GET", "b", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SUB", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("PUT", "b", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHL", "e", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("SHL", "f", ins_ptr++));
    k_tmp = ins_ptr - 8;
    asm_instructions.push_back(AsmInstruction("JUMP", std::to_string(k_tmp), ins_ptr++));
    k_tmp = ins_ptr - 16;
    asm_instructions.push_back(AsmInstruction("JUMP", std::to_string(k_tmp), ins_ptr++));
    asm_instructions.push_back(AsmInstruction("INC", "h", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("INC", "h", ins_ptr++));
    asm_instructions.push_back(AsmInstruction("JUMPR", "h", ins_ptr++));
}

void AsmCode::save_to_file(const char* filename) {
    std::ofstream ofs(filename, std::ios_base::out);
    for (auto i : asm_instructions) {
        ofs << i.get_as_string();
    }
    ofs.close();
}
