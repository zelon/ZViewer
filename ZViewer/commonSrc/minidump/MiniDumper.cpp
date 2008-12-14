
#include "stdafx.h"
#include <TCHAR.H>
#include "MiniDumper.h"
#include "../CommonFunc.h"

MiniDumper * pThis = NULL;

MiniDumper::MiniDumper(const TCHAR * szDumpFilename, const TCHAR * szDumpMsg)
{
	pThis = this;

	SPrintf(m_szDumpFilename, FILENAME_MAX, szDumpFilename);
	SPrintf(m_szDumpMsg, FILENAME_MAX, szDumpMsg);

	::SetUnhandledExceptionFilter( TopLevelFilter );
}

MiniDumper::~MiniDumper()
{
}

/// #ref : http://serious-code.net/moin.cgi/MiniDump
tstring GetFaultReason(PEXCEPTION_POINTERS exPtrs)
{
    if (::IsBadReadPtr(exPtrs, sizeof(EXCEPTION_POINTERS))) 
        return _T("bad exception pointers");

    // 간단한 에러 코드라면 그냥 변환할 수 있다.
    switch (exPtrs->ExceptionRecord->ExceptionCode)
    {
    case EXCEPTION_ACCESS_VIOLATION:         return _T("EXCEPTION_ACCESS_VIOLATION");
    case EXCEPTION_DATATYPE_MISALIGNMENT:    return _T("EXCEPTION_DATATYPE_MISALIGNMENT");
    case EXCEPTION_BREAKPOINT:               return _T("EXCEPTION_BREAKPOINT");
    case EXCEPTION_SINGLE_STEP:              return _T("EXCEPTION_SINGLE_STEP");
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return _T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
    case EXCEPTION_FLT_DENORMAL_OPERAND:     return _T("EXCEPTION_FLT_DENORMAL_OPERAND");
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return _T("EXCEPTION_FLT_DIVIDE_BY_ZERO");
    case EXCEPTION_FLT_INEXACT_RESULT:       return _T("EXCEPTION_FLT_INEXACT_RESULT");
    case EXCEPTION_FLT_INVALID_OPERATION:    return _T("EXCEPTION_FLT_INVALID_OPERATION");
    case EXCEPTION_FLT_OVERFLOW:             return _T("EXCEPTION_FLT_OVERFLOW");
    case EXCEPTION_FLT_STACK_CHECK:          return _T("EXCEPTION_FLT_STACK_CHECK");
    case EXCEPTION_FLT_UNDERFLOW:            return _T("EXCEPTION_FLT_UNDERFLOW");
    case EXCEPTION_INT_DIVIDE_BY_ZERO:       return _T("EXCEPTION_INT_DIVIDE_BY_ZERO");
    case EXCEPTION_INT_OVERFLOW:             return _T("EXCEPTION_INT_OVERFLOW");
    case EXCEPTION_PRIV_INSTRUCTION:         return _T("EXCEPTION_PRIV_INSTRUCTION");
    case EXCEPTION_IN_PAGE_ERROR:            return _T("EXCEPTION_IN_PAGE_ERROR");
    case EXCEPTION_ILLEGAL_INSTRUCTION:      return _T("EXCEPTION_ILLEGAL_INSTRUCTION");
    case EXCEPTION_NONCONTINUABLE_EXCEPTION: return _T("EXCEPTION_NONCONTINUABLE_EXCEPTION");
    case EXCEPTION_STACK_OVERFLOW:           return _T("EXCEPTION_STACK_OVERFLOW");
    case EXCEPTION_INVALID_DISPOSITION:      return _T("EXCEPTION_INVALID_DISPOSITION");
    case EXCEPTION_GUARD_PAGE:               return _T("EXCEPTION_GUARD_PAGE");
    case EXCEPTION_INVALID_HANDLE:           return _T("EXCEPTION_INVALID_HANDLE");
        //case EXCEPTION_POSSIBLE_DEADLOCK:        return _T("EXCEPTION_POSSIBLE_DEADLOCK");
    case CONTROL_C_EXIT:                     return _T("CONTROL_C_EXIT");
    case 0xE06D7363:                         return _T("Microsoft C++ Exception");
    default:
        break;
    }

    // 뭔가 좀 더 복잡한 에러라면...
    static TCHAR szFaultReason[2048];
    SPrintf(szFaultReason, 2048, _T("Unknown")); 
    ::FormatMessage(
        FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
        ::GetModuleHandle(_T("ntdll.dll")),
        exPtrs->ExceptionRecord->ExceptionCode, 
        0,
        szFaultReason,
        0,
        NULL);

    return szFaultReason;
}

LONG MiniDumper::TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	TCHAR szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		tstring folder = GetFolderNameFromFullFileName(szDbgHelpPath);
		folder += TEXT("dbghelp.dll");
		hDll = ::LoadLibrary( folder.c_str() );
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( TEXT("DBGHELP.DLL") );
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			TCHAR szDumpPath[_MAX_PATH];
			TCHAR szScratch [_MAX_PATH];

			SPrintf( szDumpPath, _MAX_PATH, TEXT("%s"), pThis->m_szDumpFilename);

			// ask the user if they want to save a dump file
			//if (::MessageBox( NULL, "Something bad happened in your program, would you like to save a diagnostic file?", m_szAppName, MB_YESNO )==IDYES)
			{
				// create the file
				HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
					FILE_ATTRIBUTE_NORMAL, NULL );

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, NULL, NULL );
					::CloseHandle(hFile);
					if (bOK)
					{
						TCHAR szErrorReason[256];
						SPrintf(szErrorReason, 256, TEXT("Error : %s"), GetFaultReason(pExceptionInfo).c_str());
						MessageBox(HWND_DESKTOP, szErrorReason, TEXT("ZViewer"), MB_OK);

						MessageBox(HWND_DESKTOP, pThis->m_szDumpMsg, TEXT("ZViewer"), MB_OK);

						SPrintf( szScratch, _MAX_PATH, TEXT("Saved dump file to '%s'"), szDumpPath );
						szResult = szScratch;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						SPrintf( szScratch, _MAX_PATH, TEXT("Failed to save dump file to '%s' (error %d)"), szDumpPath, GetLastError() );
						szResult = szScratch;
					}
				}
				else
				{
					SPrintf( szScratch, _MAX_PATH, TEXT("Failed to create dump file '%s' (error %d)"), szDumpPath, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = TEXT("DBGHELP.DLL too old");
		}
	}
	else
	{
		szResult = TEXT("DBGHELP.DLL not found");
	}

	if (szResult)
	{
		//	::MessageBox( NULL, szResult, m_szAppName, MB_OK );
	}

	return retval;
}
