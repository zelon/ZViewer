/* ------------------------------------------------------------------------
 *
 * Copyright 2006 by zelon
 *
 * LockUtil.h
 *
 * 2006. 3. 11 Jinwook Kim
 *
 * ------------------------------------------------------------------------
 */


#pragma once

#include <mutex>
#include <windows.h>

/// 윈도우의 Event 를 쉽게 쓰게 하는 유틸리티 클래스
class CEventObj
{
public:
	CEventObj()
	{
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	}

	~CEventObj()
	{
		CloseHandle(m_hEvent);
	}

	bool setEvent()
	{
		return SetEvent(m_hEvent) == TRUE;
	}

	DWORD wait()
	{
		return WaitForSingleObject(m_hEvent, INFINITE);
	}

protected:
	/// 내부적으로 가지고 있는 이벤트 핸들
	HANDLE m_hEvent;

};

#ifdef USE_WIN32_LOCK_UTIL
/// 윈도우의 크리티컬 섹션을 쉽게 쓰게 하는 유틸리티 클래스
class CLockObj
{
public:
	CLockObj()
	{
		InitializeCriticalSection(&m_cs);
	}

	~CLockObj()
	{
		DeleteCriticalSection(&m_cs);
	}

protected:
	/// 내부적으로 가지고 있는 크리티컬 섹션
	CRITICAL_SECTION m_cs;

private:

	friend class CLockObjUtil;

	inline void lock()
	{
		EnterCriticalSection(&m_cs);
	}

	inline void unlock()
	{
		LeaveCriticalSection(&m_cs);
	}
};

/// lock, unlock 을 쉽게 하기 위해 도와주는 유틸리티 클래스
class CLockObjUtil
{
public:
	CLockObjUtil(CLockObj & lockObj)
		: m_lockObj(lockObj)
	{
		lockObj.lock();
	}

	~CLockObjUtil()
	{
		m_lockObj.unlock();
	}

protected:
	/// 생성자에서 받은 크리티컬 섹션을 내부적으로 가지고 있는 멤버 변수
	CLockObj & m_lockObj;

	/// 할당 연산자가 쓰이는 걸 방지하는 코드. 할당 연산자가 쓰이게 되면 오류 발생하게 됨
	CLockObjUtil & operator=(const CLockObjUtil & obj);
};
#else
typedef std::recursive_mutex CLockObj;
typedef std::lock_guard<std::recursive_mutex> CLockObjUtil;
#endif


