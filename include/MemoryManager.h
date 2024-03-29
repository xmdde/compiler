#ifndef COMPILER_INCLUDE_MEMORYMANAGER_H
#define COMPILER_INCLUDE_MEMORYMANAGER_H

#include <stack>
#include <vector>
#include <string>
#include <map>

#include "CodeBlock.h"
#include "Procedure.h"
#include "Value.h"
#include "Logger.h"
#include "AsmCode.h"

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
    AsmCode asm_code;

    int memory_counter = 0;  // values' IDs counter
    std::vector<std::shared_ptr<CodeBlock> > graph;

    std::vector<Value> global_consts;
    std::vector<Procedure> procedures;
    std::map<std::string, int> consts_map;

    std::vector<Expression> expr_buffor;
    std::vector<Value> args_decl_buffor;
    std::vector<bool> used_op;

    int loop_depth = 0;
    int line = 1;
    int current_proc = -1;
public:
    int id_counter = 0;
    std::vector<Configuration> configs;

    MemoryManager() : graph(), global_consts(), consts_map(), args_decl_buffor(), asm_code() {
        configs = std::vector<Configuration>();
        expr_buffor = std::vector<Expression>();
        used_op = std::vector<bool>(3, false);
        logger.log("Created MemoryManager | graph.size=" + std::to_string(graph.size()));
    }

    int add_cond_block(CondOperatorType type, const std::string& val1, const std::string& val2, const std::string& val1_idx,
                       const std::string& val2_idx);
    int add_assign_block(const std::string& val, const std::string& val_idx);
    int add_keyword_block(Keyword type);
    int add_keyword_block(Keyword type, const std::string& val, const std::string& val_idx);
    int add_proc_call(const std::string& name, std::vector<std::string> args);
    int add_expr_to_buffor(ExprOperatorType op, const std::string& val1, const std::string& val2, const std::string& idx1,
                           const std::string& idx2);
    void add_val_to_buffor(ValType type, const std::string& name);
    void clear_args_decl_buffor();
    void add_const(const std::string& num);

    int connect_blocks(const std::string& config1_idx, const std::string& config2_idx);
    int connect_if(const std::string& cond_config, const std::string& commands_config);
    int connect_if_else(const std::string& cond_config, const std::string& commands_config, const std::string& else_config);
    int connect_while(const std::string& cond_config, const std::string& commands_config);
    int connect_repeat_until(const std::string& commands_config, const std::string& cond_config);

    int create_procedure();
    void add_val_to_procedure(const std::string& proc_id, ValType type, const std::string& name, const std::string& size);
    void set_procedure_name(const std::string& proc_id, const std::string& name);
    void set_procedure_head(const std::string& proc_id, const std::string& conf);

    int get_val_id(const std::string& name, ValType type, const int proc_id);
    int get_const_id(const std::string& num);
    int get_next_block_k(int id, bool next_type);
    int get_procedure_first_k(const int proc);
    int get_procedure_id(const std::string& name, const int curr_proc);

    void set_procedures_in_graph();
    void log_procedures_info();
    void export_ast();

    void translate_block(std::shared_ptr<CodeBlock> block);
    void translate_keyword_block(std::shared_ptr<CodeBlock> block);
    void translate_assign_block(std::shared_ptr<CodeBlock> block);
    void translate_cond_block(std::shared_ptr<CodeBlock> block);
    void translate_procedure_call(std::shared_ptr<CodeBlock> block);

    void check_for_errors(const std::string& name, ValType type, const int proc_num);
    void check_for_errors_help(const std::string& val1, const std::string& val1_idx, const std::string& val2,
                               const std::string& val2_idx, const int proc_num);
    void place_expr_values_in_rb_rc(const std::string& val1, const std::string& val1_idx, const std::string& val2,
                                    const std::string& val2_idx, const int proc_num);
    void place_expr_val_in_r(const std::string& val, const std::string& val_idx, const int proc_num, const std::string& r);
    void place_expr_values_in_rb_rc_reuse(const std::string& val1, const std::string& val1_idx, const std::string& val2,
                                          const std::string& val2_idx, const int proc_num, const int to_reuse);

    void handle_special_case(Expression* expr, const int proc_num);
    void handle_special_div(Expression* expr, const int proc_num);
    void handle_special_mul(Expression* expr, const int proc_num);
    int val_to_reuse(std::shared_ptr<CodeBlock> block, Expression* expr);

    void resolve_jumps();
    void jump_to_main(const int i);
    void translate();
    void save_asm_to_file(const char* filename);
};

#endif  // COMPILER_INCLUDE_MEMORYMANAGER_H
