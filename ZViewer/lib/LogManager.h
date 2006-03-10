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

class CLogManager
{
	CLogManager(void);
public:
	static CLogManager & getInstance();
	~CLogManager(void);

	void Output(const char *fmt, ... );

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

