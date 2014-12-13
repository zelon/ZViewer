#pragma once

#include "LockUtil.h"

#ifdef _DEBUG
#define USE_CONSOLE
#endif

/// 디버깅을 쉽게 하기 위한 클래스. 콘솔 윈도우 기능을 가지고 있다.
class CLogManager final
{
public:
	static CLogManager & getInstance();

	void Output(const TCHAR *fmt, ... );

	void CleanUp() {
		m_bGoOn = false;
	}

private:
  CLogManager(void);

	HANDLE m_hConsoleOutput;

	CLockObj m_logLock;

	/// If program exit, false
	bool m_bGoOn;
};

