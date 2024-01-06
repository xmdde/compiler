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

void Procedure::add_local_arr(int id, const std::string& name, int size) {
    p_logger.log("|add_local_arr| Trying to add local array Value: ID=" + std::to_string(id) + 
                 ", name = " + name + ", size=" + std::to_string(size));

    if (map.find(std::make_pair(ValType::_ARR, name)) == map.end()) {
        local_vals.push_back(Value(id, name, size));
        p_logger.log("Created Value: " + local_vals.back().get_vals_to_logger() + ". | local_vals.size=" + std::to_string(local_vals.size()));

        map[std::make_pair(ValType::_ID, name)] = id;
        p_logger.log("Added ((_ARR," + name + "), " + std::to_string(id) + ") to map. | map.size=" + std::to_string(map.size()));
    } else {
        p_logger.log("ERROR - value already in the map");  // change to error in bison
    }
}

void Procedure::add_params_templates(std::vector<Value> p) {
    p_logger.log("|add_params_templates| Trying to add a vector of parameters: size=" + std::to_string(p.size()));
    params = p;
    p_logger.log("params.size=" + std::to_string(params.size()));
}

int Procedure::get_val_id(const std::string& name, ValType type) {
    //p_logger.log("|get_val_id| " + procedure_name + " | get " + name);
    return map.find(std::make_pair(type, name))->second;
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
