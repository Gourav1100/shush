#include "interactive.h"
#include <iostream>

InteractionData interaction_data;

// Interactions Class
int Interactions::get_key() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN)
            die("read");
    }
    if (c == ESCAPE_SEQUENCE) {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return ESCAPE_SEQUENCE;
        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return ESCAPE_SEQUENCE;
        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9')
                return Keys::EXIT_KEY;
            if (seq[2] == '~') {
                switch (seq[1]) {
                case '1':
                    return Keys::HOME_KEY;
                case '3':
                    return Keys::EXIT_KEY;
                case '4':
                    return Keys::END_KEY;
                case '5':
                    return Keys::PAGE_UP;
                case '6':
                    return Keys::PAGE_DOWN;
                case '7':
                    return Keys::HOME_KEY;
                case '8':
                    return Keys::END_KEY;
                }
            } else {
                switch (seq[1]) {
                case 'A':
                    return Keys::ARROW_UP;
                case 'B':
                    return Keys::ARROW_DOWN;
                case 'C':
                    return Keys::ARROW_RIGHT;
                case 'D':
                    return Keys::ARROW_LEFT;
                case 'H':
                    return Keys::HOME_KEY;
                case 'F':
                    return Keys::END_KEY;
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
            case 'H':
                return Keys::HOME_KEY;
            case 'F':
                return Keys::EXIT_KEY;
            }
        }
    }
    if (c == 3)
        return Keys::EXIT_KEY;
    if (c == 13)
        return Keys::SUB_KEY;
    return ESCAPE_SEQUENCE;
}

void Interactions::die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    throw Exception(s);
}

void Interactions::disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &interaction_data.old_config) == -1)
        die("failed tcsetattr! (disable raw mode)");
}

void Interactions::enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &interaction_data.old_config) == -1) {
        die("failed tcgetattr! (enable raw mode)");
    }
    atexit(disable_raw_mode);
    struct termios term = interaction_data.old_config;
    term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    term.c_oflag &= ~(OPOST);
    term.c_cflag |= (CS8);
    term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    term.c_cc[VMIN] = 0;
    term.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1) {
        die("failed tcsetattr! (enable raw mode)");
    }
}

void Interactions::print(const std::string &message) { interaction_data.screen += message; }

void Interactions::println(const std::string &message) {
    interaction_data.screen += message;
    interaction_data.screen += "\r\n";
}

void Interactions::clear_screen() {
    interaction_data.screen = "";
    print("\x1b[2J");
    print("\x1b[H");
    draw();
}

void Interactions::draw() { write(STDOUT_FILENO, interaction_data.screen.c_str(), interaction_data.screen.size()); }

// Menu Class
Menu::Menu(std::string heading, std::vector<std::string> options) {
    this->heading = heading;
    this->options = options;
}

void Menu::display_menu(int selected) {
    Interactions::clear_screen();
    Interactions::println(underline(bold(heading)));
    Interactions::println("");
    for (size_t i = 0; i < options.size(); ++i) {
        if (i == (size_t)selected) {
            Interactions::println("\033[47m\033[30m" + std::to_string(i + 1) + ". " + options[i] + "\033[0m");
        } else {
            Interactions::println(std::to_string(i + 1) + ". " + options[i]);
        }
    }
    Interactions::println(italic("( Press ctrl-c or DEL key to exit )"));
    Interactions::draw();
}

int Menu::navigate() {
    Interactions::enable_raw_mode();
    int selected = 0, exit_status = 0;
    while (!exit_status) {
        display_menu(selected);
        int key = Interactions::get_key();
        switch (key) {
        case Keys::ARROW_UP:
            selected = (selected == 0) ? (int)options.size() - 1 : selected - 1;
            break;
        case Keys::ARROW_DOWN:
            selected = (selected == (int)options.size() - 1) ? 0 : selected + 1;
            break;
        case Keys::HOME_KEY:
            selected = 0;
            break;
        case Keys::END_KEY:
            selected = (int)options.size() - 1;
            break;
        case Keys::SUB_KEY:
            exit_status = 1;
            break;
        case Keys::EXIT_KEY:
            selected = -1;
            exit_status = 1;
            break;
        }
    }
    Interactions::clear_screen();
    Interactions::disable_raw_mode();
    return selected;
}