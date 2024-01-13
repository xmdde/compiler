#ifndef COMPILER_INCLUDE_PROCEDURE_H
#define COMPILER_INCLUDE_PROCEDURE_H

#include <string>
#include <map>

#include "Value.h"

class Procedure {
    int head_id;
    std::string procedure_name;
    std::vector<Value> params;
    std::vector<Value> local_vals;
    std::map<std::pair<ValType, std::string>, int> map;
    int return_adress;

public:
    Procedure(int id) : map(), params(), local_vals(), return_adress(id) {}

    void set_head(int id) {
        head_id = id;
    }

    int get_head() {
        return head_id;
    }

    void set_name(const std::string& p_name) {
        procedure_name = p_name;
    }

    std::string get_name() {
        return procedure_name;
    }

    int get_return_adress() {
        return return_adress;
    }

    void add_local_val(int id, const std::string& name);
    void add_local_arr(int id, const std::string& name, int size);
    void add_params_templates(std::vector<Value> p);
    void add_params_to_map();

    int get_val_id(const std::string& name, ValType type);
    bool if_param(const std::string& name, ValType type);
    bool is_initialized(const std::string& name, ValType type);
    void initialize(const std::string& name, ValType type);
    Value* get_value(const std::string& name, ValType type);
    std::vector<std::pair<ValType, int> >* params_info();

    void log_info();
};

#endif  // COMPILER_INCLUDE_PROCEDURE_H
