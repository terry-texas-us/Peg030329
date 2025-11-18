#pragma once

LRESULT CALLBACK SubProcFixup(HWND, UINT, WPARAM, LPARAM);

namespace fixup
{
	extern double dAxTol;
	extern double dSiz;
	
	bool SetOptions(double* dSiz, double* dAxTol);
}
