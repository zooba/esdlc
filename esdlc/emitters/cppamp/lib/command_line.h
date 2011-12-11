#pragma once

#include <memory>
#include <string>
#include <sstream>

namespace esdl
{
    struct Arg
    {
        std::wstring command;
        std::wstring variable;
        std::wistringstream value;
    };

    std::shared_ptr<Arg> get_command(int argc, wchar_t** argv, int& argi)
    {
        if (argi <= 0) argi = 1;
        if (argi >= argc) return nullptr;

        auto text = argv[argi++];

        auto pResult = std::make_shared<Arg>();
        Arg& r = *pResult;

        if (text[0] == '/' || text[0] == '-') {
            r.command = text + 1;
            auto colon = r.command.find_first_of(':');
            auto equals = r.command.find_first_of('=');
            if (colon == r.command.npos && equals == r.command.npos) {
                if (argi < argc) {
                    r.value.str(argv[argi++]);
                    // TODO: Fix if followed by variable name
                    if (r.value.str()[0] == '/' || r.value.str()[1] == '-') {
                        r.value.str(L"");
                        --argi;
                    }
                } else {
                    return nullptr;
                }
            } else {
                if (colon == r.command.npos || (equals != r.command.npos && equals < colon)) {
                    colon = equals;
                }
                r.value.str(r.command.substr(colon + 1));
                r.command.resize(colon);
            }
        } else {
            r.variable = text;
            auto colon = r.variable.find_first_of(':');
            auto equals = r.variable.find_first_of('=');
            if (colon == r.variable.npos && equals == r.variable.npos) {
                if (argi < argc) {
                    r.value.str(argv[argi++]);
                } else {
                    return nullptr;
                }
            } else {
                if (colon == r.variable.npos || (equals != r.variable.npos && equals < colon)) {
                    colon = equals;
                }
                r.value.str(r.variable.substr(colon + 1));
                r.variable.resize(colon);
            }
        }

        return pResult;
    }
}
