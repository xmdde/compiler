#include "MemoryManager.h"

// Creates CondBlock and adds it to the graph. Returns its Configuration id (index in configs).
int MemoryManager::add_cond_block(CondOperatorType type, std::string val1, std::string val2, std::string val1_idx, std::string val2_idx) {
    graph.push_back(std::make_shared<CondBlock>(id_counter, type, val1, val2, val1_idx, val2_idx));
    logger.log("Created CondBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + " " + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates AssignBlock and adds it to the graph. Returns its index in configs.
int MemoryManager::add_assign_block(const std::string& val, const std::string& val_idx) {
    graph.push_back(std::make_shared<AssignBlock>(id_counter, val, val_idx, expr_buffor.back()));
    logger.log("Created AssignBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + " " + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates empty/proc_end block and adds it to the graph. Returns its index in configs.
int MemoryManager::add_keyword_block(Keyword type) {
    graph.push_back(std::make_shared<KeywordBlock>(id_counter, type));
    logger.log("Created KeywordBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + " " + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates WRITE/READ block and adds it to the graph. Returns its index in configs.
int MemoryManager::add_keyword_block(Keyword type, const std::string& val, const std::string& val_idx) {
    graph.push_back(std::make_shared<KeywordBlock>(id_counter, type, val, val_idx));
    logger.log("Created KeywordBlock: " + graph.back()->get_vals_to_logger() + "| graph.size=" + std::to_string(graph.size()));
    configs.push_back(Configuration(id_counter, id_counter));
    logger.log("Created Configuration (" + std::to_string(id_counter) + " " + std::to_string(id_counter) + ") | configs.size=" + std::to_string(configs.size()));
    id_counter++;
    return configs.size() - 1;
}

// Creates Expression and adds it to the buffor. Returns its index.
int MemoryManager::add_expr_to_buffor(ExprOperatorType op, const std::string& val1, const std::string& val2, const std::string& idx1, const std::string& idx2) {
    expr_buffor.push_back(Expression(op, val1, val2, idx1, idx2));
    logger.log("Created " + expr_buffor.back().get_vals_to_logger() + "| expr_buffor.size=" + std::to_string(expr_buffor.size()));
    return expr_buffor.size() - 1;
}

void MemoryManager::add_const(const std::string& num) {
    if (consts_map.find(num) == consts_map.end()) {
        global_consts.push_back(Value(memory_counter, ValType::_NUM, num));
        logger.log("Created NUM Value " + global_consts.back().get_vals_to_logger() + "| global_consts.size=" + std::to_string(global_consts.size()));
        consts_map[num] = memory_counter;
        logger.log("Added (" + num + "," + std::to_string(memory_counter)+") to the consts map.");
        memory_counter++;
    } else {
        logger.log("NUM Value " + num + " already found in the map.");
    }
}

int MemoryManager::connnect_blocks(const std::string& config1_idx, const std::string& config2_idx) {
    logger.log("|connect_blocks|Configuration1 idx=" + config1_idx + ", Configuration2 idx =" + config2_idx);
    int idx1 = std::stoi(config1_idx);
    int idx2 = std::stoi(config2_idx);
    logger.log("Configuration1: " + configs[idx1].get_vals_to_logger() + ", Configuration2: " + configs[idx2].get_vals_to_logger());
    
    return 0;
}