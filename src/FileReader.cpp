#include "FileReader.h"
#include <cctype>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <unordered_map>

JsonValue FileReader::readJsonFile(const std::string &filePath) {
    file = fopen(filePath.c_str(), "r");

    return parseValue();
};

std::string FileReader::parseString() {
    int c = getc(file);
    if (c != '"') throw std::runtime_error("Expected '\"'");

    std::string result;
    while ((c = getc(file)) != EOF && c != '"') {
        if (c == '\\') {
            c = getc(file);
            if (c == 'n') result += '\n';
            else if (c == 't') result += '\t';
            else result += c;
        } else {
            result += c;
        }
    }
    return result;
}

double FileReader::parseNumber() {
    bool negative = false;
    double result = 0;
    double decimalMult = 0;

    int c = getc(file);
    if (c == '-')
        negative = true;
    else
        ungetc(c, file);

    while ((c = getc(file)) != EOF && (isdigit(c) || c == '.')) {
        if (c == '.') {
            decimalMult = 1;
            continue;
        }
        if (decimalMult == 0)
            result *= 10;
        else
            decimalMult *= 10;

        result += (c - '0') / (decimalMult == 0 ? 1 : decimalMult);
    }
    if (c != EOF) ungetc(c, file);
    return result * (negative ? -1 : 1);
}

JsonValue FileReader::parseObject() {
    int c = getc(file);
    std::unordered_map<std::string, JsonValue> obj;

    skipWhitespace();
    if ((c = fgetc(file)) == '}') return JsonValue(obj); // empty object
    ungetc(c, file);

    do {
        skipWhitespace();
        std::string key = parseString();

        skipWhitespace();
        if (fgetc(file) != ':') throw std::runtime_error("Expected ':'");

        JsonValue value = parseValue();
        obj[key] = value;

        skipWhitespace();
        c = getc(file);
    } while (c == ',');

    if (c != '}') throw std::runtime_error("Expected '}'");
    return JsonValue(obj);
}

JsonValue FileReader::parseArray() {
    int c = getc(file);
    std::vector<JsonValue> arr;

    skipWhitespace();
    if ((c = fgetc(file)) == ']') return JsonValue(arr); // empty array
    ungetc(c, file);

    do {
        JsonValue value = parseValue();
        arr.push_back(value);

        skipWhitespace();
        c = getc(file);
    } while (c == ',');

    if (c != ']') throw std::runtime_error("Expected ']'");
    return JsonValue(arr);
}

void FileReader::skipWhitespace() {
    int c;
    while ((c = getc(file)) != EOF && isspace(c)) {
        // consume whitespace
    }
    if (c != EOF) ungetc(c, file);
}

JsonValue FileReader::parseValue() {
    skipWhitespace();
    int c = fgetc(file);

    if (c == '"') {
        ungetc(c, file);
        return JsonValue(parseString());
    } else if (isdigit(c) || c == '-') {
        ungetc(c, file);
        return JsonValue(parseNumber());
    } else if (c == '{') {
        ungetc(c, file);
        return parseObject();
    } else if (c == '[') {
        ungetc(c, file);
        return parseArray();
    }
    throw std::runtime_error("Unexpected character");
}
