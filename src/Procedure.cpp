#include "Procedure.h"
#include "Logger.h"

logging::Logger p_logger = logging::Logger("procedure_logs.log");

void Procedure::add_local_val(int id, const std::string& name) {
    p_logger.log("|add_local_val| Trying to add local Value: ID=" + std::to_string(id) + ", name = " + name);
    
    if (map.find(std::make_pair(ValType::_ID, name)) == map.end()) {
        local_vals.push_back(Value(id, ValType::_ID, name));
        p_logger.log("Created Value: " + local_vals.back().get_vals_to_logger() + ". | local_vals.size=" + std::to_string(local_vals.size()));

        map[std::make_pair(ValType::_ID, name)] = id;
        p_logger.log("Added ((_ID," + name + "), " + std::to_string(id) + ") to map. | map.size=" + std::to_string(map.size()));
    } else {
        p_logger.log("ERROR - value already in the map");  // change to error in bison
    }
}

void Procedure::add_params_to_map() {
    p_logger.log("|add_params_to_map| Trying to add params to map. params.size()=" + std::to_string(params.size()));
    for (int i = 0; i < params.size(); i++) {
        std::string t = (params[i].get_type() == ValType::_ARR) ? "_ARR" : "_ID";
        std::string name = params[i].get_name();
        int id = params[i].get_id();
        ValType type = params[i].get_type();

        p_logger.log(std::to_string(i) + "| ((" + t + "," + name + "), " + std::to_string(id) + ")");
        if (map.find(std::make_pair(type, name)) == map.end()) {
            map[std::make_pair(type, name)] = id;
            p_logger.log("Added to map. | map.size=" + std::to_string(map.size()));
        } else {
            p_logger.log("ERROR - value already in the map");  // change to error in bison / catch exceptions
        }
    }
}

void Procedure::add_local_arr(int id, const std::string& name, int size) {
    p_logger.log("|add_local_arr| Trying to add local array Value: ID=" + std::to_string(id) + 
                 ", name = " + name + ", size=" + std::to_string(size));

    if (map.find(std::make_pair(ValType::_ARR, name)) == map.end()) {
        local_vals.push_back(Value(id, name, size));
        p_logger.log("Created Value: " + local_vals.back().get_vals_to_logger() + ". | local_vals.size=" + std::to_string(local_vals.size()));

        map[std::make_pair(ValType::_ARR, name)] = id;
        p_logger.log("Added ((_ARR," + name + "), " + std::to_string(id) + ") to map. | map.size=" + std::to_string(map.size()));
    } else {
        p_logger.log("ERROR - value already in the map");  // change to error in bison
    }
}

void Procedure::add_params_templates(std::vector<Value> p) {
    p_logger.log("|add_params_templates| Trying to add a vector of parameters: size=" + std::to_string(p.size()));
    params = p;
    p_logger.log("params.size=" + std::to_string(params.size()));
    add_params_to_map();
}

int Procedure::get_val_id(const std::string& name, ValType type) {
    p_logger.log("|get_val_id| " + procedure_name + " | get " + name);
    if (type == ValType::_ID) {
        p_logger.log("szukam _ID");
    }
    int id = map.find(std::make_pair(type, name))->second; //p_logger.log("id is: " + std::to_string(id));
    return id;
}

bool Procedure::if_param(const std::string& name, ValType type) {
    for (auto p : params) {
        if (p.get_name() == name && p.get_type() == type)
            return true;
    }
    return false;
}

void Procedure::log_info() {
    p_logger.log("---------------------\nProcedure: name = " + procedure_name + ", head_id=" + std::to_string(head_id));
    std::string p = "";
    std::string v = "";
    for (auto val : local_vals) {
        v += val.get_vals_to_logger() + "\n";
    }
    for (auto val : params) {
        p += val.get_vals_to_logger() + "\n";
    }
    p_logger.log("| local_vals: size=" + std::to_string(local_vals.size()) + " |\n" + v +
                 "| params: size=" + std::to_string(params.size()) + " |\n" + p + "map.size=" + std::to_string(map.size()));
}
