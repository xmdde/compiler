#ifndef COMPILER_INCLUDE_MEMORYMANAGER_H
#define COMPILER_INCLUDE_MEMORYMANAGER_H

#include "CodeBlock.h"
#include "Procedure.h"
#include "Value.h"
#include "Logger.h"
#include <stack>
#include <vector>
#include <string>
#include <map>

class Configuration {
public:
    int begin_id;
    int end_id;

    Configuration(int b, int e) : begin_id(b), end_id(e) {}

    std::string get_vals_to_logger() {
        return "begin_id=" + std::to_string(begin_id) + ", end_id=" + std::to_string(end_id);
    }
};

class MemoryManager {
    logging::Logger logger = logging::Logger("logs.log");

    int memory_counter = 0; //values IDs
    int curr_head = 0; //zmien przy end
    //registers?
    std::vector<std::shared_ptr<CodeBlock>> graph;
    std::vector<Value> memory;
    std::vector<Value> global_consts;
    std::vector<Procedure> procedures;
    std::map<std::string, int> consts_map;
    //map for var in procedure
    std::vector<Expression> expr_buffor;
    std::vector<Value> args_decl_buffor;

public:
    int id_counter = 0;
    std::stack<std::string> current_procedure; //push what and when
    std::vector<Configuration> configs;

    MemoryManager() : graph(), memory(), global_consts(), consts_map(), args_decl_buffor() {
        current_procedure = std::stack<std::string>();
        configs = std::vector<Configuration>();
        expr_buffor = std::vector<Expression>();
        logger.log("Created MemoryManager | graph.size=" + std::to_string(graph.size()));
    }
    //fun: find val's id in a given procedure by type and name

    // Create Block and add it to the graph. Return its Configuration id (index in configs).
    int add_cond_block(CondOperatorType type, std::string val1, std::string val2, std::string val1_idx, std::string val2_idx);
    int add_assign_block(const std::string& val, const std::string& val_idx);
    int add_keyword_block(Keyword type);
    int add_keyword_block(Keyword type, const std::string& val, const std::string& val_idx);
    int add_proc_call(const std::string& name, std::vector<std::string> args);

    // Create Expression and add it to the expr_buffor. Return its index.
    int add_expr_to_buffor(ExprOperatorType op, const std::string& val1, const std::string& val2, const std::string& idx1, const std::string& idx2);
    void add_val_to_buffor(ValType type, const std::string& name);
    void clear_args_decl_buffor();
    void add_const(const std::string& num);

    // Connect subgraphs based on given configs (1->2 true and false). Return new Configuration's index.
    int connect_blocks(const std::string& config1_idx, const std::string& config2_idx);
    int connect_if(const std::string& cond_config, const std::string& commands_config);
    int connect_if_else(const std::string& cond_config, const std::string& commands_config, const std::string& else_config);
    int connect_while(const std::string& cond_config, const std::string& commands_config);
    int connect_repeat_until(const std::string& commands_config, const std::string& cond_config);

    int create_procedure();
    void add_val_to_procedure(const std::string& proc_id, ValType type, const std::string& name, const std::string& size);
    void export_ast();
};

#endif  // COMPILER_INCLUDE_MEMORYMANAGER_H
