#include "JsonReader.h"
#include <cctype>
#include <stdexcept>
#include <string>
#include <unordered_map>

JsonValue JsonReader::readJsonFile() {
    return parseValue();
};

std::string JsonReader::parseString() {
    if (next() != '"') throw std::runtime_error("Expected '\"'");

    std::string result;
    char c;
    while ((c = next()) != '"') {
        if (c == '\\') {
            c = next();
            if (c == 'n') result += '\n';
            else if (c == 't') result += '\t';
            else result += c;
        } else {
            result += c;
        }
    }
    return result;
}

double JsonReader::parseNumber() {
    bool negative = false;
    double result = 0;
    double decimalMult = 0;

    if (peek() == '-') {
        negative = true;
        pos++;
    }

    for (char c = next(); isdigit(c) || c == '.'; c = next()) {
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
    pos--;
    return result * (negative ? -1 : 1);
}

JsonValue JsonReader::parseObject() {
    char c = next();
    std::unordered_map<std::string, JsonValue> obj;

    skipWhitespace();
    if (next() == '}') return JsonValue(obj); // empty object
    pos--;

    do {
        skipWhitespace();
        std::string key = parseString();

        skipWhitespace();
        if (next() != ':') throw std::runtime_error("Expected ':'");

        JsonValue value = parseValue();
        obj[key] = value;

        skipWhitespace();
        c = next();
    } while (c == ',');

    if (c != '}') throw std::runtime_error("Expected '}'");
    return JsonValue(obj);
}

JsonValue JsonReader::parseArray() {
    int c = next();
    std::vector<JsonValue> arr;

    skipWhitespace();
    if (next() == ']') return JsonValue(arr); // empty array
    pos--;

    do {
        JsonValue value = parseValue();
        arr.push_back(value);

        skipWhitespace();
        c = next();
    } while (c == ',');

    if (c != ']') throw std::runtime_error("Expected ']'");
    return JsonValue(arr);
}

void JsonReader::skipWhitespace() {
    while (isspace(peek())) pos++;
}

JsonValue JsonReader::parseValue() {
    skipWhitespace();
    char c = peek();

    if (c == '"') {
        return JsonValue(parseString());
    } else if (isdigit(c) || c == '-') {
        return JsonValue(parseNumber());
    } else if (c == '{') {
        return parseObject();
    } else if (c == '[') {
        return parseArray();
    }
    throw std::runtime_error("Unexpected character");
}
