#ifndef BCDA_CONFIG_H
#define BCDA_CONFIG_H

#include <map>
#include <string>

extern std::map<std::string, std::string> read_config_file(std::string path);

#endif /* BCDA_CONFIG_H */