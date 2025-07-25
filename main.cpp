#include "parser.cpp"
#include "codegen.cpp"
#include "linker.hpp"
#include "checker.cpp"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
    bool debug = false;
    std::string inputPath, outputPath;

    if (argc == 4 && std::string(argv[1]) == "--debug") {
        debug = true;
        inputPath = argv[2];
        outputPath = argv[3];
    } else if (argc == 3) {
        inputPath = argv[1];
        outputPath = argv[2];
    } else {
        std::cerr << "Usage: " << argv[0] << " [--debug] input.q output.s\n";
        return 1;
    }

    try {
        Linker linker;
        std::string source = linker.link(inputPath);

        Tokenizer tokenizer(source, debug);
        std::vector<Token> tokens = tokenizer.tokenize();

        Parser parser(tokens);
        auto program = parser.parseProgram();

        SemanticChecker checker;
        checker.check(program);

        CodegenASM codegen;
        std::string asmCode = codegen.generate(program);

        std::ofstream outFile(outputPath);
        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot write to output file: " << outputPath << "\n";
            return 1;
        }
        outFile << asmCode;
        outFile.close();

        std::cout << "Compilation successful. Output written to " << outputPath << "\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
