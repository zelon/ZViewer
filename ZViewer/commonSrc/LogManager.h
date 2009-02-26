/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * LogManager.h
 *
 * 2006. 3. 11 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include "LockUtil.h"

/// 디버깅을 쉽게 하기 위한 클래스. 콘솔 윈도우 기능을 가지고 있다.
class CLogManager
{
	CLogManager(void);
public:
	static CLogManager & getInstance();
	~CLogManager(void);

	void Output(const TCHAR *fmt, ... );

	void CleanUp()
	{
		m_bGoOn = false;
	}
protected:
	HANDLE			m_hConsoleOutput;			// Console Window Handle

	CLockObj m_logLock;

	/// If program exit, false
	bool m_bGoOn;
};

