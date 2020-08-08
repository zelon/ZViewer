// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#define NOMINMAX
/// if Visual Studio .NET 2005, disable 6011 warning because of STL warning
#if _MSC_VER == 1400
#pragma warning(disable:6011)
#endif

#ifndef STRICT
#define STRICT
#endif

// 아래 지정된 플랫폼에 우선하는 플랫폼을 대상으로 해야 한다면 다음 정의를 수정하십시오.
// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.

#ifndef WINVER
#define WINVER 0x0501		///< 0x0501 = WinXP
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501	///< 0x0500 = WinXP
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0500 ///< 0x0500 = WinME
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0600	// Internet Explorer 6.0
#endif

#define _ATL_APARTMENT_THREADED
#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

// ATL의 일반적이며 대개 안전하게 무시되는 일부 경고 메시지 숨김을 해제합니다.
#define _ATL_ALL_WARNINGS

#include <afxdisp.h>

#include "resource.h"
#include <atlcom.h>

#include <afxOle.h>
#include <atlconv.h>  // for ATL string conversion macros

#include <strsafe.h>

#include <algorithm>
#include <cassert>
#include <fstream>
#include <list>
#include <map>
#include <mutex>
#include <sstream>
#include <vector>

#pragma warning(push)
#pragma warning(disable:4819)
#include <FreeImagePlus.h>
#pragma warning(pop)

#include "../commonSrc/CommonDefine.h"

using namespace ATL;
