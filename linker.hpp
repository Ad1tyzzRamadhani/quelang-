#ifndef LINKER_HPP
#define LINKER_HPP

#include <string>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <stdexcept>

class Linker {
    std::unordered_set<std::string> loadedFiles;

public:
    std::string link(const std::string& path) {
        loadedFiles.clear();
        return processFile(path);
    }

private:
    std::string processFile(const std::string& filepath) {
        if (loadedFiles.count(filepath)) return ""; 
        loadedFiles.insert(filepath);

        std::ifstream in(filepath);
        if (!in.is_open()) throw std::runtime_error("Failed to open file: " + filepath);

        std::ostringstream result;
        std::string line;

        while (std::getline(in, line)) {
            size_t commentPos = line.find('#');
            if (commentPos != std::string::npos) {
                line = line.substr(0, commentPos);
            }
            
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.rfind("load", 0) == 0) {
                size_t firstQuote = line.find('"');
                size_t lastQuote = line.rfind('"');
                if (firstQuote != std::string::npos && lastQuote != std::string::npos && firstQuote < lastQuote) {
                    std::string path = line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
                    result << processFile(path) << "\n";
                }
                continue;
            }

            if (!line.empty()) {
                result << line << "\n";
            }
        }

        result << "\n";
        return result.str();
    }
};

#endif
