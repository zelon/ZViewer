#include "stdafx.h"
#include "CommonFunc.h"

void DebugPrintf( const char *fmt, ... )
{
#ifdef _DEBUG
	va_list v;
	char buf[1024*4];
	int len;

	va_start( v, fmt );
	len = wvsprintf( buf, fmt, v );
	va_end( v );

	OutputDebugString( buf );
	OutputDebugString( "\r\n" );
#endif
}

bool SetRegistryValue(HKEY hOpenKey, const std::string & strKey,LPCTSTR szValue, const std::string & strData)
{
	if( !hOpenKey || strKey.empty() || !szValue)
	{
		_ASSERTE(!"SetRegistryValue invalid arg");
		return false;
	}

	bool bRetVal = false;
	DWORD dwDisposition;
	HKEY hTempKey = NULL;

	if( ERROR_SUCCESS == ::RegCreateKeyEx(hOpenKey, strKey.c_str(), NULL,
		NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hTempKey, &dwDisposition) )
	{
		// 마지막 \0 까지 포함해야한다던데;;
		DWORD	dwBufferLength = (DWORD)strData.size() + 1;

		if( ERROR_SUCCESS == ::RegSetValueEx(hTempKey, (LPTSTR)szValue,
			NULL, REG_SZ, (const BYTE *)strData.c_str(), dwBufferLength) )
		{
			bRetVal = true;
		}
	}

	if( hTempKey )
	{
		::RegCloseKey(hTempKey);
	}

	return bRetVal;
}

