#ifndef BDCA_ARGS_H
#define BDCA_ARGS_H

#include <vector>
#include <string>

struct arguments
{
    std::string input_file;
    std::string output_file;
    std::string plateform;
};

extern arguments parse_args(const std::vector<std::string>& args);
#endif /* BDCA_ARGS_H */