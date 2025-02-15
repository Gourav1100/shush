#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "command.h"
#include "exception.h"
#include "types.h"
#include <array>
#include <filesystem>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include <vector>

class System {
  private:
    SystemType systemType;

  public:
    System(SystemType);
    void copy(std::string, std::string);
    void move(std::string, std::string);
    void remove(std::string);
    void rename(std::string, std::string);
    void make_directory(std::string);
    void remove_directory(std::string);
    std::vector<std::string> list_directory(std::string);
    void write(std::string, std::string);
    std::string read(std::string);
    bool exists(std::string);
    std::string get_system_type_string();
    SystemType get_system_type();
    std::string get_config_base(std::string);
    std::string join_path(std::string, std::string);
};

#endif