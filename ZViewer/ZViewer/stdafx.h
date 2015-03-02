#pragma once

/// _WIN32_WINNT means NT 4.0. If you want for above Win2000, use 0x0500. Check http://a.tk.co.kr/75
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <CommCtrl.h>
#include <WindowsX.h>
#include <shlobj.h>
#include <tchar.h>
#include <strsafe.h>
#include <VersionHelpers.h>

#include <fstream>

// STL Headers
#include <algorithm>
#include <cassert>
#include <chrono>
#include <deque>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "freeimage/FreeImagePlus.h"

#if defined(_M_IX86)
#ifdef _DEBUG
#include "vld/vld.h"
#endif
#elif defined(_M_X64)
// don't use vld
#else
static_assert(false, "unknown architecture");
#endif

#include "CommonDefine.h"
