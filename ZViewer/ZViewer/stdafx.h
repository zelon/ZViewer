/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*
*                                       http://www.wimy.com
*********************************************************************/

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
#include <vector>

#include "freeimage/FreeImagePlus.h"

#ifdef _DEBUG
#include "vld/vld.h"
#endif

#include "CommonDefine.h"
