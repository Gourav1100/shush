#ifndef __COMMAND_H__
#define __COMMAND_H__

#ifndef __COMMAND_LIMITS__
#define __COMMAND_LIMITS__
#define COMMAND_BUFFER_SIZE 1000
#endif

#include "exception.h"
#include "types.h"
#include <array>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

class Command {
  private:
    std::vector<std::string> commands;
    std::vector<std::string> outputs;
    SystemType systemType;

  public:
    Command();
    Command(SystemType);
    void execute(std::string, bool = false);
    std::string get_command();
    std::string get_output();
    std::string get_command(int);
    std::string get_output(int);
};

#endif