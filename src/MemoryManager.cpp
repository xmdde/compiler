#include "MemoryManager.h"
#include "AsmCode.h"

#include <regex>
#include <fstream>
#include <cmath>
#include <iostream>

bool is_num(const std::string& val) {
    std::smatch matcher;
    if (std::regex_match(val, matcher, std::regex("^\\d*$"))) {
        return true;
    } else {
        return false;
    }
}

// Creates CondBlock and adds it to the graph. Returns its Configuration id (index in configs).
int MemoryManager::add_cond_block(CondOperatorType type, const std::string& val1, const std::string& val2, const std::string& val1_idx,
                                  const std::string& val2_idx) {
    graph.push_back(std::make_shared<CondBlock>(id_counter, type, val1, val2, val1_idx, val2_idx, &asm_code));
    logger.log("Created CondBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" +
               std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates AssignBlock and adds it to the graph. Returns its index in configs.
int MemoryManager::add_assign_block(const std::string& val, const std::string& val_idx) {
    graph.push_back(std::make_shared<AssignBlock>(id_counter, val, val_idx, expr_buffor.back(), &asm_code));
    logger.log("Created AssignBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" +
               std::to_string(configs.size()));
    id_counter++;
    if (expr_buffor.back().get_operator() == ExprOperatorType::_MUL) {
        used_op[0] = true;
    } else if (expr_buffor.back().get_operator() == ExprOperatorType::_DIV) {
        used_op[1] = true;
    } else if (expr_buffor.back().get_operator() == ExprOperatorType::_MOD) {
        used_op[2] = true;
    }
    return configs.size() - 1;
}

// Creates empty/proc_end block and adds it to the graph. Returns its index in configs.
int MemoryManager::add_keyword_block(Keyword type) {
    graph.push_back(std::make_shared<KeywordBlock>(id_counter, type, &asm_code));
    logger.log("Created KeywordBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" +
               std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates WRITE/READ block and adds it to the graph. Returns its index in configs.
int MemoryManager::add_keyword_block(Keyword type, const std::string& val, const std::string& val_idx) {
    graph.push_back(std::make_shared<KeywordBlock>(id_counter, type, val, val_idx, &asm_code));
    logger.log("Created KeywordBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" +
               std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

int MemoryManager::add_proc_call(const std::string& name, std::vector<std::string> args) {
    graph.push_back(std::make_shared<ProcedureCall>(id_counter, name, args, &asm_code));
    logger.log("Created ProcedureCall: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" +
               std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates Expression and adds it to the buffor. Returns its index.
int MemoryManager::add_expr_to_buffor(ExprOperatorType op, const std::string& val1, const std::string& val2, const std::string& idx1,
                                      const std::string& idx2) {
    expr_buffor.push_back(Expression(op, val1, val2, idx1, idx2));
    logger.log("Created " + expr_buffor.back().get_vals_to_logger() + "| expr_buffor.size=" + std::to_string(expr_buffor.size()));
    return expr_buffor.size() - 1;
}

void MemoryManager::clear_args_decl_buffor() {
    args_decl_buffor.clear();
}

void MemoryManager::add_val_to_buffor(ValType type, const std::string& name) {
    args_decl_buffor.push_back(Value(memory_counter, type, name, true));
    logger.log("|add_val_to_buffor| Created value template: " + args_decl_buffor.back().get_vals_to_logger() +
               "| args_decl_buffor.size=" + std::to_string(args_decl_buffor.size()));
    memory_counter++;
}

void MemoryManager::add_const(const std::string& num) {
    if (consts_map.find(num) == consts_map.end()) {
        global_consts.push_back(Value(memory_counter, ValType::_NUM, num));
        logger.log("|add_consts| Created NUM Value " + global_consts.back().get_vals_to_logger() + "| global_consts.size=" +
                   std::to_string(global_consts.size()));
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
    graph[comm_end]->if_else_end = true;

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
    graph[comm_end]->if_else_end = true;
    graph[else_end]->if_else_end = true;

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
    graph[comm_end]->if_else_end = true;

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
    graph[comm_begin]->repeat_begin = true;

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
    procedures.push_back(Procedure(memory_counter++));
    logger.log("procedures.size=" + std::to_string(procedures.size()));
    return procedures.size() - 1;
}

void MemoryManager::add_val_to_procedure(const std::string& proc_id, ValType type, const std::string& name, const std::string& size) {
    logger.log("|add_val_to_procedure| " + proc_id + ", " + name);
    int idx = std::stoi(proc_id);

    try {
        if (type == ValType::_ARR) {
            procedures[idx].add_local_arr(memory_counter, name, std::stoi(size));
            memory_counter += std::stoi(size);
        } else {
            procedures[idx].add_local_val(memory_counter, name);
            memory_counter++;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "ERROR: " << e.what() << " (l. " << std::to_string(line) << ")" << std::endl;
        exit(-1);
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
    return procedures[proc_id].get_val_id(name, type, line);
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

void MemoryManager::resolve_jumps() {
    std::vector<int> ins_to_resolve;
    asm_code.get_ins_to_complete(ins_to_resolve);
    logger.log("|resolve_jumps| ins_to_resolve.size()=" + std::to_string(ins_to_resolve.size()));
    for (auto i : ins_to_resolve) {
        asm_code.complete_jump(i, get_next_block_k(asm_code.get_block_id(i), asm_code.get_where_jump_type(i)));
    }
}

int MemoryManager::get_next_block_k(int id, bool next_type) { 
    // ! jesli dojdziemy w koncu do bloku pustego bedacego koncem procedury -> return -1, wtedy robimy sztucznie jump k+1
    logger.log("|get_next_block_k| id=" + std::to_string(id) + ", " + std::to_string(next_type));
    int curr = id;
    int nxt = next_type ? graph[id]->next_TRUE : graph[id]->next_FALSE;
    if (nxt == -1) {
        return -1;
    }
    while (graph[nxt]->first_instruction_k == -1) {
        curr = nxt;
        nxt = next_type ? graph[curr]->next_TRUE : graph[curr]->next_FALSE;
        if (nxt == -1) {
            logger.log("jump to empty block, k=-1");
            return -1;
        }
    }
    logger.log("next non-empty block id=" + std::to_string(nxt) + ", k=" + std::to_string(graph[nxt]->first_instruction_k));
    return graph[nxt]->first_instruction_k;
}

int MemoryManager::get_procedure_first_k(const int proc) {
    const int b = procedures[proc].get_head();
    return graph[b]->first_instruction_k;
}

int MemoryManager::get_procedure_id(const std::string& name, const int curr_proc) {
    if (procedures[curr_proc].get_name() == name) {
        throw std::runtime_error("rekurencja w procedurze " + procedures[curr_proc].get_name() + " (l. " + std::to_string(line) + ")");
    }
    for (int i = 0; i < curr_proc; i++) {
        if (procedures[i].get_name() == name)
            return i;
    }
    throw std::runtime_error("niezdefiniowana wczesniej procedura " + name + " (l. " + std::to_string(line) + ")");
}

/*----------------------------------------------------------------------------------------------*/

void MemoryManager::translate_block(std::shared_ptr<CodeBlock> block) {
    if (current_proc != block->procedure_num) {
        if (current_proc != -1)
            line += 2;
        line += 2;
        current_proc = block->procedure_num;
        if (procedures[current_proc].get_local_vals_size() != 0)
            line++;
    }
    block->set_depth(loop_depth);

    const std::string bt = block->block_type;
    if (bt == "KeywordBlock") {
        translate_keyword_block(block);
        if (block->if_else_end) {
            asm_code.jump_to_next_block(block->ID);
        }
    } else if (bt == "AssignBlock") {
        translate_assign_block(block);
        if (block->if_else_end) {
            asm_code.jump_to_next_block(block->ID);
        }
    } else if (bt == "CondBlock") {
        translate_cond_block(block);
    } else {
        translate_procedure_call(block);
    }
    line++;

    if (block->next_TRUE == -1) {
        const int p = block->procedure_num;
        if (p != procedures.size() - 1) {
            asm_code.create_const_in_reg(procedures[p].get_return_adress(), "a");
            asm_code.add("LOAD", "a");
            asm_code.add("INC", "a");
            asm_code.add("INC", "a");
            asm_code.add("INC", "a");
            asm_code.add("JUMPR", "a");
        } else {
            asm_code.add("HALT", "");
        }
    }
}

void MemoryManager::translate_keyword_block(std::shared_ptr<CodeBlock> block) {
    const std::string val = block->get_val();
    const std::string val_idx = block->get_val_idx();
    const int proc = block->procedure_num;
    Keyword key = block->get_keyword();

    if (key == Keyword::_READ) {
        block->first_instruction_k = asm_code.get_k();
        if (val_idx == "") {
            asm_code.create_const_in_reg(get_val_id(val, ValType::_ID, proc), "b");
            if (procedures[proc].if_param(val, ValType::_ID)) {
                asm_code.indirect_load_put("b");
            } else {
                procedures[proc].initialize(val, ValType::_ID);
            }
            asm_code.add("READ", "", "# read " + val);
            asm_code.add("STORE", "b");
        } else {
            if (is_num(val_idx)) {
                asm_code.place_id_in_ra_idx_num(get_val_id(val, ValType::_ARR, proc), std::stoll(val_idx),
                                                procedures[proc].if_param(val, ValType::_ARR));
            } else {
                int idx_id = get_val_id(val_idx, ValType::_ID, proc);
                asm_code.place_id_in_ra(get_val_id(val, ValType::_ARR, proc), idx_id, procedures[proc].if_param(val, ValType::_ARR),
                                        procedures[proc].if_param(val_idx, ValType::_ID));
            }
            asm_code.add("PUT", "b");
            asm_code.add("READ", "", "# read " + val + "[" + val_idx + "]");
            asm_code.add("STORE", "b");
        }
    } else if (key == Keyword::_WRITE) {
        block->first_instruction_k = asm_code.get_k();
        if (val_idx == "") {
            if (is_num(val)) {
                asm_code.create_const_in_reg(std::stoll(val), "a");
            } else {
                int idx_id = get_val_id(val, ValType::_ID, proc);
                asm_code.create_const_in_reg(idx_id, "d");
                if (procedures[proc].if_param(val, ValType::_ID)) {
                    asm_code.indirect_load_put("d");
                } else {
                    check_for_errors(val, ValType::_ID, proc);
                }
                asm_code.add("LOAD", "d");
            }
            asm_code.add("WRITE", "");
        } else {
            if (is_num(val_idx)) {
                asm_code.place_id_in_ra_idx_num(get_val_id(val, ValType::_ARR, proc), std::stoll(val_idx),
                                                procedures[proc].if_param(val, ValType::_ARR));
            } else {
                int idx_id = get_val_id(val_idx, ValType::_ID, proc);
                asm_code.place_id_in_ra(get_val_id(val, ValType::_ARR, proc), idx_id, procedures[proc].if_param(val, ValType::_ARR),
                                        procedures[proc].if_param(val_idx, ValType::_ID));
                check_for_errors(val_idx, ValType::_ID, proc);
            }
            asm_code.add("LOAD", "a");
            asm_code.add("WRITE", "");
        }
    } else {
        block->first_instruction_k = asm_code.get_k();
        asm_code.add("JUMP", std::to_string(block->first_instruction_k + 1), "# _empty");
        loop_depth--;
    }
}

void MemoryManager::translate_assign_block(std::shared_ptr<CodeBlock> block) {
    logger.log("|translate_assign_block| block ID=" + std::to_string(block->ID));
    block->first_instruction_k = asm_code.get_k();
    const int proc = block->procedure_num;

    Expression* expr = block->get_expression();
    const bool opt = expr->is_special_case();
    if (opt) {
        logger.log("possible optimization");
        handle_special_case(expr, proc);
    } else if (val_to_reuse(block, expr) == -1) {
        place_expr_values_in_rb_rc(expr->get_val1(), expr->get_val1_idx(), expr->get_val2(), expr->get_val2_idx(), proc);
    } else {
        place_expr_values_in_rb_rc_reuse(expr->get_val1(), expr->get_val1_idx(), expr->get_val2(), expr->get_val2_idx(),
                                         proc, val_to_reuse(block, expr));
    }
    check_for_errors_help(expr->get_val1(), expr->get_val1_idx(), expr->get_val2(), expr->get_val2_idx(), proc);

    bool reuse_g = false;
    if (block->ID >= 1 && graph[block->ID-1]->block_type == "AssignBlock" && block->get_depth() == graph[block->ID-1]->get_depth() &&
        !graph[block->ID-1]->if_else_end && !block->repeat_begin && block->procedure_num == graph[block->ID-1]->procedure_num &&
        block->get_val() == graph[block->ID-1]->get_val() && block->get_val_idx() == graph[block->ID-1]->get_val_idx()) {
        reuse_g = true;
    }

    const std::string val = block->get_val();
    const std::string val_idx = block->get_val_idx();
    if (!reuse_g) {
        if (val_idx == "") {
            int idx_id = get_val_id(val, ValType::_ID, proc);
            asm_code.create_const_in_reg(idx_id, "g");
            if (procedures[proc].if_param(val, ValType::_ID)) {
                logger.log(val + " is param - call indirect_load_put()");
                asm_code.indirect_load_put("g");
            } else {
                procedures[proc].initialize(val, ValType::_ID);
            }
        } else {
            if (is_num(val_idx)) {
                asm_code.place_id_in_ra_idx_num(get_val_id(val, ValType::_ARR, proc), std::stoll(val_idx),
                                                procedures[proc].if_param(val, ValType::_ARR));
            } else {
                int idx_id = get_val_id(val_idx, ValType::_ID, proc);
                check_for_errors(val_idx, ValType::_ID, proc);
                asm_code.place_id_in_ra(get_val_id(val, ValType::_ARR, proc), idx_id, procedures[proc].if_param(val, ValType::_ARR),
                                        procedures[proc].if_param(val_idx, ValType::_ID));
            }
            asm_code.add("PUT", "g");
        }
    }

    if (opt) {
        asm_code.add("GET", "b");
        asm_code.add("STORE", "g", "# " + val + ":=");
        return;
    }

    switch (expr->get_operator()) {  // b,c used!
    case ExprOperatorType::_NOOP:
        asm_code.add("GET", "b");
        asm_code.add("STORE", "g", "# " + val + ":=");
        break;
    case ExprOperatorType::_PLUS:
        asm_code.add("GET", "b");
        asm_code.add("ADD", "c", "# plus");
        asm_code.add("STORE", "g", "# " + val + ":=");
        break;
    case ExprOperatorType::_MIN:
        asm_code.add("GET", "b");
        asm_code.add("SUB", "c", "# min");
        asm_code.add("STORE", "g", "# " + val + ":=");
        break;
    case ExprOperatorType::_MUL:  // d
        asm_code.add("STRK", "h", "# store k in h");
        asm_code.add("JUMP", std::to_string(asm_code.mul_k), "# multiply");
        asm_code.add("GET", "d");
        asm_code.add("STORE", "g", "# " + val + ":=");
        break;
    case ExprOperatorType::_DIV:  // d
        asm_code.add("STRK", "h", "# store k in h");
        asm_code.add("JUMP", std::to_string(asm_code.div_k), "# divide");
        asm_code.add("GET", "d");
        asm_code.add("STORE", "g", "# " + val + ":=");
        break;
    default:  // mod
        asm_code.add("STRK", "h", "# store k in h");
        asm_code.add("JUMP", std::to_string(asm_code.mod_k), "# modulo");
        asm_code.add("GET", "b");
        asm_code.add("STORE", "g", "# " + val + ":=");
        break;
    }
}

void MemoryManager::place_expr_val_in_r(const std::string& val, const std::string& val_idx, const int proc_num, const std::string& r) {
    logger.log("|place_expr_val_in_rb| " + val + ", " + val_idx);
    if (val_idx == "") {
        if (is_num(val)) {
            asm_code.create_const_in_reg(std::stoll(val), r);
        } else {
            int idx_id = get_val_id(val, ValType::_ID, proc_num);
            asm_code.create_const_in_reg(idx_id, "a");
            if (procedures[proc_num].if_param(val, ValType::_ID)) {
                logger.log(val + " is param - call indirect_load_put()");
                asm_code.indirect_load_put("a");
            }
            asm_code.add("LOAD", "a");
            asm_code.add("PUT", r);
        }
    } else {
        //change id in ra
        bool is_val_param = procedures[proc_num].if_param(val, ValType::_ARR);
        bool is_idx_param = procedures[proc_num].if_param(val_idx, ValType::_ID);
        logger.log("if params: " + val + " " + std::to_string(is_val_param) + ", " + val_idx + " " + std::to_string(is_idx_param));

        if (is_num(val_idx)) {
            asm_code.place_id_in_ra_idx_num(get_val_id(val, ValType::_ARR, proc_num), std::stoll(val_idx), is_val_param);
            asm_code.add("LOAD", "a");
            asm_code.add("PUT", r);
        } else {
            int idx_id = get_val_id(val_idx, ValType::_ID, proc_num);
            asm_code.place_id_in_ra(get_val_id(val, ValType::_ARR, proc_num), idx_id, is_val_param, is_idx_param);  // uses e,f
            asm_code.add("LOAD", "a");
            asm_code.add("PUT", r);
        }
    }
}

void MemoryManager::place_expr_values_in_rb_rc(const std::string& val1, const std::string& val1_idx, const std::string& val2,
                                               const std::string& val2_idx, const int proc_num) {
    logger.log("|place_expr_values_in_rb_rc| " + val1 + ", " + val1_idx + ", " + val2 + ", " + val2_idx);

    place_expr_val_in_r(val1, val1_idx, proc_num, "b");
    if (val2 == "")  // _NOOP
        return;

    if (val1 == val2 && val1_idx == val2_idx) {
        asm_code.add("PUT", "c");
        return;
    }
    place_expr_val_in_r(val2, val2_idx, proc_num, "c");
}

void MemoryManager::place_expr_values_in_rb_rc_reuse(const std::string& val1, const std::string& val1_idx, const std::string& val2,
                                                     const std::string& val2_idx, const int proc_num, const int to_reuse) {
    if (to_reuse == 1) {
        asm_code.add("PUT", "b");
        if (val2 == "")
            return;
        if (val1 == val2 && val1_idx == val2_idx) {
            asm_code.add("PUT", "c");
            return;
        } else {
            place_expr_val_in_r(val2, val2_idx, proc_num, "c");
        }
    } else {
        asm_code.add("PUT", "c");
        place_expr_val_in_r(val1, val1_idx, proc_num, "b");
    }
}

void MemoryManager::translate_cond_block(std::shared_ptr<CodeBlock> block) {
    logger.log("|translate_cond_block| block ID=" + std::to_string(block->ID));
    block->first_instruction_k = asm_code.get_k();

    const std::string val1 = block->get_val();
    const std::string val1_idx = block->get_val_idx();
    const std::string val2 = block->get_val2();
    const std::string val2_idx = block->get_val2_idx();
    bool is_c_zero = (val2 == "0");
    if (is_c_zero) {
        place_expr_val_in_r(val1, val1_idx, block->procedure_num, "b");
    } else {
        place_expr_values_in_rb_rc(val1, val1_idx, val2, val2_idx, block->procedure_num);
    }

    switch (block->get_cond()) {  // b,c used
    case CondOperatorType::_EQ:
        if (is_c_zero) {
            asm_code.cond_eq_zero(block->ID);
        } else {
            asm_code.cond__eq(block->ID);
        }
        break;
    case CondOperatorType::_NEQ:
        if (is_c_zero) {
            asm_code.cond_neq_zero(block->ID);
        } else {
            asm_code.cond__neq(block->ID);
        }
        break;
    case CondOperatorType::_LLEQ:
        if (is_c_zero) {
            asm_code.create_const_in_reg(0, "c");
        }
        asm_code.cond__lleq("b", "c", block->ID);
        break;
    case CondOperatorType::_LMEQ:
        if (is_c_zero) {
            asm_code.create_const_in_reg(0, "c");
        }
        asm_code.cond__lleq("c", "b", block->ID);
        break;
    case CondOperatorType::_LLESS:
        if (is_c_zero) {
            asm_code.create_const_in_reg(0, "c");
        }
        asm_code.cond__lless("b", "c", block->ID);
        break;
    case CondOperatorType::_LMORE:
        if (is_c_zero) {
            asm_code.cond_neq_zero(block->ID);
        } else {
            asm_code.cond__lless("c", "b", block->ID);
        }
        break;
    }
    loop_depth++;
}

void MemoryManager::translate_procedure_call(std::shared_ptr<CodeBlock> block) {
    logger.log("|translate_procedure_call| block ID=" + std::to_string(block->ID));
    block->first_instruction_k = asm_code.get_k();

    std::string name = block->get_proc_name();
    std::vector<std::string>* params = block->get_params();
    int proc_id = get_procedure_id(name, block->procedure_num);
    std::vector<std::pair<ValType, int> >* params_info = procedures[proc_id].params_info();
    logger.log("procedure name = " + name + ", params size=" + std::to_string(params->size()) +
               ", proc_id=" + std::to_string(proc_id) + ", params_info size=" + std::to_string(params_info->size()));

    if (params_info->size() != params->size()) {
        throw std::runtime_error("zla liczba argumentow w wywolaniu procedury (l. " + std::to_string(line) + ")");
    }

    for (int i = 0; i < params->size(); i++) {
        int id;
        try {
            id = get_val_id((*params)[i], (*params_info)[i].first, block->procedure_num);
        } catch (const std::runtime_error& e) {
            std::cerr << "ERROR: niewlasciwe parametry w wywolaniu procedury " + name +
                         " (l. " + std::to_string(line) + ")" << std::endl;
            exit(-1);
        }
        logger.log("put id=" + std::to_string(id) + +"(" + (*params)[i] + ") in p_id=" +
                   std::to_string((*params_info)[i].second));
        asm_code.create_const_in_reg(id, "a");
        if (procedures[block->procedure_num].if_param((*params)[i], (*params_info)[i].first)) {
            logger.log((*params)[i] + " is param");
            asm_code.indirect_load_put("a");
        } else {
            procedures[block->procedure_num].initialize((*params)[i], (*params_info)[i].first);
        }
        if (i == 0) {
            asm_code.store_ra_in_p((*params_info)[i].second);  // ?
        } else {
            asm_code.add("INC", "g");
            asm_code.add("STORE", "g");
        }
    }
    // load k to p with return adress
    asm_code.create_const_in_reg(procedures[proc_id].get_return_adress(), "b");
    asm_code.add("STRK", "a");
    asm_code.add("STORE", "b");
    asm_code.add("JUMP", std::to_string(graph[procedures[proc_id].get_head()]->first_instruction_k), "# jump to procedure");
}

void MemoryManager::handle_special_case(Expression* expr, const int proc_num) {
    switch (expr->get_operator()) {
    case ExprOperatorType::_MIN:
        place_expr_val_in_r(expr->get_val1(), expr->get_val1_idx(), proc_num, "b");
        for (int i = 1; i <= std::stoi(expr->get_val2()); i++) {
            asm_code.add("DEC", "b");
        }
        break;
    case ExprOperatorType::_PLUS:
        if (expr->get_val2() == "0" || expr->get_val2() == "1" || expr->get_val2() == "2" || expr->get_val2() == "3" ||
            expr->get_val2() == "4" || expr->get_val2() == "5") {
            place_expr_val_in_r(expr->get_val1(), expr->get_val1_idx(), proc_num, "b");
            for (int i = 1; i <= std::stoi(expr->get_val2()); i++) {
                asm_code.add("INC", "b");
            }
        } else {
            place_expr_val_in_r(expr->get_val2(), expr->get_val2_idx(), proc_num, "b");
            for (int i = 1; i <= std::stoi(expr->get_val1()); i++) {
                asm_code.add("INC", "b");
            }
        }
        break;
    case ExprOperatorType::_DIV:
        handle_special_div(expr, proc_num);
        break;
    case ExprOperatorType::_MUL:
        handle_special_mul(expr, proc_num);
        break;
    case ExprOperatorType::_MOD:
        place_expr_val_in_r(expr->get_val1(), expr->get_val1_idx(), proc_num, "b");
        asm_code.add("GET", "b");
        asm_code.add("SHR", "b");
        asm_code.add("SHL", "b");
        asm_code.add("SUB", "b");
        asm_code.add("PUT", "b");
        break;
    default:
        break;
    }
}

void MemoryManager::handle_special_div(Expression* expr, const int proc_num) {
    if (expr->get_val1() == expr->get_val2() && expr->get_val1_idx() == expr->get_val2_idx()) {
        if (expr->get_val1() == "0") {
            asm_code.create_const_in_reg(0, "b");
        } else {
            asm_code.create_const_in_reg(1, "b");
        }
        return;
    }
    place_expr_val_in_r(expr->get_val1(), expr->get_val1_idx(), proc_num, "b");
    if (expr->get_val2() == "1") {
        return;
    }
    for (int i = 1; i <= std::log2(std::stoll(expr->get_val2())); i++) {
        asm_code.add("SHR", "b");
    }
}

void MemoryManager::handle_special_mul(Expression* expr, const int proc_num) {
    if (expr->get_val1() == "0" || expr->get_val2() == "0") {
        asm_code.create_const_in_reg(0, "b");
        return;
    }
    if (expr->get_val1() == "1") {
        place_expr_val_in_r(expr->get_val2(), expr->get_val2_idx(), proc_num, "b");
    } else if (expr->get_val2() == "1") {
        place_expr_val_in_r(expr->get_val1(), expr->get_val1_idx(), proc_num, "b");
    } else if (expr->is_power_of_2(expr->get_val1())) {
        place_expr_val_in_r(expr->get_val2(), expr->get_val2_idx(), proc_num, "b");
        for (int i = 1; i <= std::log2(std::stoll(expr->get_val1())); i++) {
            asm_code.add("SHL", "b");
        }
    } else {
        place_expr_val_in_r(expr->get_val1(), expr->get_val1_idx(), proc_num, "b");
        for (int i = 1; i <= std::log2(std::stoll(expr->get_val2())); i++) {
            asm_code.add("SHL", "b");
        }
    }
}

int MemoryManager::val_to_reuse(std::shared_ptr<CodeBlock> block, Expression* expr) {
    if (block->ID >= 1 && graph[block->ID-1]->block_type == "AssignBlock" && block->get_depth() == graph[block->ID-1]->get_depth() &&
        !graph[block->ID-1]->if_else_end && !block->repeat_begin && block->procedure_num == graph[block->ID-1]->procedure_num) {
        if (graph[block->ID-1]->get_val() == expr->get_val1() && graph[block->ID-1]->get_val_idx() == expr->get_val1_idx()) {
            return 1;
        } else if (graph[block->ID-1]->get_val() == expr->get_val2() && graph[block->ID-1]->get_val_idx() == expr->get_val2_idx()) {
            return 2;
        }
    }
    return -1;
}

// initialized?
void MemoryManager::check_for_errors(const std::string& name, ValType type, const int proc_num) {
    logger.log("|check_for_errors| " + name + " in " + std::to_string(proc_num));
    if (type != ValType::_ARR && !procedures[proc_num].if_param(name, type) && !procedures[proc_num].is_initialized(name, type)) {
        if (loop_depth >= 1) {
            std::cerr << "WARNING: zmienna " + name + " moze byc niezainicjowana (l. " + std::to_string(line) + ")\n";
        } else {
            throw std::runtime_error("uzycie niezainicjowanej zmiennej " + name + " (l. " + std::to_string(line) + ")");
        }
    }
}

void MemoryManager::check_for_errors_help(const std::string& val1, const std::string& val1_idx, const std::string& val2,
                                          const std::string& val2_idx, const int proc_num) {
    if (val1_idx == "" && !is_num(val1)) {
        check_for_errors(val1, ValType::_ID, proc_num);
    } else if (!is_num(val1_idx)) {
        check_for_errors(val1_idx, ValType::_ID, proc_num);
    }
    if (val2 == "") {
        return;
    }
    if (val2_idx == "" && !is_num(val2)) {
        check_for_errors(val2, ValType::_ID, proc_num);
    } else if (!is_num(val2_idx)) {
        check_for_errors(val2_idx, ValType::_ID, proc_num);
    }
}

/*----------------------------------------------------------------------------------------------*/

void MemoryManager::translate() {
    const int first_jump = asm_code.get_k();
    asm_code.add("JUMP", "", "# przeskocz mul/div/mod i procedury");
    if (used_op[0]) {
        asm_code.asm_multiply();
    }
    if (used_op[1]) {
        asm_code.asm_divide();
    }
    if (used_op[2]) {
        asm_code.asm_modulo();
    }
    for (auto g : graph)
        translate_block(g);
    jump_to_main(first_jump);
    resolve_jumps();
}

void MemoryManager::save_asm_to_file(const char* filename) {
    asm_code.save_to_file(filename);
}

void MemoryManager::jump_to_main(const int i) {
    int k = graph[procedures.back().get_head()]->first_instruction_k;
    asm_code.complete_jump(i, k);
}
