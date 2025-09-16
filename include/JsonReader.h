#include "json.h"
#include <SDL3/SDL_iostream.h>
#include <string>

class JsonReader {
private:
    SDL_IOStream* stream;
    std::string buffer;
    size_t pos = 0;

    char next() {
        if (pos >= buffer.size()) return EOF;
        return buffer[pos++];
    }
    char peek() {
        if (pos >= buffer.size()) return EOF;
        return buffer[pos];
    }
    void skipWhitespace();
    std::string parseString();
    bool parseBool();
    double parseNumber();
    JsonValue parseObject();
    JsonValue parseArray();
    JsonValue parseValue();
public:
    JsonReader(SDL_IOStream* stream) : stream(stream) {
        if (!stream) {
            throw std::runtime_error("Failed to open stream");
        }
        Sint64 size = SDL_GetIOSize(stream);
        buffer.resize(size);
        SDL_ReadIO(stream, buffer.data(), size);
    }

    JsonValue readJsonFile();
};
