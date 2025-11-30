#pragma once

#ifndef WINVER				// Allow use of features specific to Windows 10 or later.
#define WINVER 0x0A00		
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows 10 or later.
#define _WIN32_WINNT 0x0A00	
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 10 or later
#define _WIN32_IE 0x0A00	
#endif

#define _AFX_ALL_WARNINGS

#define tasSTR2(x) #x
#define tasSTR(x) tasSTR2(x)
#define tasMSG(desc) message(__FILE__ "(" tasSTR(__LINE__) ") : " #desc)

#define ODA_FUNCTIONALITY 0 // This functionality is dead forever. Do not enable it. Use to find related code only.
#define GL_FUNCTIONALITY 0

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used services from windows.h
#define NOMINMAX                 // Prevent windows.h from defining min/max macros
#define STRICT                   // Enforce stricter type safety

#include <windows.h>

#include <afx.h> // Core MFC GUI classes
//#include <Windows.h>
//#include <vector> // Dynamic contiguous array; the most frequently used container in modern C++. Extremely compile-time expensive – always pre-include.
#include <string> // std::string and std::wstring; owns null-terminated character data with SSO. Essential for almost all text handling.
//#include <memory> //Smart pointers (std::unique_ptr, std::shared_ptr, std::weak_ptr), std::make_unique, std::make_shared, and std::allocator.
//#include <array> // std::array<T,N>; fixed-size contiguous array with STL interface and no dynamic allocation.
//#include <unordered_map> // Hash table (std::unordered_map, std::unordered_set); average O(1) lookup/insert. Very common for caches and dictionaries.
//#include <map> // Red - black tree(std::map, std::set); ordered associative containers with O(log n) operations.
//#include <algorithm> // 100+ generic algorithms (std::sort, std::find, std::transform, std::copy, etc.). Used in almost every project.
//#include <utility> // std::move, std::forward, std::pair, std::exchange, std::in_place, etc. Core building blocks of modern C++.

//#include <afxext.h>
//#include <afxdisp.h>
//#include <afxadv.h>
//#include <afxtempl.h>

//#include <ddeml.h>
//#include <direct.h>

//#include <shlobj.h>
//#include <shlwapi.h>
//#include <commdlg.h>

//#include <sstream>
//#include <iomanip>
//#include <cctype>

//#include <cfloat>
//#include <cmath>

#if GL_FUNCTIONALITY
#include <gl\gl.h>
#include <gl\glu.h>
#endif

#if ODA_FUNCTIONALITY
#include "ad2.h"
#include "odio.h"
#endif

//#include "resource.h"
//#include "Messages.h"

//#include "ExpProcs.h"
UINT AFXAPI HashKey(CString& str);

//#include "SafeMath.h"

//#include "Pnt.h"
//#include "Pnt4.h"
//#include "Vec.h"
//#include "Vec4.h"
//#include "Mat4.h"
//#include "TMat.h"
//#include "RefSys.h"
//#include "Viewport.h"
//#include "AbstractView.h"
//#include "ModelView.h"
//#include "ModelTransform.h"
//#include "PenStyle.h"
//#include "Line.h"
//#include "Polyline.h"
//#include "CharCellDef.h"
//#include "FontDef.h"
//#include "Prim.h"
//#include "PrimArc.h"
//#include "PrimBSpline.h"
//#include "PrimCSpline.h"
//#include "PrimInsert.h"
//#include "PrimLine.h"
//#include "PrimMark.h"
//#include "PrimPolygon.h"
//#include "PrimPolyline.h"
//#include "PrimSegRef.h"
//#include "PrimTag.h"
//#include "PrimText.h"
//#include "PrimDim.h"
//#include "Block.h"
//#include "Layer.h"
//#include "Seg.h"
//#include "Segs.h"
//#include "SegsDet.h"
//#include "SegsTrap.h"
//#include "PrimState.h"
//#include "FilePeg.h"
//#include "FileTracing.h"

//#include "OpenGL.h"

