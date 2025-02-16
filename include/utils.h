#ifndef __UTILS_H__
#define __UTILS_H__

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

inline std::string to_lower(std::string str) {
    std::string result = "";
    for (char c : str) {
        result += tolower(c);
    }
    return result;
};

inline std::string bold(std::string str) { return "\033[1m" + str + "\033[0m"; };

inline std::string italic(std::string str) { return "\033[3m" + str + "\033[0m"; };

inline std::string underline(std::string str) { return "\033[4m" + str + "\033[0m"; };

inline void print_tabular(std::vector<std::string> headers, std::vector<std::vector<std::string>> data,
                          std::string NO_DATA_MESSAGE = "No data available") {
    if (headers.size() == 0) {
        return;
    }
    std::vector<int> column_widths;
    for (int i = 0; i < (int)headers.size(); i++) {
        column_widths.push_back(headers[i].size());
    }
    for (std::vector<std::string> row : data) {
        for (int i = 0; i < (int)row.size(); i++) {
            column_widths[i] = std::max(column_widths[i], (int)row[i].size());
        }
    }
    int total_width = 2;
    for (int width : column_widths) {
        total_width += width + 5;
    }
    total_width--;
    if (data.size() == 0 && column_widths[0] < (int)NO_DATA_MESSAGE.size()) {
        int remaining = (int)NO_DATA_MESSAGE.size() - column_widths[0];
        if (remaining < 0) {
            remaining = 0;
        }
        total_width += remaining;
        column_widths[0] += remaining;
    }
    for (int i = 0; i < total_width; i++) {
        std::cout << "-";
    }
    std::cout << std::endl;
    std::cout << "| ";
    for (int i = 0; i < (int)headers.size(); i++) {
        std::cout << bold(headers[i]);
        for (int j = 0; j < (int)column_widths[i] - (int)headers[i].size() + 2; j++) {
            std::cout << " ";
        }
        std::cout << " | ";
    }
    std::cout << std::endl;
    for (int i = 0; i < total_width; i++) {
        std::cout << "-";
    }
    std::cout << std::endl;
    if (data.size() == 0) {
        std::cout << "| " << NO_DATA_MESSAGE;
        for (int i = 0; i < total_width - (int)std::string(NO_DATA_MESSAGE).size() - 4; i++) {
            std::cout << " ";
        }
        std::cout << " |" << std::endl;
        for (int i = 0; i < total_width; i++) {
            std::cout << "-";
        }
        std::cout << std::endl;
        return;
    }
    for (std::vector<std::string> row : data) {
        std::cout << "| ";
        for (int i = 0; i < (int)row.size(); i++) {
            std::cout << italic(row[i]);
            for (int j = 0; j < (int)column_widths[i] - (int)row[i].size() + 2; j++) {
                std::cout << " ";
            }
            std::cout << " | ";
        }
        std::cout << std::endl;
    }
    for (int i = 0; i < total_width; i++) {
        std::cout << "-";
    }
    std::cout << std::endl;
};

inline bool prompt(std::string message) {
    std::string response;
    std::cout << message << bold(" (y/n): ");
    std::cin >> response;
    return to_lower(response) == "y" || to_lower(response) == "yes";
};

inline std::string join(std::vector<std::string> strings, std::string delimiter) {
    std::string result = "";
    for (int i = 0; i < (int)strings.size(); i++) {
        result += strings[i];
        if (i < (int)strings.size() - 1) {
            result += delimiter;
        }
    }
    return result;
};

#endif