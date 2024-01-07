#include "MemoryManager.h"
#include "AsmCode.h"

bool is_num(const std::string& val) {
    try {
        //size_t pos;
        int value = std::stoi(val);
        //return pos == str.length();
        return true;
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
}

// Creates CondBlock and adds it to the graph. Returns its Configuration id (index in configs).
int MemoryManager::add_cond_block(CondOperatorType type, const std::string& val1, const std::string& val2, const std::string& val1_idx, const std::string& val2_idx) {
    graph.push_back(std::make_shared<CondBlock>(id_counter, type, val1, val2, val1_idx, val2_idx, &asm_code));
    logger.log("Created CondBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates AssignBlock and adds it to the graph. Returns its index in configs.
int MemoryManager::add_assign_block(const std::string& val, const std::string& val_idx) {
    graph.push_back(std::make_shared<AssignBlock>(id_counter, val, val_idx, expr_buffor.back(), &asm_code));
    logger.log("Created AssignBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates empty/proc_end block and adds it to the graph. Returns its index in configs.
int MemoryManager::add_keyword_block(Keyword type) {
    graph.push_back(std::make_shared<KeywordBlock>(id_counter, type, &asm_code));
    logger.log("Created KeywordBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates WRITE/READ block and adds it to the graph. Returns its index in configs.
int MemoryManager::add_keyword_block(Keyword type, const std::string& val, const std::string& val_idx) {
    graph.push_back(std::make_shared<KeywordBlock>(id_counter, type, val, val_idx, &asm_code));
    logger.log("Created KeywordBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

int MemoryManager::add_proc_call(const std::string& name, std::vector<std::string> args) {
    graph.push_back(std::make_shared<ProcedureCall>(id_counter, name, args, &asm_code));
    logger.log("Created ProcedureCall: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates Expression and adds it to the buffor. Returns its index.
int MemoryManager::add_expr_to_buffor(ExprOperatorType op, const std::string& val1, const std::string& val2, const std::string& idx1, const std::string& idx2) {
    expr_buffor.push_back(Expression(op, val1, val2, idx1, idx2));
    logger.log("Created " + expr_buffor.back().get_vals_to_logger() + "| expr_buffor.size=" + std::to_string(expr_buffor.size()));
    return expr_buffor.size() - 1;
}

void MemoryManager::clear_args_decl_buffor() {
    args_decl_buffor.clear();
}

void MemoryManager::add_val_to_buffor(ValType type, const std::string& name) {
    args_decl_buffor.push_back(Value(type, name));
    logger.log("|add_val_to_buffor| Created value template: " + args_decl_buffor.back().get_vals_to_logger() + "| args_decl_buffor.size=" + std::to_string(args_decl_buffor.size()));
}

void MemoryManager::add_const(const std::string& num) {
    if (consts_map.find(num) == consts_map.end()) {
        global_consts.push_back(Value(memory_counter, ValType::_NUM, num));
        logger.log("|add_consts| Created NUM Value " + global_consts.back().get_vals_to_logger() + "| global_consts.size=" + std::to_string(global_consts.size()));
        consts_map[num] = memory_counter;
        logger.log("Added (" + num + "," + std::to_string(memory_counter) + ") to the consts map.");
        memory_counter++;
    } else {
        logger.log("|add_consts| NUM Value " + num + " is already in the map.");
    }
}

int MemoryManager::connect_blocks(const std::string& config1_idx, const std::string& config2_idx) {
    logger.log("|connect_blocks| Configuration1 idx=" + config1_idx + ", Configuration2 idx=" + config2_idx);
    int idx1 = std::stoi(config1_idx);
    int idx2 = std::stoi(config2_idx);
    logger.log("Configuration1: " + configs[idx1].get_vals_to_logger() + ", Configuration2: " + configs[idx2].get_vals_to_logger());

    int from_id = configs[idx1].end_id; 
    int to_id = configs[idx2].begin_id;
    logger.log("Setting next_TRUE and next_FALSE of block ID=" + std::to_string(from_id) + " to block ID=" + std::to_string(to_id));
    graph[from_id]->set_next_true(to_id);
    graph[from_id]->set_next_false(to_id);
    logger.log(graph[from_id]->get_nbhs_to_logger());

    configs.push_back(Configuration(configs[idx1].begin_id, configs[idx2].end_id));
    logger.log("Created Configuration (" + configs.back().get_vals_to_logger() + ") | configs.size=" + std::to_string(configs.size()));
    return configs.size() - 1;
}

int MemoryManager::connect_if(const std::string& cond_config, const std::string& commands_config) {
    logger.log("|connect_if| condition Configuration idx=" + cond_config + ", commmands Configuration idx=" + commands_config);
    int cond_idx = std::stoi(cond_config);
    int commands_idx = std::stoi(commands_config);
    logger.log("condition: " + configs[cond_idx].get_vals_to_logger() + ", commands: " + configs[commands_idx].get_vals_to_logger());

    int endif_config = add_keyword_block(Keyword::_EMPTY);
    int endif_idx = configs[endif_config].begin_id;
    int from = configs[cond_idx].end_id;
    int comm_begin = configs[commands_idx].begin_id;
    int comm_end = configs[commands_idx].end_id;
    logger.log("Setting next_TRUE of block ID=" + std::to_string(from) + " to block ID=" + std::to_string(comm_begin));
    graph[from]->set_next_true(comm_begin);
    logger.log("Setting next_FALSE of block ID=" + std::to_string(from) + " to block ID=" + std::to_string(endif_idx));
    graph[from]->set_next_false(endif_idx);
    logger.log(graph[from]->get_nbhs_to_logger());

    logger.log("Setting next_TRUE and next_FALSE of block ID=" + std::to_string(comm_end) + " to block ID=" + std::to_string(endif_idx));
    graph[comm_end]->set_next_true(endif_idx);
    graph[comm_end]->set_next_false(endif_idx);
    logger.log(graph[comm_end]->get_nbhs_to_logger());
    
    configs.push_back(Configuration(from, endif_idx));
    logger.log("Created Configuration (" + configs.back().get_vals_to_logger() + ") | configs.size=" + std::to_string(configs.size()));
    return configs.size() - 1;
}

int MemoryManager::connect_if_else(const std::string& cond_config, const std::string& commands_config, const std::string& else_config) {
    logger.log("|connect_if_else| condition Configuration idx=" + cond_config + ", commmands Configuration idx=" + commands_config + 
               ", else_commands Configuration idx=" + else_config);
    int cond_idx = std::stoi(cond_config);
    int commands_idx = std::stoi(commands_config);
    int else_idx = std::stoi(else_config);
    logger.log("condition: " + configs[cond_idx].get_vals_to_logger() + ", commands: " + configs[commands_idx].get_vals_to_logger() +
               ", else: " + configs[else_idx].get_vals_to_logger());

    int endif_config = add_keyword_block(Keyword::_EMPTY);
    int endif_idx = configs[endif_config].begin_id;
    int from = configs[cond_idx].end_id;
    int comm_begin = configs[commands_idx].begin_id;
    int else_begin = configs[else_idx].begin_id;
    logger.log("Setting next_TRUE of block ID=" + std::to_string(from) + " to block ID=" + std::to_string(comm_begin));
    graph[from]->set_next_true(comm_begin);
    logger.log("Setting next_FALSE of block ID=" + std::to_string(from) + " to block ID=" + std::to_string(else_begin));
    graph[from]->set_next_false(else_begin);
    logger.log(graph[from]->get_nbhs_to_logger());

    int comm_end = configs[commands_idx].end_id;
    int else_end = configs[else_idx].end_id;
    logger.log("Setting next_TRUE and next_FALSE of block ID=" + std::to_string(comm_end) + " to block ID=" + std::to_string(endif_idx));
    graph[comm_end]->set_next_true(endif_idx);
    graph[comm_end]->set_next_false(endif_idx);
    logger.log(graph[comm_end]->get_nbhs_to_logger());
    logger.log("Setting next_TRUE and next_FALSE of block ID=" + std::to_string(else_end) + " to block ID=" + std::to_string(endif_idx));
    graph[else_end]->set_next_true(endif_idx);
    graph[else_end]->set_next_false(endif_idx);
    logger.log(graph[else_end]->get_nbhs_to_logger());

    configs.push_back(Configuration(from, endif_idx));
    logger.log("Created Configuration (" + configs.back().get_vals_to_logger() + ") | configs.size=" + std::to_string(configs.size()));
    return configs.size() - 1;
}

int MemoryManager::connect_while(const std::string& cond_config, const std::string& commands_config) {
    logger.log("|connect_while| condition Configuration idx=" + cond_config + ", commmands Configuration idx=" + commands_config);
    int cond_idx = std::stoi(cond_config);
    int commands_idx = std::stoi(commands_config);
    logger.log("condition: " + configs[cond_idx].get_vals_to_logger() + ", commands: " + configs[commands_idx].get_vals_to_logger());

    int endwhile_config = add_keyword_block(Keyword::_EMPTY);
    int endwhile_id = configs[endwhile_config].begin_id;
    int from = configs[cond_idx].end_id;
    int comm_begin = configs[commands_idx].begin_id;
    int comm_end = configs[commands_idx].end_id;
    logger.log("Setting next_TRUE of block ID=" + std::to_string(from) + " to block ID=" + std::to_string(comm_begin));
    graph[from]->set_next_true(comm_begin);
    logger.log("Setting next_FALSE of block ID=" + std::to_string(from) + " to block ID=" + std::to_string(endwhile_id));
    graph[from]->set_next_false(endwhile_id);
    logger.log(graph[from]->get_nbhs_to_logger());

    logger.log("Setting next_TRUE and next_FALSE of block ID=" + std::to_string(comm_end) + " to block ID=" + std::to_string(from));
    graph[comm_end]->set_next_true(from);
    graph[comm_end]->set_next_false(from);
    logger.log(graph[comm_end]->get_nbhs_to_logger());

    configs.push_back(Configuration(from, endwhile_id));
    logger.log("Created Configuration (" + configs.back().get_vals_to_logger() + ") | configs.size=" + std::to_string(configs.size()));
    return configs.size() - 1;
}

int MemoryManager::connect_repeat_until(const std::string& commands_config, const std::string& cond_config) {
    logger.log("|connect_repeat_until| condition Configuration idx=" + cond_config + ", commmands Configuration idx=" + commands_config);
    int cond_idx = std::stoi(cond_config);
    int commands_idx = std::stoi(commands_config);
    logger.log("condition: " + configs[cond_idx].get_vals_to_logger() + ", commands: " + configs[commands_idx].get_vals_to_logger());

    int empty_config = add_keyword_block(Keyword::_EMPTY);
    int empty_id = configs[empty_config].begin_id;
    int cond = configs[cond_idx].end_id;
    int comm_begin = configs[commands_idx].begin_id;
    int comm_end = configs[commands_idx].end_id;

    logger.log("Setting next_TRUE and next_FALSE of block ID=" + std::to_string(comm_end) + " to block ID=" + std::to_string(cond));
    graph[comm_end]->set_next_true(cond);
    graph[comm_end]->set_next_false(cond);
    logger.log(graph[comm_end]->get_nbhs_to_logger());

    logger.log("Setting next_TRUE of block ID=" + std::to_string(cond) + " to block ID=" + std::to_string(empty_id));
    graph[cond]->set_next_true(empty_id);
    logger.log("Setting next_FALSE of block ID=" + std::to_string(cond) + " to block ID=" + std::to_string(comm_begin));
    graph[cond]->set_next_false(comm_begin);
    logger.log(graph[cond]->get_nbhs_to_logger());

    configs.push_back(Configuration(comm_begin, empty_id));
    logger.log("Created Configuration (" + configs.back().get_vals_to_logger() + ") | configs.size=" + std::to_string(configs.size()));
    return configs.size() - 1;
}

int MemoryManager::create_procedure() {
    logger.log("|create_procedure| Creating an empty procedure.");
    procedures.push_back(Procedure());
    logger.log("procedures.size=" + std::to_string(procedures.size()));
    return procedures.size() - 1;
}

void MemoryManager::add_val_to_procedure(const std::string& proc_id, ValType type, const std::string& name, const std::string& size) {
    logger.log("|add_val_to_procedure| " + proc_id + ", " + name);
    int idx = std::stoi(proc_id);

    if (type == ValType::_ARR) {
        procedures[idx].add_local_arr(memory_counter, name, std::stoi(size));
        memory_counter += std::stoi(size);
    } else {
        procedures[idx].add_local_val(memory_counter, name);
        memory_counter++;
    }
}

void MemoryManager::set_procedure_name(const std::string& proc_id, const std::string& name) {
    logger.log("|set_procedure_name| " + proc_id + ", " + name);
    int idx = std::stoi(proc_id);
    procedures[idx].set_name(name);
    procedures[idx].add_params_templates(args_decl_buffor);
    clear_args_decl_buffor();
}

void MemoryManager::set_procedure_head(const std::string& proc_id, const std::string& conf) {
    logger.log("|set_procedure_head| " + proc_id + ", Configuration: " + conf);
    int idx = std::stoi(proc_id);
    int h = configs[std::stoi(conf)].begin_id;
    procedures[idx].set_head(h);
}

int MemoryManager::get_val_id(const std::string& name, ValType type, const int proc_id) {
    return procedures[proc_id].get_val_id(name, type);
}

int MemoryManager::get_const_id(const std::string& num) {
    return consts_map.find(num)->second;
}

void MemoryManager::set_procedures_in_graph() {
    int curr = 0;
    int i = 0;
    while (curr < procedures.size() - 1) {
        while (i < procedures[curr+1].get_head()) {
            graph[i]->procedure_num = curr;
            i++;
        }
        curr++;
    }
    while (i < graph.size()) {
        graph[i]->procedure_num = curr;
        i++;
    }
    log_procedures_info();
}

void MemoryManager::log_procedures_info() {
    for (auto p : procedures) {
        p.log_info();
    }
}

void MemoryManager::export_ast() {
    for (auto block : graph) {
        if (block->next_TRUE != -1) {
            std::cout << block->ID << " " << block->next_TRUE << std::endl;
            if (block->next_FALSE != block->next_TRUE)
                std::cout << block->ID << " " << block->next_FALSE << std::endl;
        }
    }
}

/*----------------------------------------------------------------------------------------------*/

void MemoryManager::initialize_all_consts() {
    for (auto n : global_consts) {
        asm_code.create_const_in_reg(std::stoi(n.get_name()), "a");
        asm_code.store_ra_in_p(n.get_id());
    }
}

void MemoryManager::translate_block(std::shared_ptr<CodeBlock> block) {
    std::string bt = block->block_type;
    if (bt == "KeywordBlock") {
        translate_keyword_block(block);
    } else if (bt == "AssignBlock") {
        translate_assign_block(block);
    } else if (bt == "CondBlock") {

    } else {

    }
}

void MemoryManager::translate_keyword_block(std::shared_ptr<CodeBlock> block) {
    const std::string val = block->get_val();
    const std::string val_idx = block->get_val_idx();
    Keyword key = block->get_keyword();

    if (key == Keyword::_READ) {
        block->first_instruction_k = asm_code.get_k();
        if (val_idx == "") {
            asm_code.create_const_in_reg(get_val_id(val, ValType::_ID, block->procedure_num), "b");
            asm_code.add("READ", "", "# read " + val);
            asm_code.add("STORE", "b");
        } else {
            int idx_id = is_num(val_idx) ? get_const_id(val_idx) : get_val_id(val_idx, ValType::_ID, block->procedure_num);
            asm_code.place_id_in_ra(get_val_id(val, ValType::_ARR, block->procedure_num), idx_id);
            asm_code.add("PUT", "b");
            asm_code.add("READ", "", "# read " + val + "[" + val_idx + "]");
            asm_code.add("STORE", "b");
        }
    } else if (key == Keyword::_WRITE) {
        block->first_instruction_k = asm_code.get_k();
        if (val_idx == "") {
            int idx_id = is_num(val) ? get_const_id(val) : get_val_id(val, ValType::_ID, block->procedure_num);
            asm_code.create_const_in_reg(idx_id, "d");
            asm_code.add("LOAD", "d");
            asm_code.add("WRITE", "");
        } else {
            int idx_id = is_num(val_idx) ? get_const_id(val_idx) : get_val_id(val_idx, ValType::_ID, block->procedure_num);
            asm_code.place_id_in_ra(get_val_id(val, ValType::_ARR, block->procedure_num), idx_id);
            asm_code.add("LOAD", "a");
            asm_code.add("WRITE", "");
        }
    }
}

void MemoryManager::translate_assign_block(std::shared_ptr<CodeBlock> block) {
    block->first_instruction_k = asm_code.get_k();

    Expression* expr = block->get_expression();
    place_expr_values_in_rb_rc(expr->get_val1(), expr->get_val1_idx(), expr->get_val2(), expr->get_val2_idx(), block->procedure_num);
    //log?

    const std::string val = block->get_val();
    const std::string val_idx = block->get_val_idx();

    switch (expr->get_operator()) {  // b,c used!
    case ExprOperatorType::_NOOP:
        if (val_idx == "") {
            int idx_id = is_num(val) ? get_const_id(val) : get_val_id(val, ValType::_ID, block->procedure_num);
            asm_code.create_const_in_reg(idx_id, "e");
            asm_code.add("GET", "b");
            asm_code.add("STORE", "e", "# " + val + ":=");
        } else {
            int idx_id = is_num(val_idx) ? get_const_id(val_idx) : get_val_id(val_idx, ValType::_ID, block->procedure_num);
            asm_code.place_id_in_ra(get_val_id(val, ValType::_ARR, block->procedure_num), idx_id);
            asm_code.add("PUT", "e");
            asm_code.add("GET", "b");
            asm_code.add("STORE", "e", "# " + val + ":=");
        }
        break;
    case ExprOperatorType::_PLUS:

        break;
    case ExprOperatorType::_MIN:

        break;
    case ExprOperatorType::_MUL: // d
        if (val_idx == "") {
            int idx_id = is_num(val) ? get_const_id(val) : get_val_id(val, ValType::_ID, block->procedure_num);
            asm_code.create_const_in_reg(idx_id, "g");  // adres docelowy
            asm_code.add("STRK", "h", "# store k in h");
            asm_code.add("JUMP", std::to_string(asm_code.mul_k), "# multiply");
            asm_code.add("GET", "d");
            asm_code.add("STORE", "g", "# " + val + ":=");
        } else {
            int idx_id = is_num(val_idx) ? get_const_id(val_idx) : get_val_id(val_idx, ValType::_ID, block->procedure_num);
            asm_code.place_id_in_ra(get_val_id(val, ValType::_ARR, block->procedure_num), idx_id);
            asm_code.add("PUT", "g");
            asm_code.add("STRK", "h", "# store k in h");
            asm_code.add("JUMP", std::to_string(asm_code.mul_k), "# multiply");
            asm_code.add("GET", "d");
            asm_code.add("STORE", "g", "# " + val + ":=");
        }
        break;
    default:  // div/mod
        if (val_idx == "") {
            int idx_id = is_num(val) ? get_const_id(val) : get_val_id(val, ValType::_ID, block->procedure_num);
            asm_code.create_const_in_reg(idx_id, "g");  // adres docelowy
            asm_code.add("STRK", "h", "# store k in h");
            asm_code.add("JUMP", std::to_string(asm_code.div_k), "# divide");
        } else {
            int idx_id = is_num(val_idx) ? get_const_id(val_idx) : get_val_id(val_idx, ValType::_ID, block->procedure_num);
            asm_code.place_id_in_ra(get_val_id(val, ValType::_ARR, block->procedure_num), idx_id);
            asm_code.add("PUT", "g");
            asm_code.add("STRK", "h", "# store k in h");
            asm_code.add("JUMP", std::to_string(asm_code.div_k), "# divide");
        }

        if (expr->get_operator() == ExprOperatorType::_DIV) {
            asm_code.add("GET", "d");
            asm_code.add("STORE", "g", "# " + val + ":=");
        } else {
            asm_code.add("GET", "b");
            asm_code.add("STORE", "g", "# " + val + ":=");
        }
        break;
    }

}

void MemoryManager::place_expr_values_in_rb_rc(const std::string& val1, const std::string& val1_idx, const std::string& val2,
                                               const std::string& val2_idx, const int proc_num) {
    logger.log("|place_expr_values_in_rb_rc| " + val1 + ", " + val1_idx + ", " + val2 + ", " + val2_idx);

    if (val1_idx == "") {
        int idx_id = is_num(val1) ? get_const_id(val1) : get_val_id(val1, ValType::_ID, proc_num);
        asm_code.create_const_in_reg(idx_id, "a");
    } else {
        int idx_id = is_num(val1_idx) ? get_const_id(val1_idx) : get_val_id(val1_idx, ValType::_ID, proc_num);
        asm_code.place_id_in_ra(get_val_id(val1, ValType::_ARR, proc_num), idx_id);  // uses e,f
    }
    asm_code.add("LOAD", "a");
    asm_code.add("PUT", "b");

    if (val2 == "") {  // _NOOP
        return;
    }

    if (val2_idx == "") {
        int idx_id = is_num(val2) ? get_const_id(val2) : get_val_id(val2, ValType::_ID, proc_num);
        asm_code.create_const_in_reg(idx_id, "a");
    } else {
        int idx_id = is_num(val2_idx) ? get_const_id(val2_idx) : get_val_id(val2_idx, ValType::_ID, proc_num);
        asm_code.place_id_in_ra(get_val_id(val2, ValType::_ARR, proc_num), idx_id);  // uses e,f
    }
    asm_code.add("LOAD", "a");
    asm_code.add("PUT", "c");
}

void MemoryManager::translate_expression(Expression* expr) {}

/*----------------------------------------------------------------------------------------------*/

void MemoryManager::test() {
    initialize_all_consts();
    asm_code.add("JUMP", std::to_string(asm_code.get_k()+20+32), "# przeskocz mul/div");
    asm_code.asm_multiply();
    asm_code.asm_divide();
    for (auto g : graph)
        translate_block(g);
    asm_code.add("HALT", "");
    asm_code.print_asm_code();
}
