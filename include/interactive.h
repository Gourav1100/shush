#ifndef __INTERACTIVE_H__
#define __INTERACTIVE_H__

#include "exception.h"
#include "utils.h"
#include <iostream>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

#define ESCAPE_SEQUENCE '\x1b'

enum Keys { HOME_KEY = 1000, END_KEY, ARROW_UP, ARROW_DOWN, ARROW_RIGHT, ARROW_LEFT, PAGE_UP, PAGE_DOWN, EXIT_KEY, SUB_KEY };

struct InteractionData {
    std::string screen;
    struct termios old_config;
};

class Interactions {
  public:
    static int get_key();
    static void enable_raw_mode();
    static void disable_raw_mode();
    static void die(const char *s);
    static void print(const std::string &message);
    static void println(const std::string &message);
    static void clear_screen();
    static void draw();
};

class Menu : private Interactions {
  private:
    std::vector<std::string> options;
    std::string heading;
    void display_menu(int);

  public:
    Menu(std::string, std::vector<std::string>);
    int navigate();
};

#endif