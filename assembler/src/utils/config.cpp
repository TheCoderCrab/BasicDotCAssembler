#include <bdc_assembler/utils/config.hpp>
#include <bdc_assembler/utils/log.hpp>
#include <fstream>


std::map<std::string, std::string> read_config_file(std::string path)
{
    std::map<std::string, std::string> cfg;
    std::ifstream file(path);
    std::string line;
    if(file)
    {
        while(std::getline(file, line))
        {
            line.erase(std::remove_if(line.begin(), line.end(), isspace),
                                 line.end());
            if(line[0] == '#' || line.empty())
                continue;
            std::size_t p = line.find("=");
            std::string name = line.substr(0, p);
            std::string value = line.substr(p + 1);
            cfg.insert_or_assign(name, value);
        }
    }
    else
        WARN_M("Cannot open config file: " << path);
    return cfg;
}