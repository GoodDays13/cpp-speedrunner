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
    JsonValue readJsonFile(const std::string& filePath);
};
