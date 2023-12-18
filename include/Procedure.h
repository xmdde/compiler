#ifndef COMPILER_INCLUDE_PROCEDURE_H
#define COMPILER_INCLUDE_PROCEDURE_H

#include "Value.h"
#include <string>
#include <map>

class Procedure {
    std::string name;
    std::vector<Value> params;
    std::vector<Value> local_vals;
    std::map<std::pair<std::string, ValType>, int> map;

public:
    Procedure(std::string name) : name(name), map() {}
    //fun: find vals id by type and name
};

#endif  // COMPILER_INCLUDE_PROCEDURE_H
