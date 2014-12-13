#include "StdAfx.h"
#include ".\logmanager.h"
#include "CommonFunc.h"

CLogManager & CLogManager::getInstance() {
	static CLogManager inst;
	return inst;
}

CLogManager::CLogManager(void) {
	m_bGoOn = true;
#ifdef USE_CONSOLE
	if ( !AllocConsole() )
	{
		const DWORD errorCode = GetLastError();

		MessageBox(NULL, toString(errorCode).c_str(), TEXT("Error"), MB_OK);
		MessageBox(NULL, TEXT("AllocConsole Error!!!"), TEXT("Error") , MB_OK);
	}
	else
	{
		m_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}
#endif
}

#ifdef USE_CONSOLE
void CLogManager::Output(const TCHAR * fmt, ...)
{
	if ( false == m_bGoOn ) return;

	va_list			argptr;
	TCHAR cBuf[512];
	int				iCnt;
	DWORD			dwWritten;

	// 가변 인자 정리
	va_start(argptr, fmt);
	iCnt = StringCbVPrintf(cBuf, sizeof(cBuf) / sizeof(TCHAR), fmt, argptr);
	va_end(argptr);

	if ( S_OK != iCnt )
	{
		assert(false);
	}
	CLockObjUtil lock(m_logLock);

	// 콘솔 윈도우에 출력
	WriteConsole(m_hConsoleOutput, cBuf, (DWORD)(_tcslen(cBuf)), &dwWritten, NULL);
	WriteConsole(m_hConsoleOutput, "\n", 1, &dwWritten, NULL);
}
#else
void CLogManager::Output(const TCHAR * , ...) {
  return;
}
#endif
