#ifndef BDCA_PARSER_H
#define BDCA_PARSER_H

#include <string>
#include <vector>

enum arg_type { DISABLED, MEMORY8, MEMORY16, MEMORY32, LITERAL8, LITERAL16, LITERAL32, REG };

enum instruction_n { NO_OP  = 0xFF,
                   CPUINF = 0x00,
                   SET    = 0x10,
                   PUSH4,
                   PUSH2,
                   PUSH1,
                   POP4,
                   POP2,
                   POP1,
                   LGDTR,
                   SGDTR,
                   LDS,
                   LCS,
                   LSS,
                   SDS,
                   SCS,
                   SSS,
                   LIHP = 0x20,
                   SIHP,
                   INT,
                   IRET,
                   NOT = 0x30,
                   OR,
                   AND,
                   XOR,
                   NOR,
                   NAND,
                   XNOR,
                   ADD,
                   FADD,
                   SUB,
                   FSUB,
                   MUL,
                   FMUL,
                   DIV,
                   FDIV,
                   SL = 0x70,
                   SR = 0x71,
                   INTG,
                   FLOAT,
                   JMP = 0x40,
                   CALL,
                   CMP,
                   FCMP,
                   UCMP,
                   JE, JZ, JG, JL, JEG, JEL,
                   RET,
                   HLT,
                   SLEEPS,
                   SLEEPMS,
                   SLEEPMCS,
                   LCPL = 0X60,
                   SCPL,
                   IN = 0X80,
                   OUT };

struct arg
{
    std::vector<std::string>    tokens              ;
    std::string                 value               ;
    std::string                 prefix              ;
    bool                        activated  = false  ;
    bool                        reg                 ;
    bool                        mem                 ;
    uint                        size                ;
    uint                        ivalue              ;
};

struct line_data
{
    bool            pseudo_ins = false  ;
    int             line_num            ;
    std::string     line                ;
    std::string     line_only_space     ;
    std::string     instruction         ;
    instruction_n   instr               ;
    uint8_t         arg_desc            ;
    arg             args[2]             ;
};



extern std::vector<line_data> parse_file(const std::string& file);

#endif /* BDCA_PARSER_H */