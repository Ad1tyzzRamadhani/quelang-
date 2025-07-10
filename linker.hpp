#ifndef LINKER_HPP
#define LINKER_HPP

#include <string>
#include <unordered_set>
#include <unordered_map>
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
            size_t pos = line.find("#");
            if (pos != std::string::npos) line = line.substr(0, pos);

            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.rfind("load \"", 0) == 0 && line.back() == '"') {
                std::string path = line.substr(6);
                if (path.front() == '"' && path.back() == '"') {
                    path = path.substr(1, path.length() - 2);
                }
                result << processFile(path) << "\n"; 
            } else if (!line.empty()) {
                result << line << "\n";
            }
        }

        result << "\n"; 
        return result.str();
    }
};

#endif
