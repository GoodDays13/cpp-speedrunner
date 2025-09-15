#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

class JsonValue {
private:
    std::variant<std::nullptr_t, bool, double, std::string, std::vector<JsonValue>, std::unordered_map<std::string, JsonValue>> value;
public:
    JsonValue() : value(nullptr) {}
    JsonValue(std::nullptr_t) : value(nullptr) {}
    JsonValue(bool b) : value(b) {}
    JsonValue(double d) : value(d) {}
    JsonValue(const std::string& s) : value(s) {}
    JsonValue(const char* s) : value(std::string(s)) {}
    JsonValue(const std::vector<JsonValue>& arr) : value(arr) {}
    JsonValue(const std::unordered_map<std::string, JsonValue>& obj) : value(obj) {}

    JsonValue& operator[](const std::string& key) {
        if (auto obj = std::get_if<std::unordered_map<std::string, JsonValue>>(&value)) {
            return (*obj)[key];  // Creates key if missing
        }
        throw std::runtime_error("JsonValue is not an object");
    }

    JsonValue& operator[](size_t index) {
        if (auto arr = std::get_if<std::vector<JsonValue>>(&value)) {
            if (index < arr->size()) {
                return (*arr)[index];
            }
            throw std::out_of_range("JsonValue array index out of range");
        }
        throw std::runtime_error("JsonValue is not an array");
    }

    bool isNull() const {
        return std::holds_alternative<std::nullptr_t>(value);
    }

    bool getBool() const {
        if (auto ptr = std::get_if<bool>(&value)) {
            return *ptr;
        }
        throw std::runtime_error("JsonValue is not a bool");
    }

    double getDouble() const {
        if (auto ptr = std::get_if<double>(&value)) {
            return *ptr;
        }
        throw std::runtime_error("JsonValue is not a double");
    }

    std::string getString() const {
        if (auto ptr = std::get_if<std::string>(&value)) {
            return *ptr;
        }
        throw std::runtime_error("JsonValue is not a string");
    }

    bool isBool() const {
        return std::holds_alternative<bool>(value);
    }

    bool isDouble() const {
        return std::holds_alternative<double>(value);
    }

    bool isString() const {
        return std::holds_alternative<std::string>(value);
    }

    bool isArray() const {
        return std::holds_alternative<std::vector<JsonValue>>(value);
    }

    bool isObject() const {
        return std::holds_alternative<std::unordered_map<std::string, JsonValue>>(value);
    }

    size_t size() const {
        if (auto arr = std::get_if<std::vector<JsonValue>>(&value)) {
            return arr->size();
        }
        if (auto obj = std::get_if<std::unordered_map<std::string, JsonValue>>(&value)) {
            return obj->size();
        }
        throw std::runtime_error("JsonValue is neither an array nor an object");
    }
};
