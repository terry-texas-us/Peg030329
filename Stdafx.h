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

#define WIN32_LEAN_AND_MEAN      // Exclude rarely-used services from Windows.h
#define NOMINMAX                 // Prevent Windows.h from defining min/max macros
#define STRICT                   // Enforce stricter type safety

//#include <Windows.h>

//#include <afx.h>
//#include <afxadv.h>
//#include <afxext.h>
//#include <afxdisp.h>
//#include <afxtempl.h>
#include <afxwin.h>
//#include <commdlg.h>
//#include <ddeml.h>
//#include <direct.h>
//#include <shlobj.h>
//#include <shlwapi.h>

//#include <algorithm>
//#include <array>
//#include <cctype>
//#include <cfloat>
//#include <cmath>
//#include <iomanip>
//#include <memory>
//#include <map>
//#include <sstream>
#include <string>
//#include <unordered_map>
//#include <utility>
//#include <vector>

#if GL_FUNCTIONALITY
#include <gl\gl.h>
#include <gl\glu.h>
#endif

#if ODA_FUNCTIONALITY
#include "ad2.h"
#include "odio.h"
#endif

#include "resource.h"

//#include "AbstractView.h"
//#include "Block.h"
//#include "Pnt.h"
//#include "Pnt4.h"
//#include "Mat4.h"
//#include "Messages.h"
//#include "RefSys.h"
//#include "Viewport.h"
//#include "ModelView.h"
//#include "ModelTransform.h"
//#include "PenStyle.h"
//#include "Line.h"
//#include "Polyline.h"
//#include "CharCellDef.h"
//#include "FilePeg.h"
//#include "FileTracing.h"
//#include "FontDef.h"
//#include "Layer.h"
//#include "OpenGL.h"
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
//#include "PrimState.h"
//#include "PrimTag.h"
//#include "PrimText.h"
//#include "PrimDim.h"
//#include "SafeMath.h"
//#include "Seg.h"
//#include "Segs.h"
//#include "SegsDet.h"
//#include "SegsTrap.h"
//#include "TMat.h"
//#include "Vec.h"
//#include "Vec4.h"

UINT AFXAPI HashKey(CString& str);