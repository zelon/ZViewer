// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

/// if Visual Studio .NET 2005, disable 6011 warning because of STL warning
#if _MSC_VER == 1400
#pragma warning(disable:6011)
#endif

#include <windows.h>
#include <cassert>
#include <CommCtrl.h>
#include <WindowsX.h>
#include <Shlwapi.h>

// STL Headers
#include <vector>
#include <list>
#include <deque>
#include <map>
#include <string>
#include <functional>
#include <algorithm>
#include <tchar.h>

#ifdef _MSC_VER
#include <strsafe.h>
#endif

#include <cassert>

#include <shlobj.h>
#include <io.h>

#include "../lib/freeimage/FreeImagePlus.h"

#include "../commonSrc/CommonDefine.h"
