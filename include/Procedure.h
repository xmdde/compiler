#ifndef COMPILER_INCLUDE_PROCEDURE_H
#define COMPILER_INCLUDE_PROCEDURE_H

#include "Value.h"
#include <string>
#include <map>

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
    //void add_param_template();
    void log_info();
    //fun: find vals id by type and name
};

#endif  // COMPILER_INCLUDE_PROCEDURE_H
