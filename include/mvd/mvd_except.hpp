#ifndef MVD_EXCEPT_HPP
#define MVD_EXCEPT_HPP

#include <stdexcept>
#include <string>


class MVDParserException : public std::runtime_error{
public:
    MVDParserException(const std::string & str) : std::runtime_error(str) {}
};

#endif // MVD_EXCEPT_HPP

