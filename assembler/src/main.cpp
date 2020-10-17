#include <bdc_assembler/utils/log.hpp>
#include <bdc_assembler/core/modules.hpp>
#include <bdc_assembler/core/parser.hpp>
#include <arg_parser/arg_parser.hpp>
#include <fstream>
#include <filesystem>
#include <string>

static void show_help()
{

}

static void show_version()
{
    
}

const std::string optname(const int code, const Arg_parser::Option options[])
{
    static char buf[2] = "?";

    if(code != 0)
    for(int i = 0; options[i].code; ++i)
        if(code == options[i].code)
        { 
            if(options[i].name)
                 return options[i].name;
            else 
                break;
        }
    if(code > 0 && code < 256)
        buf[0] = code;
    else
        buf[0] = '?';
    return buf;
}

int main(const int argc, const char** argv)
{
    DEBUG_M("Running Assembler");

#pragma region ARG PARSER

    const Arg_parser::Option options[] =
    {
        { 'o', "output-file"    , Arg_parser::yes },
        { 'p', "plateform"      , Arg_parser::yes },
        { 'h', "help"           , Arg_parser::no  },
        { 'v', "version"        , Arg_parser::no  },
        { 0  , 0                , Arg_parser::no  }
    };
    const Arg_parser parser(argc, argv, options);
    for(int argind = 0; argind < parser.arguments(); ++argind)
    {
        const int code = parser.code(argind);
        if(!code) break;
            switch(code)
            {
            case 'h':
                show_help();
                return 0;
            case 'v':
                show_version();
                return 0;
            }
    }
    std::string arg_input_file;
    std::string arg_output_file;
    std::string arg_plateform = "flat";
    for(int argind = 0; argind < parser.arguments(); ++argind)
    {
        const int code = parser.code(argind);
        const std::string arg = parser.argument(argind);
        if(code)
        {
            const std::string name = optname(code, options);
            if(name == "output-file")
            {
                if(arg_output_file.empty())
                    arg_output_file = arg;
                else
                {
                    ERR_M("Output file specified more than one time");
                    exit(EXIT_FAILURE);
                }
            }
            else if(name == "plateform")
            {
                if(arg_plateform.empty())
                    arg_plateform = arg;
                else
                {
                    ERR_M("Plateform specified more than one time");
                    exit(EXIT_FAILURE);
                }
            }

        }
        else
        {
            if(arg_input_file.empty())
                arg_input_file = arg;
            else
            {
                ERR_M("input file specified more than one time");
                exit(EXIT_FAILURE);
            }
        }
    }

    if(arg_input_file == "")
    {
        ERR_M("Input file not specified");
        exit(EXIT_FAILURE);
    }
    if(arg_output_file == "")
        arg_output_file = arg_input_file + ".bin";

#pragma endregion

    if(!std::filesystem::exists(arg_input_file))
    {
        ERR_M("Input file " << arg_input_file << " does not exist!");
        exit(EXIT_FAILURE);
    }
    load_modules();

    std::ifstream ifs(arg_input_file);
    std::vector<char> v = call_module(arg_plateform, parse_file(
            std::string((std::istreambuf_iterator<char>(ifs)),
           (std::istreambuf_iterator<char>()                    ))));
    ifs.close();

    std::ofstream ofs(arg_output_file, std::ios::out | std::ios::binary);
    ofs.write(v.data(), v.size());
    ofs.close();

    unload_modules();
    DEBUG_M("Assembler ended");
}