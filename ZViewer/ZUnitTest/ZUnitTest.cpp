// ZUnitTest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include <iostream>
#include "../commonSrc/CommonFunc.h"

using namespace std;

bool g_bSuccess = true;

void TouchUnitTestFlag()
{
	tstring strFileName = TEXT("..\\ZUnitTest\\UnitTestFlag.cpp");
	HANDLE hFile = CreateFile(strFileName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,  NULL);

	SYSTEMTIME  now;

	FILETIME  tsUserTime;
	GetSystemTime(&now);
	if(!SystemTimeToFileTime(&now, &tsUserTime))
	{
		assert(false);
		//PrintError(_T("SystemTimeToFileTime()"), GetLastError());
		return;
	}

	SetFileTime(hFile, &tsUserTime, &tsUserTime, &tsUserTime);
	CloseHandle(hFile);
}

void PrintErrorMsg(const tstring & strMsg)
{
	tstring strErrorMsg = TEXT("ZUnitTest: UnitTest error:");
	strErrorMsg += strMsg;
	strErrorMsg += TEXT(" : UnitTest failed");

	wcout << strErrorMsg.c_str() << endl;

	g_bSuccess = false;
}

void TEST_GetOnlyFileNameWithoutExt()
{
	tstring strFilename = TEXT("C:\\test\\aaa.bmp");
	tstring strResult = GetOnlyFileNameWithoutExt(strFilename);

	if ( strResult != TEXT("aaa") )
	{
		PrintErrorMsg(TEXT("TEST_GetOnlyFileNameWithoutExt failed"));
	}
}

void TEST_toString()
{
	if ( toString(500) != TEXT("500") ) PrintErrorMsg(TEXT("toString() failed"));
}

int _tmain(int argc, _TCHAR* argv[])
{
	TEST_GetOnlyFileNameWithoutExt();

	/// TODO
	/*
	TEST_GetResizedRectForBigToSmall(const RECT & MaximumSize, const RECT & originalSize);

	TEST_GetResizedRectForSmallToBig(const RECT & MaximumSize, const RECT & originalSize);

	*/

	TEST_toString();

	if ( g_bSuccess )
	{
		return 0;
	}

	/// 결과가 failed 이면 UnitTestFlag.cpp 파일을 touch 하여 다시 빌드하게 한다.
	TouchUnitTestFlag();
	return 1;
}
