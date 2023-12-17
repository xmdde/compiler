#ifndef COMPILER_INCLUDE_PROCEDURE_H
#define COMPILER_INCLUDE_PROCEDURE_H

#include "Value.h"
#include <string>

class Procedure {
    std::string name;
    std::vector<Value> params;
    std::vector<Value> local_vals;

public:
    Procedure(std::string name) : name(name) {}
    //fun: find vals id by type and name
};

#endif  // COMPILER_INCLUDE_PROCEDURE_H
