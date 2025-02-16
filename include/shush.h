#ifndef __SHUSH_HPP__
#define __SHUSH_HPP__

#include "interactive.h"
#include "parser.h"
#include "system.h"
#include "types.h"
#include <iostream>
using namespace std;

#define APPLICATION "shush"
#define APPLICATION_VERSION "1.1.0"
#define SHUSH_HEADERS                                                                                                  \
    { "Alias", "System / Host", "User", "Login via SSH Key", "Login via Password" }
#define SHUSH_LIST_HEADERS                                                                                             \
    { "Alias", "System / Host" }

inline std::unordered_map<std::string, int> ArgumentTypes = {{"help", NON_VALUED_ARGUMENT},
                                                             {"yes", NON_VALUED_ARGUMENT},
                                                             {"add", NON_VALUED_ARGUMENT},
                                                             {"list", NON_VALUED_ARGUMENT},
                                                             {"remove", NON_VALUED_ARGUMENT},
                                                             {"update", NON_VALUED_ARGUMENT},
                                                             {"connect", NON_VALUED_ARGUMENT},
                                                             {"system", OPTIONALY_VALUED_ARGUMENT},
                                                             {"host", VALUED_ARGUMENT},
                                                             {"user", VALUED_ARGUMENT},
                                                             {"key", VALUED_ARGUMENT},
                                                             {"password", VALUED_ARGUMENT},
                                                             {"alias", VALUED_ARGUMENT},
                                                             {"z", VALUED_ARGUMENT},
                                                             {"s", OPTIONALY_VALUED_ARGUMENT},
                                                             {"h", NON_VALUED_ARGUMENT},
                                                             {"y", NON_VALUED_ARGUMENT},
                                                             {"a", NON_VALUED_ARGUMENT},
                                                             {"l", NON_VALUED_ARGUMENT},
                                                             {"r", NON_VALUED_ARGUMENT},
                                                             {"e", NON_VALUED_ARGUMENT},
                                                             {"c", NON_VALUED_ARGUMENT},
                                                             {"u", VALUED_ARGUMENT},
                                                             {"k", VALUED_ARGUMENT},
                                                             {"p", VALUED_ARGUMENT}};

class Shush {
  private:
    System system;
    ArgumentParser arguments;

    bool yesFlag = false;

    bool helpFlag = false;
    bool addFlag = false;
    bool listFlag = false;
    bool removeFlag = false;
    bool updateFlag = false;
    bool connectFlag = false;

    bool userFlag = 0;
    bool systemFlag = 0;
    bool KeyFlag = 0;
    bool passwordFlag = 0;
    bool aliasFlag = 0;
    string _system = "";
    string _ssh_key = "";
    string _user = "";
    string _password = "";
    string _alias = "";

    void help();
    void prepare();
    void add_system(bool);
    void list_systems(bool);
    void remove_system();
    void connect_system();

    vector<SystemObject> get_system_objects();
    vector<SystemObject> get_system_header_objects();

  public:
    Shush();
    Shush(SystemType);
    void init(int, char const *[]);
};

#endif