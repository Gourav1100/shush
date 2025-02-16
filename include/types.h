#ifndef __TYPES_H__
#define __TYPES_H__

#include <string>
#include <unordered_map>

#define NON_VALUED_ARGUMENT 1
#define VALUED_ARGUMENT 2
#define OPTIONALY_VALUED_ARGUMENT 3

enum SystemType { WINDOWS, LINUX, UNKNOWN };

struct SystemObject {
    bool password_exisits = false;
    bool ssh_key_exists = false;
    std::string username = "";
    std::string system = "";
    std::string alias = "";
};

#endif