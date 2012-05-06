#pragma once

#include <memory>
#include <string>
#include <sstream>

namespace esdl
{
    struct Arg
    {
        Arg() : isValid(false), isCommand(false) { }
        
        bool isValid;
        operator bool() { return isValid; }
        bool isCommand;
        std::wstring variable;
        std::wstring value;

        template<typename T>
        Arg& operator>>(T& dest) {
            std::wistringstream(value) >> dest;
            return *this;
        };
    };

    Arg get_command(int argc, wchar_t** argv, int& argi)
    {
        if (argi <= 0) argi = 1;
        if (argi >= argc) return Arg();

        Arg result;
        result.isValid = true;
        result.isCommand = false;
        result.variable = argv[argi++];
        auto colon = result.variable.find_first_of(':');
        auto equals = result.variable.find_first_of('=');
        if (colon == result.variable.npos) {
            colon = equals;
        }
        if (colon != result.variable.npos) {
            if (equals != result.variable.npos && equals < colon) {
                colon = equals;
            }
            result.value = result.variable.substr(colon + 1);
            result.variable.resize(colon);
        }

        if (result.variable[0] == '/' || result.variable[0] == '-') {
            result.variable = result.variable.substr(1);
            result.isCommand = true;
        }

        return result;
    }
}
