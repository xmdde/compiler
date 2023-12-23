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
        p_logger.log("ERROR - value already in the map"); //change to error in bison
    }
}

void Procedure::add_local_arr(int id, const std::string& name, int size) {
    p_logger.log("|add_local_arr| Trying to add local array Value: ID=" + std::to_string(id) + ", name = " + name + ", size=" + std::to_string(size));

    if (map.find(std::make_pair(ValType::_ARR, name)) == map.end()) {
        local_vals.push_back(Value(id, name, size));
        p_logger.log("Created Value: " + local_vals.back().get_vals_to_logger() + ". | local_vals.size=" + std::to_string(local_vals.size()));

        map[std::make_pair(ValType::_ID, name)] = id;
        p_logger.log("Added ((_ARR," + name + "), " + std::to_string(id) + ") to map. | map.size=" + std::to_string(map.size()));
    } else {
        p_logger.log("ERROR - value already in the map"); //change to error in bison
    }
}

//przy arr - id_counter+=size !!
