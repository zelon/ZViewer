
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
