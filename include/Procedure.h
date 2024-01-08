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

public:
    Procedure(const std::string& name) : procedure_name(name), map(), params(), local_vals() {}
    Procedure() : map(), params(), local_vals() {}

    void set_head(int id) {
        head_id = id;
    }

    int get_head() {
        return head_id;
    }

    void set_name(const std::string& p_name) {
        procedure_name = p_name;
    }

    void add_local_val(int id, const std::string& name);
    void add_local_arr(int id, const std::string& name, int size);
    void add_params_templates(std::vector<Value> p);
    void add_params_to_map();
    int get_val_id(const std::string& name, ValType type);
    bool if_param(const std::string& name, ValType type);

    void log_info();
};

#endif  // COMPILER_INCLUDE_PROCEDURE_H
