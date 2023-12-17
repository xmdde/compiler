#ifndef COMPILER_INCLUDE_MEMORYMANAGER_H
#define COMPILER_INCLUDE_MEMORYMANAGER_H

#include "CodeBlock.h"
#include "Value.h"
#include "Logger.h"
#include <stack>
#include <vector>
#include <string>

auto logger = logging::Logger("logs.log");

class Configuration {
public:
    int begin_id;
    int end_id;

    Configuration(int b, int e) : begin_id(b), end_id(e) {}
};

class MemoryManager {
    //registers
    std::vector<std::shared_ptr<CodeBlock>> graph;
    std::vector<Value> memory;
    std::vector<Value> global_consts;
    //procedures
    //map for consts
    //map for var in procedure

public:
    int id_counter = 0;
    std::stack<std::string> current_procedure; //push what and when
    std::vector<Configuration> configs;
    std::vector<Expression> expr_buffor;

    MemoryManager() : graph(), memory(), global_consts() {
        current_procedure = std::stack<std::string>();
        configs = std::vector<Configuration>();
        expr_buffor = std::vector<Expression>();
        logger.log("Created MemoryManager - graph.size=" + std::to_string(graph.size()));
    }
    //fun: find val's id in a given procedure by type and name

    // Creates CondBlock and adds it to the graph. Returns its Configuration id (index in configs).
    int add_cond_block(CondOperatorType type, std::string val1, std::string val2, std::string val1_idx, std::string val2_idx) {
        graph.push_back(std::make_shared<CondBlock>(id_counter, type, val1, val2, val1_idx, val2_idx));
        logger.log("Created CondBlock: " + graph.back()->get_vals_to_logger() + "- graph.size=" + std::to_string(graph.size()));
        configs.push_back(Configuration(id_counter, id_counter));
        logger.log("Created Configuration (" + std::to_string(id_counter) + " " + std::to_string(id_counter) + "), - configs.size=" + std::to_string(configs.size()));
        id_counter++;
        return configs.size() - 1;
    }

    int add_expr_to_buffor(ExprOperatorType op, std::string val1, std::string val2, std::string idx1, std::string idx2) {
        expr_buffor.push_back(Expression(op, val1, val2, idx1, idx2));
        logger.log("Created " + expr_buffor.back().get_vals_to_logger() + ", expr_buffor.size=" + std::to_string(expr_buffor.size()));
        return expr_buffor.size() - 1;
    }
};

#endif  // COMPILER_INCLUDE_MEMORYMANAGER_H
