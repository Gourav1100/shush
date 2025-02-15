#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

#include <exception>
#include <string>

class Exception : public std::exception {
  private:
    std::string message;

  public:
    Exception(std::string);
    const char *what() const throw();
};

#endif