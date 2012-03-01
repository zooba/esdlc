#pragma once

#include <amp.h>
#undef max
#undef min
#include <string>

namespace esdl
{
    extern concurrency::accelerator_view acc;

    int getch();
    bool equals(const std::wstring& x, const wchar_t* y);

    inline bool equals(const std::wstring& x, const wchar_t* y, const wchar_t* z) {
        return equals(x, y) || equals(x, z);
    }

    inline bool equals(const std::wstring& x, const wchar_t* y, const wchar_t* z, const wchar_t* w) {
        return equals(x, y, z) || equals(x, w);
    }

    inline bool equals(const std::wstring& x, const std::wstring& y) { return equals(x, y.c_str()); }


    template<typename GeneType> struct scale_value
    {
        static GeneType scale(float value, GeneType highest, GeneType lowest) restrict(cpu, amp) {
            return (GeneType)(value * (highest - lowest));
        }
    };
}
