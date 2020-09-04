#ifndef BDCA_MODULES_H
#define BDCA_MODULES_H

#include <bdc_assembler/core/parser.hpp>
#include <string>
#include <vector>
#include <map>

#define MODULES_DIR "modules/"

struct module
{
    void* lib_handle;
    uint uid;
    std::map<std::string, std::string> config;
    std::string module_folder;
    std::string lib_file;
    std::string plateform;

    std::vector<char> (*fp_get_raw_bytes)(const std::vector<line_data>& v);
};

extern void load_modules();
extern std::vector<char> call_module(const std::string& plateform, const std::vector<line_data>& v);
extern void unload_modules();

#endif /* BDCA_MODULES_H */