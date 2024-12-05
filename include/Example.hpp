#include <string>

class Example {
   private:
    std::string str;

   public:
    Example(std::string s) : str(s) {}
    std::string get_string() const;
};