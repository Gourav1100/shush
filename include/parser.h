#ifndef __PARSER_H__
#define __PARSER_H__

#include "exception.h"
#include "types.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#define PARSER_NON_VALUED_DEFAULT "-parser-true"

class ArgumentParser {
  private:
    int argc;
    char const **argv;
    std::unordered_map<std::string, std::string> arguments;
    std::unordered_map<std::string, int> argument_type;
    void parse();
    bool isKey(std::string);
    std::string getValue(int);

  public:
    ArgumentParser(int, char const *[], std::unordered_map<std::string, int>);
    std::string Get(std::string);
    std::vector<std::string> Keys();
    bool isParserDefault(std::string);
};

#endif