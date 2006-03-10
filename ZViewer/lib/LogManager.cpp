/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * LogManager.cpp
 *
 * 2006. 3. 11 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#include "StdAfx.h"
#include ".\logmanager.h"

CLogManager & CLogManager::getInstance()
{
	static CLogManager inst;
	return inst;
}


CLogManager::CLogManager(void)
{
	m_bGoOn = true;
#ifdef _DEBUG
	
	if ( !AllocConsole() )
	{
		DWORD errorCode = GetLastError();

		char temp[20];
		wsprintf(temp, "%d", errorCode);
		MessageBox(NULL, temp, "Error", MB_OK);

		MessageBox(NULL, "AllocConsole Error!!!", "Error" , MB_OK);
	}
	else
	{
		m_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	}
#endif
}

CLogManager::~CLogManager(void)
{
}


void CLogManager::Output(const char * fmt, ...)
{
	if ( false == m_bGoOn ) return;
#ifndef _DEBUG
	return;
#endif

	va_list			argptr;
	char			cBuf[512];
	int				iCnt;
	DWORD			dwWritten;

	// 가변 인자 정리
	va_start(argptr, fmt);
	iCnt = vsprintf(cBuf, fmt, argptr);
	va_end(argptr);

	CLockObjUtil lock(m_logLock);

	// 콘솔 윈도우에 출력
	WriteConsole(m_hConsoleOutput, cBuf, (DWORD)(strlen(cBuf)), &dwWritten, NULL);
	WriteConsole(m_hConsoleOutput, "\n", 1, &dwWritten, NULL);
}

