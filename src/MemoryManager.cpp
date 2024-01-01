#include "MemoryManager.h"
#include "AsmCode.h"

// Creates CondBlock and adds it to the graph. Returns its Configuration id (index in configs).
int MemoryManager::add_cond_block(CondOperatorType type, const std::string& val1, const std::string& val2, const std::string& val1_idx, const std::string& val2_idx) {
    graph.push_back(std::make_shared<CondBlock>(id_counter, type, val1, val2, val1_idx, val2_idx));
    logger.log("Created CondBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates AssignBlock and adds it to the graph. Returns its index in configs.
int MemoryManager::add_assign_block(const std::string& val, const std::string& val_idx) {
    graph.push_back(std::make_shared<AssignBlock>(id_counter, val, val_idx, expr_buffor.back()));
    logger.log("Created AssignBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates empty/proc_end block and adds it to the graph. Returns its index in configs.
int MemoryManager::add_keyword_block(Keyword type) {
    graph.push_back(std::make_shared<KeywordBlock>(id_counter, type));
    logger.log("Created KeywordBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates WRITE/READ block and adds it to the graph. Returns its index in configs.
int MemoryManager::add_keyword_block(Keyword type, const std::string& val, const std::string& val_idx) {
    graph.push_back(std::make_shared<KeywordBlock>(id_counter, type, val, val_idx));
    logger.log("Created KeywordBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + "," + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

int MemoryManager::add_proc_call(const std::string& name, std::vector<std::string> args) {
    graph.push_back(std::make_shared<ProcedureCall>(id_counter, name, args));
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

/*---------------------------------------*/

//void MemoryManager::
