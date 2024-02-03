#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "generator.hpp"
#include "lexer.hpp"
#include "parser.hpp"

struct ProgramInput {
    char* input_filename;
    char* output_filename;
};

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Incorrect usage.\n";
        exit(1);
    }

    ProgramInput program_input{ argv[1], argv[2] };

    std::filesystem::path input_filepath{ program_input.input_filename };
    if (!std::filesystem::exists(input_filepath)) {
        std::cout << "Specified input file does not exist.\n";
        exit(1);
    }

    std::ifstream input_file(program_input.input_filename);
    std::stringstream input_file_buffer;
    input_file_buffer << input_file.rdbuf();
    std::string input = input_file_buffer.str();

    Lexer lexer(input);

    Parser parser(lexer.tokens);

    Generator generator(parser.global_body);

    std::ofstream output_file(program_input.output_filename +
                              std::string(".asm"));
    output_file << generator.output.str();
    output_file.close();

    std::stringstream build_command;
    build_command << "nasm -felf64 " << program_input.output_filename
                  << ".asm -o " << program_input.output_filename << ".o && ld "
                  << program_input.output_filename << ".o -o"
                  << program_input.output_filename;

    system(build_command.str().c_str());

    return 0;
}
