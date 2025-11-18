// Stdafx.h :

#pragma once

#ifndef VC_EXTRALEAN		
#define VC_EXTRALEAN		// Exclude rarely-used stuff from windows headers
#endif

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

#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>
#include <afxadv.h>
#include <afxtempl.h>

#include <ddeml.h>
#include <direct.h>

#include <shlobj.h>
#include <shlwapi.h>
#include <commdlg.h>

#include <cfloat>
#include <cmath>

#if defined(GL_FUNCTIONALITY)
#include <gl\gl.h>
#include <gl\glu.h>
#endif // GL_FUNCTIONALITY

#define tasSTR2(x) #x
#define tasSTR(x) tasSTR2(x)
#define tasMSG(desc) message(__FILE__ "(" tasSTR(__LINE__) ") : " #desc)

#define ODA_FUNCTIONALITY 0

#if ODA_FUNCTIONALITY
#include "ad2.h"
#include "odio.h"
#endif

#include "resource.h"
#include "Messages.h"

#include "ExpProcs.h"
UINT AFXAPI HashKey(CString& str);

#include "SafeMath.h"

#include "Pnt.h"
#include "Pnt4.h"
#include "Vec.h"
#include "Vec4.h"
#include "Mat4.h"
#include "TMat.h"
#include "RefSys.h"
#include "Viewport.h"
#include "AbstractView.h"
#include "ModelView.h"
#include "ModelTransform.h"
#include "PenStyle.h"
#include "Line.h"
#include "Polyline.h"
#include "CharCellDef.h"
#include "FontDef.h"
#include "Prim.h"
#include "PrimArc.h"
#include "PrimBSpline.h"
#include "PrimCSpline.h"
#include "PrimInsert.h"
#include "PrimLine.h"
#include "PrimMark.h"
#include "PrimPolygon.h"
#include "PrimPolyline.h"
#include "PrimSegRef.h"
#include "PrimTag.h"
#include "PrimText.h"
#include "PrimDim.h"
#include "Seg.h"
#include "Block.h"
#include "Segs.h"
#include "Layer.h"
#include "SegsDet.h"
#include "SegsTrap.h"
#include "PrimState.h"
#include "FilePeg.h"
#include "FileTracing.h"

#include "OpenGL.h"

