#include "exception.h"

Exception::Exception(std::string message) { this->message = message; }

const char *Exception::what() const throw() { return message.c_str(); }