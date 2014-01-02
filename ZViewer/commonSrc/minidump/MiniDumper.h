
#include "dbghelp.h"

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

class MiniDumper
{
public:
	MiniDumper(const TCHAR * szDumpFilename = TEXT("Minidump.dmp"), const TCHAR * szDumpMsg = TEXT("Oops! Crash!"));
	~MiniDumper();

//private:///< 덤프 상황에서 최대한 함수 호출을 줄이기 위해 private 를 쓰지 않음
	TCHAR m_szDumpFilename[FILENAME_MAX];
	TCHAR m_szDumpMsg[FILENAME_MAX];
	static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );
};
