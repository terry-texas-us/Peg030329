#pragma once

#include <Windows.h>

LRESULT CALLBACK SubProcFixup(HWND, UINT, WPARAM, LPARAM) noexcept;

namespace fixup
{
    extern double dAxTol;
    extern double dSiz;

    bool SetOptions(double* size, double* influenceAngle);
}
