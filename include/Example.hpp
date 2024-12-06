#ifndef EXAMPLE_HPP
#define EXAMPLE_HPP

#include <string>

class Example {
   private:
    std::string str;

   public:
    Example(std::string s) : str(s) {}
    std::string get_string() const;
};

#endif
