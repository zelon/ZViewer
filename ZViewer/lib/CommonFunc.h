#pragma once

void DebugPrintf( const char *fmt, ... );

bool SetRegistryValue(HKEY hOpenKey, const std::string & strKey,LPCTSTR szValue, const std::string & strData);