#include "json.h"
#include <cstdio>
#include <string>

class FileReader {
private:
    FILE* file;

    void skipWhitespace();
    std::string parseString();
    bool parseBool();
    double parseNumber();
    JsonValue parseObject();
    JsonValue parseArray();
    JsonValue parseValue();
public:
    FileReader(const std::string& filename) {
        file = fopen(filename.c_str(), "r");
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filename);
        }
    }
    ~FileReader() { if (file) fclose(file); }

    JsonValue readJsonFile();
};
