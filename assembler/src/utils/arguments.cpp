#include <bdc_assembler/core/arguments.hpp>
#include <sstream>
#include <utility>
#include <cstdint>
#include <bdc_assembler/utils/log.hpp>



static std::pair<std::string, std::string> get_arg_data(const std::string& arg)
{
    std::stringstream ss;
    std::pair<std::string, std::string> p;
    for(char c : arg)
    {
        if(c == '=')
        {
            p.first = ss.str();
            ss.str("");
            continue;
        }
        ss << c;
    }
    p.second = ss.str();
    return p;
}

static uint char_count(const std::string& s, char c)
{
    uint cn = 0;    
    for(char ch : s)
        if(ch == c)
            cn++;
    return cn;
}

arguments parse_args(const std::vector<std::string>& args)
{
    arguments parsed_args{ .input_file = "in.bcd", 
                           .output_file = "out.dce",
                           .plateform = "flat" };
    for(std::string arg : args)
    {
        if(char_count(arg, '=') == 1)
        {
            std::pair<std::string, std::string> p = get_arg_data(arg);
            if(p.first == "plateform")
                parsed_args.plateform = p.second;
            else if(p.first == "if")
                parsed_args.input_file = p.second;
            else if(p.first == "of")
                parsed_args.output_file = p.second;
        }
        else
        {
            ERR_M("Syntax error in argument: " << arg);
            exit(EXIT_FAILURE);
        }
    }
    return parsed_args;
}