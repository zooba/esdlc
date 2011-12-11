#include "utility.h"
#include <conio.h>
#include <iostream>

namespace { concurrency::accelerator _acc(concurrency::accelerator::default_accelerator); }
concurrency::accelerator_view esdl::acc = _acc.create_view(concurrency::queuing_mode::deferred);


#include <Windows.h>

bool esdl::equals(const std::wstring& x, const wchar_t* y) {
    return CompareStringEx(NULL, 3, x.c_str(), -1, y, -1, NULL, NULL, NULL) == 2;
}

namespace {
    bool _isRedirected() {
        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
        if (hStdin == INVALID_HANDLE_VALUE)
            return true;
        DWORD type = GetFileType(hStdin);
        if (type == FILE_TYPE_UNKNOWN ||
            (type & ~FILE_TYPE_REMOTE) != FILE_TYPE_CHAR ||
            GetConsoleMode(hStdin, &type) == 0)
            return true;
        return false;
    }

    bool isRedirected = _isRedirected();
}

int esdl::getch() {
    if (isRedirected)
        return std::cin.get();
    else
        return _getch();
}
