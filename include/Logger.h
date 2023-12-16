#ifndef COMPILER_INCLUDE_LOGGER_H
#define COMPILER_INCLUDE_LOGGER_H

#include <fstream>
#include <iostream>
#include <vector>
#include "CodeBlock.h"

namespace logging {

class Logger {
    std::vector<std::string> log_list;
    std::string file_name;
    std::ofstream output;
public:
    Logger(std::string filename) : file_name(filename) {
        output.open(file_name);
    }

    void log(const std::string& msg) {
        output << msg << std::endl;
    }

    void log(const std::string& msg, int param) {
        output << msg << param << std::endl;
    }

    void log(CondBlock* condBlock) {
        output << "CondBlock (ID: " << condBlock->ID << ", " << condBlock->get_vals_to_logger() << ")" << std::endl;
    }

    void log(AssignBlock* assignBlock) {
    }

    void close_logger() {
        for (auto it : log_list) {
            std::cout << it << std::endl;
        }
        output.close();
    }

    ~Logger() {
        output.close();
    }
};
}

#endif  // COMPILER_LOGGER_H
