#pragma once

bool FilenameCompare(const std::string & a, const std::string & b);
const std::string GetOnlyFileName(const std::string & strFullFileName);
bool StringCompare(const std::string & a, const std::string & b);

void DebugPrintf( const char *fmt, ... );

bool SetRegistryValue(HKEY hOpenKey, const std::string & strKey,LPCTSTR szValue, const std::string & strData);

// 최대 크기를 넘지 않는 적당한 리사이즈 크기를 돌려준다.
RECT GetResizedRect(const RECT & MaximumSize, const RECT & originalSize);

std::string toString(int i);

bool SelectFolder(HWND hWnd, char * szFolder);

std::string GetFolderNameFromFullFileName(const std::string & strFullFilename);
std::string GetFileNameFromFullFileName(const std::string & strFullFilename);
