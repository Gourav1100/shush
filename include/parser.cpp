#include "parser.h"

bool ArgumentParser::isKey(std::string value) { return value.find("-") == 0; }

std::string ArgumentParser::getValue(int index = -1) {
    if (index >= this->argc || index < 0) {
        return PARSER_NON_VALUED_DEFAULT;
    }
    std::string value = this->argv[index];
    if (this->isKey(value)) {
        return PARSER_NON_VALUED_DEFAULT;
    }
    return value;
}
bool ArgumentParser::isParserDefault(std::string value) { return value == PARSER_NON_VALUED_DEFAULT; }

ArgumentParser::ArgumentParser(int argc, char const *argv[], std::unordered_map<std::string, int> argument_type) {
    this->argc = argc;
    this->argv = argv;
    this->argument_type = argument_type;
    this->parse();
}

std::string ArgumentParser::Get(std::string key) { return this->arguments[key]; }

void ArgumentParser::parse() {
    int index = 1;
    while (index < this->argc) {
        std::string arg = this->argv[index];
        std::string key;
        bool short_hand = false;
        if (arg.find("--") == 0) {
            key = to_lower(arg.substr(2));
        } else if (arg.find("-") == 0) {
            key = to_lower(arg.substr(1));
            short_hand = true;
        } else {
            throw Exception("Invalid argument: " + arg);
        }
        index++;
        if (short_hand) {
            for (char c : key) {
                std::string k = std::string(1, c);
                if (this->argument_type[k] != NON_VALUED_ARGUMENT) {
                    this->arguments[k] = this->getValue(index++);
                    if (this->isParserDefault(this->arguments[k]) &&
                        this->argument_type[std::string(1, c)] == VALUED_ARGUMENT) {
                        throw Exception("Value required for argument: " + std::string(1, c));
                    }
                } else {
                    this->arguments[k] = this->getValue();
                }
            }
        } else {
            if (this->argument_type[key] != NON_VALUED_ARGUMENT) {
                this->arguments[key] = this->getValue(index++);
                if (this->isParserDefault(this->arguments[key]) && this->argument_type[key] == VALUED_ARGUMENT) {
                    throw Exception("Value required for argument: " + key);
                }
            } else {
                this->arguments[key] = this->getValue();
            }
        }
    }
}

std::vector<std::string> ArgumentParser::Keys() {
    std::vector<std::string> keys;
    for (auto const &pair : this->arguments) {
        keys.push_back(pair.first);
    }
    return keys;
}