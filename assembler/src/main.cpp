#include <bdc_assembler/utils/log.hpp>
#include <bdc_assembler/core/arguments.hpp>
#include <bdc_assembler/core/modules.hpp>
#include <bdc_assembler/core/parser.hpp>
#include <fstream>
#include <filesystem>


int main(int argc, char** argv)
{
    DEBUG_M("Running Assembler");
    // setup signal handler

    std::vector<std::string> args;
    for(uint i = 1; i < argc; i++)
        args.push_back(argv[i]);
    arguments prog_arg = parse_args(args);

    DEBUG_M("input_file: " << prog_arg.input_file << ", output_file: " << prog_arg.output_file 
            << ", plateform: " << prog_arg.plateform);

    if(!std::filesystem::exists(prog_arg.input_file))
    {
        ERR_M("Input file " << prog_arg.input_file << " does not exist!");
        exit(EXIT_FAILURE);
    }
    load_modules();

    std::ifstream input_file(prog_arg.input_file);
    std::vector<char> v = call_module(prog_arg.plateform, parse_file(
            std::string((std::istreambuf_iterator<char>(input_file)),
           (std::istreambuf_iterator<char>()                    ))));
    input_file.close();

    std::ofstream output_file(prog_arg.output_file, std::ios::out | std::ios::binary);
    output_file.write(v.data(), v.size());
    output_file.close();

    unload_modules();
    DEBUG_M("Assembler ended");
}