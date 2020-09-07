#include <bdc_assembler/core/parser.hpp>
#include <bdc_assembler/utils/log.hpp>
#include <vector>
#include <sstream>
#include <map>
#include <utility>
#include <regex>
#include <algorithm>

static std::regex s_label_decl ("([a-zA-Z_][a-zA-Z0-9_@]*):");
static std::regex s_label_call (":([a-zA-Z_][a-zA-Z0-9_@]*)");
static std::regex s_register   ("&(a|A|b|B|c|C|d|D|r|R|t|T|u|U|sp|SP|memr0|MEMR0|memr1|MEMR1|irp|IRP|rp|RP|ip|IP|fa|FA|fb|FB|fc|FC|fd|FD|fr|FR)");
static std::regex s_literal_hex("\\$0x([0-9a-fA-F]+)");
static std::regex s_literal_bin("\\$0b([01]+)");
static std::regex s_literal_oct("\\$0c([0-7]+)");
static std::regex s_literal_dec("\\$([0-9]+)");
static std::regex s_mem_access ("@");

static std::map<std::string, uint> s_labels;

static int string_to_int(const std::string& s, uint line_num)
{
    if(s.starts_with("0x"))
    {
        try
        {
            return std::stol(s.substr(2), nullptr, 16);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            ERR_M("Invalid hex: " << s << ", at line: " << line_num);
            exit(EXIT_FAILURE);
        } 
    }
    if(s.starts_with("0b"))
    {
        try
        {
            return std::stol(s.substr(2), nullptr, 2);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            ERR_M("Invalid binary: " << s << ", at line: " << line_num);
            exit(EXIT_FAILURE);
        } 
    }
    if(s.starts_with("0c"))
    {
        try
        {
            return std::stol(s.substr(2), nullptr, 8);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            ERR_M("Invalid octal: " << s << ", at line: " << line_num);
            exit(EXIT_FAILURE);
        } 
    }
    else
    {
        try
        {
            return std::stol(s);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
            ERR_M("Invalid decimal: " << s << ", at line: " << line_num);
            exit(EXIT_FAILURE);
        }
    }
    return -1;
}
static std::vector<std::pair<int, std::string>> get_lines(const std::string& str)
{
    std::vector<std::pair<int, std::string>> lines;
    std::string line;
    std::stringstream ss;
    ss << str;
    for(uint line_num = 1; std::getline(ss, line); line_num++)
        lines.push_back(std::pair<int, std::string>(line_num, line));
    return lines;
}
static std::string trim(std::string& str)
{
    for(char c = str[0]; std::isspace(c); c = str[0])
        str.erase(str.begin());
    for(char c = str[str.size() - 1]; std::isspace(c); c = str[str.size() - 1])
        str.erase(str.end() - 1);
    return str;
}
static std::string remove_any(const std::string& str, char c)
{
    std::string s = str;
    for(uint i = 0; i < s.size(); i++)
        if(s[i] == c)
        {
            s.substr(i, 1);
            i--;
        }
    return s;
}
static std::vector<std::string> split(const std::string& str, char r)
{
    std::vector<std::string> splited;
    std::stringstream s;
    for(char c : str)
        if(c == r)
        {
            splited.push_back(s.str());
            s = std::stringstream();
        }
        else
            s << c;
    splited.push_back(s.str()); // The last element
    return splited;
}
static uint char_count(const std::string& s, char c)
{
    uint cn = 0;    
    for(char ch : s)
        if(ch == c)
            cn++;
    return cn;
}

static uint8_t get_reg(const std::string& rs)
{
#define RET(n, r) if(rs == r) return n
    RET(0x00, "a");
    RET(0x01, "b");
    RET(0x02, "c");
    RET(0x03, "d");
    RET(0x04, "r");
    RET(0x06, "t");
    RET(0x07, "u");
    RET(0x10, "sp");
    RET(0x11, "memr0");
    RET(0x12, "memr1");
    RET(0x20, "irp");
    RET(0x30, "rp");
    RET(0x31, "ip");
    RET(0x50, "fa");
    RET(0x51, "fb");
    RET(0x52, "fc");
    RET(0x53, "fd");
    RET(0x54, "fr");
#undef RET
    WARN_M("Invalid reg: " << rs << " returning 0xFF");
    return 0xFF;
}

std::vector<line_data> parse_file(const std::string& file)
{
    DEBUG_M("Parsing file");
    std::vector<line_data> lines_data;
    std::vector<std::pair<int, std::string>> lines = get_lines(file);
    for(std::pair<int, std::string>& line : lines)
    {
        DEBUG_M("Line " << line.first << "\tbefor trim:|" << line.second << '|');
        line.second = trim(line.second);
        DEBUG_M("Line " << line.first << "\tafter trim:|" << line.second << '|');
    }
    for(auto i = lines.begin(); i != lines.end(); i++)
        if((*i).second.empty())
            lines.erase(i);
    uint adr = 0;
    for(std::vector<std::pair<int, std::string>>::iterator i = lines.begin(); i != lines.end(); i++)
    {
        std::string& line = (*i).second;
        int line_num = (*i).first;
        trim(line);
        if(line.ends_with(':'))
        {
            std::smatch label_match;
            if(std::regex_match(line, label_match, s_label_decl))
            {
                DEBUG_M("Adding label: " << label_match[1] << ", with address: " << adr);
                s_labels[label_match[1]] = adr;
                continue;
            }
            else
            {
                ERR_M("At line: " << line_num << ", '" << label_match[1] << "' is an invalid label name");
                exit(EXIT_FAILURE);
                return std::vector<line_data>();
            }
        }
        std::string line_only_space = line;
        line_data line_d{
                         .line_num = line_num,
                         .line = line,
                         .line_only_space = line_only_space
                        };
        {
            auto both_space = [] (char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); };
            for(char& c : line_only_space)
                if(c == '\t')
                    c = ' ';
            line_only_space.erase(std::unique(line_only_space.begin(), line_only_space.end(),
                                  both_space), line_only_space.end());
            if(line_only_space.find(' ') == std::string::npos) // no space found
                line_d.instruction = line_only_space;
            else
            {
                auto proccess_arg = [line_num] (std::string& arg_str, arg& arg)
                {
                    trim(arg_str);
                    std::vector<std::string> tokens = split(arg_str, ' ');
                    if(tokens.size() == 0)
                    {
                        ERR_M("Syntax error at line: " << line_num);
                        exit(EXIT_FAILURE);
                    }
                    arg.value = tokens[tokens.size() - 1];
                    arg.tokens = tokens;
                    tokens.erase(tokens.end() - 1);
                    if(tokens.size() == 0);
                    else if(tokens.size() == 1)
                        arg.prefix = tokens[0];
                    else
                    {
                        ERR_M("Syntax error at line: " << line_num << ", invalid number of argument prefixes");
                        exit(EXIT_FAILURE);
                    }
                    arg.activated = true;
                };
                line_d.instruction = line_only_space.substr(0, line_only_space.find(' '));
                std::transform(line_d.instruction.begin(), line_d.instruction.end(),
                               line_d.instruction.begin(), ::toupper                );
                if(line_only_space.find(',') == std::string::npos)
                {
                    std::string arg = line_only_space.substr(line_only_space.find(' '), line_only_space.find(',') - line_only_space.find(' ') - 1);
                    proccess_arg(arg, line_d.args[0]);
                }
                else
                {
                    if(char_count(line_only_space, ',') == 1)
                    {
                        std::string arg0 = line_only_space.substr(line_only_space.find(' ') + 1, line_only_space.find(',') - line_only_space.find(' ') - 1);
                        std::string arg1 = line_only_space.substr(line_only_space.find(',') + 1);
                        proccess_arg(arg0, line_d.args[0]);
                        proccess_arg(arg1, line_d.args[1]);
                    }
                    else
                    {
                        ERR_M("Syntax error on line: " << line_num);
                        exit(EXIT_FAILURE);
                    }
                }
                if(line_d.args[0].activated)
                {
                    auto get_arg_size = [&] (arg& a)
                    {
                        auto get_prefix_size = [&] (std::string p)
                        {
                            std::transform(p.begin(), p.end(), p.begin(), ::toupper);
                            if(p == "BYTE")
                                return 1;
                            if(p == "WORD")
                                return 2;
                            if(p == "DWORD" || p == "")
                                return 4;
                            else
                            {
                                ERR_M("Invalid prefix : " << a.prefix << " in line: " << line_num);
                                exit(EXIT_FAILURE);
                                return -1;
                            }
                        };
                        if(!a.activated)
                            return 0;
                        std::smatch match;
#define MATCH(e) std::regex_match(a.value, match, e)
                        if(MATCH(s_label_call)  || MATCH(s_literal_hex) || MATCH(s_literal_bin)
                        || MATCH(s_literal_oct) || MATCH(s_literal_dec))
                        {
                            a.reg = false;
                            if(!MATCH(s_label_call))
                                a.ivalue = string_to_int(a.value.substr(1), line_num);
                            a.size = get_prefix_size(a.prefix);
                            return (int) a.size;
                        }
                        else if(MATCH(s_register))
                        {
                            if(a.prefix != "")
                            {
                                ERR_M("Can't have a prefix for a register. Line: " << line_num);
                                exit(EXIT_FAILURE);
                                return -1;
                            }
                            DEBUG_M("It is a register at line: " << line_num);
                            a.reg = true;
                            a.ivalue = get_reg(a.value.substr(1));
                            a.size = 1;
                            return 1;
                        }
                        else if(MATCH(s_mem_access))
                        {
                            a.mem = true;
                            a.size = get_prefix_size(a.prefix);
                            return 0;
                        }
                        else
                        {
                            ERR_M("Invalid argument: " << a.value << ", in line: " << line_num);
                            exit(EXIT_FAILURE);
                            return -1;
                        }
#undef MATCH
                    ERR_M("Unexpexted error while parsing line: " << line_num);
                    exit(EXIT_FAILURE);
                    return -1;
                    };

                    adr += get_arg_size(line_d.args[0]) + get_arg_size(line_d.args[1]);
                }
            }
            if(!line_d.instruction.starts_with("PUT"))
                adr += 2;
        }
        DEBUG_M("Instruction: " << line_d.instruction << ", at: " << adr -2);
        lines_data.push_back(line_d);
    }
    DEBUG_M("Replacing labels");
    for(line_data& l : lines_data) // Replacing labels with their values and instructions with their opcodes
    {
        auto replace_label = [&] (arg& a)
        {
            if(a.activated)
            {
                std::smatch match;
                if(std::regex_match(a.value, match, s_label_call))
                {
                    if(s_labels.contains(match[1]))
                        a.ivalue = s_labels[match[1]];
                    else
                    {
                        ERR_M("Label not found: " << match[1] << ", at line: " << l.line_num);
                        exit(EXIT_FAILURE);
                    }
                }
            }
        };
        replace_label(l.args[0]);
        replace_label(l.args[1]);
        DEBUG_M("Replacing instructions");
#define ERR(n) { ERR_M("Instruction: " << l.instruction << " can only have " << n << " argument"); exit(EXIT_FAILURE); } do{}while(0)
#define ADD0(s) else if(l.instruction == #s) { if(l.args[0].activated  ||  l.args[1].activated)ERR(0); l.instr = instruction_n::s; } // To add a generic instruction taking 0 arguments
#define ADD1(s) else if(l.instruction == #s) { if(!l.args[0].activated ||  l.args[1].activated)ERR(0); l.instr = instruction_n::s; } // To add a generic instruction taking 1 arguments
#define ADD2(s) else if(l.instruction == #s) { if(!l.args[0].activated || !l.args[1].activated)ERR(0); l.instr = instruction_n::s; } // To add a generic instruction taking 2 arguments
        if(l.instruction == "PUT_BYTE")
        {
            if(!l.args[0].activated ||  l.args[1].activated)
                ERR(0);
            if(l.args[0].mem || l.args[0].reg)
            {
                ERR_M("PUT_Byte only accepts literal integers, at line: " << l.line_num);
                exit(EXIT_FAILURE);
            }
            l.args[0].size = 1;
            l.pseudo_ins = true;
        }
        else if(l.instruction == "PUT_WORD")
        {
            if(!l.args[0].activated ||  l.args[1].activated)
                ERR(0);
            if(l.args[0].mem || l.args[0].reg)
            {
                ERR_M("PUT_Byte only accepts literal integers, at line: " << l.line_num);
                exit(EXIT_FAILURE);
            }
            l.args[0].size = 2;
            l.pseudo_ins = true;
        }
        else if(l.instruction == "PUT_DWORD")
        {
            if(!l.args[0].activated ||  l.args[1].activated)
                ERR(0);
            if(l.args[0].mem || l.args[0].reg)
            {
                ERR_M("PUT_Byte only accepts literal integers, at line: " << l.line_num);
                exit(EXIT_FAILURE);
            }
            l.args[0].size = 4;
            l.pseudo_ins = true;
        }
        ADD0(NO_OP)
        ADD1(CPUINF)
        ADD2(SET)
        else if(l.instruction == "PUSH")
        {
            if(!l.args[0].activated || l.args[1].activated)
                ERR(1);
            if(l.args[0].size == 4)
                l.instr = instruction_n::PUSH4;
            else if(l.args[0].size == 2)
                l.instr = instruction_n::PUSH2;
            else if(l.args[0].size == 1)
                l.instr = instruction_n::PUSH1;
        }
        ADD1(PUSH4)
        ADD1(PUSH2)
        ADD1(PUSH1)
        else if(l.instruction == "POP")
        {
            if(!l.args[0].activated || l.args[1].activated)
                ERR(1);
            if(l.args[0].size == 4)
                l.instr = instruction_n::POP4;
            else if(l.args[0].size == 2)
                l.instr = instruction_n::POP2;
            else if(l.args[0].size == 1)
                l.instr = instruction_n::POP1;
        }
        ADD1(POP4)
        ADD1(POP2)
        ADD1(POP1)
        ADD1(LGDTR)
        ADD1(SGDTR)
        ADD1(LDS)
        ADD1(LCS)
        ADD1(LSS)
        ADD1(SDS)
        ADD1(SCS)
        ADD1(SSS)
        ADD1(LIHP)
        ADD1(SIHP)
        ADD1(INT)
        ADD0(IRET)
        ADD2(NOT)
        ADD2(OR)
        ADD2(AND)
        ADD2(XOR)
        ADD2(NOR)
        ADD2(NAND)
        ADD2(XNOR)
        ADD2(ADD)
        ADD2(FADD)
        ADD2(SUB)
        ADD2(FSUB)
        ADD2(MUL)
        ADD2(FMUL)
        ADD2(DIV)
        ADD2(FDIV)
        ADD2(SL)
        ADD2(SR)
        ADD2(INTG)
        ADD2(FLOAT)
        ADD1(JMP)
        ADD1(CALL)
        ADD2(CMP)
        ADD2(FCMP)
        ADD2(UCMP)
        ADD1(JE)
        ADD1(JZ)
        ADD1(JG)
        ADD1(JEG)
        ADD1(JEL)
        ADD0(RET)
        ADD0(HLT)
        ADD1(SLEEPS)
        ADD1(SLEEPMS)
        else if(l.instruction == "SLEEP")
        {
            if(!l.args[0].activated ||  l.args[1].activated)
                ERR(0);
            l.instr = instruction_n::SLEEPMS;
        }
        ADD1(SLEEPMCS)
        ADD1(LCPL)
        ADD1(SCPL)
        ADD2(IN)
        ADD2(OUT)
        else
        {
            ERR_M("Unknown instruction: " << l.instruction << " at line " << l.line_num);
            exit(EXIT_FAILURE);
        }
#undef ADD2
#undef ADD1
#undef ADD0
        auto get_arg_type = [&] (arg& a)
        {
            if(!a.activated)
                return arg_type::DISABLED;
            if(a.reg)
                return arg_type::REG;
            if(a.mem)
            {
                if(a.size == 1)
                    return arg_type::MEMORY8;
                if(a.size == 2)
                    return arg_type::MEMORY16;
                if(a.size == 4)
                    return arg_type::MEMORY32;
            }
            if(a.size == 1)
                return arg_type::LITERAL8;
            if(a.size == 2)
                return arg_type::LITERAL16;
            if(a.size == 4)
                return arg_type::LITERAL32;
            return arg_type::DISABLED;
        };
        l.arg_desc = (uint8_t) get_arg_type(l.args[1]);
        l.arg_desc = l.arg_desc | (((uint8_t) get_arg_type(l.args[0])) << 4);
        DEBUG_M(std::hex << std::showbase << (uint) get_arg_type(l.args[0]) << ' ' << (uint) get_arg_type(l.args[1]) << ' ' << (uint) l.arg_desc << std::noshowbase << std::dec);
#undef ERR
    }
    DEBUG_M("Ended parsing file");
    return lines_data;
}