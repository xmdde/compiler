#ifndef COMPILER_INCLUDE_PROCEDURE_H
#define COMPILER_INCLUDE_PROCEDURE_H

#include <string>
#include <map>

#include "Value.h"

class Procedure {
    int head_id;
    std::string name;
    std::vector<Value> params;
    std::vector<Value> local_vals;
    std::map<std::pair<ValType, std::string>, int> map;

public:
    Procedure(const std::string& name) : name(name), map(), params(), local_vals() {}
    Procedure() : map(), params(), local_vals() {}

    void set_head(int id) {
        head_id = id;
    }

    void set_name(const std::string& p_name) {
        name = p_name;
    }

    void add_local_val(int id, const std::string& name);
    void add_local_arr(int id, const std::string& name, int size);
    void add_params_templates(std::vector<Value> p);
    // void fill_in_params_info(); tutaj lapanie bledow xd + add to map
    int get_val_id(const std::string& name, ValType type);

    void log_info();
};

#endif  // COMPILER_INCLUDE_PROCEDURE_H
