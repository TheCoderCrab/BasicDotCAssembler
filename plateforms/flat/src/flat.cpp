#include <bdc_assembler/module.hpp>
#include <bdc_assembler/core/parser.hpp>
#include <bdc_assembler/utils/log.hpp>
#include <vector>

__export std::vector<char> module_get_raw_bytes(const std::vector<line_data>& v)
{
    std::vector<char> output;
    DEBUG_M("FLAT: " << v.size());
    for(line_data l : v)
    {
        output.push_back((char) l.instr);
        output.push_back(l.arg_desc);
        auto push_arg = [&] (arg& a)
        {
            if(a.activated)
            {
                DEBUG_M("Arg activated for instruction at line: " << l.line_num);
                if(a.mem)
                    return;
                if(a.reg || a.size == 1)
                {
                    DEBUG_M("it is a register");
                    output.push_back((uint8_t) a.ivalue);
                    return;
                }
                union
                {
                    uint i;
                    char ca[4];
                } i_ca;
                
                if(a.size == 2)
                {
                    i_ca.i = a.ivalue;
                    output.push_back(i_ca.ca[0]);
                    output.push_back(i_ca.ca[1]);
                }
                if(a.size == 4)
                {
                    i_ca.i = a.ivalue;
                    output.push_back(i_ca.ca[0]);
                    output.push_back(i_ca.ca[1]);
                    output.push_back(i_ca.ca[2]);
                    output.push_back(i_ca.ca[3]);
                }
            }
            else DEBUG_M("Arg not activated for nstruction at line: " << l.line_num);
        };
        push_arg(l.args[0]);
        push_arg(l.args[1]);
    }
    return output;
}