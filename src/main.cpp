#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "lexer.hpp"

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

    for (auto& token : lexer.tokens) {
        std::cout << token.type << ", " << token.value.value_or("null") << "\n";
    }

    return 0;
}
