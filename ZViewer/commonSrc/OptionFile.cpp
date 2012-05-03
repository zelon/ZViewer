/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	OptionFile.cpp
*
*                                       http://www.wimy.com
*********************************************************************/

#include "stdafx.h"
#include "OptionFile.h"
#include "CommonFunc.h"
#include "UnicodeFile.h"
#include <fstream>

using namespace std;

/// 데이터 파일을 읽어들여서 맵을 구성한다.
bool COptionFile::LoadFromFile(const tstring & strFilename, iniMap & settings)
{
	CUnicodeFile file;
	if ( false == file.open(strFilename, CUnicodeFile::eFileOpenMode_READ) )
	{
		return false;
	}

	tstring aLine;

	while ( file.getLine(aLine) )
	{
		if ( aLine.empty() ) continue;

		if ( aLine[0] == L'#' || aLine[0] == L'/' )	// 처음 바이트가 # 이거나 / 이면 무시한다.
		{
			continue;
		}

		if ( aLine.size() <= 3 )	// 데이터로 볼 수 없는 길이면 무시한다.
		{
			continue;
		}

		tstring::size_type pos = aLine.find(TEXT("="));

		if ( pos == aLine.npos )	// 처음 '=' 을 찾을 수 없으면 이상한 줄이다.
		{
			continue;
		}

		tstring strValue = aLine.substr(pos+1);
		tstring strKey = aLine.substr(0, pos);

		assert(strKey.empty() == false);

		settings[strKey] = strValue;
	}
	return true;
}

/// 맵 내용을 데이터 파일에 쓴다.
void COptionFile::SaveToFile(const tstring & strFilename, const iniMap & settings)
{
	CUnicodeFile file;
	file.open(strFilename, CUnicodeFile::eFileOpenMode_WRITE);
	
	iniMap::const_iterator it;

	for ( it = settings.begin(); it != settings.end(); ++it )
	{
		const tstring key = it->first;
		const tstring value = it->second;

		tstring line = key;
		line += TEXT("=");
		line += value;

		file.writeLine(line);
	}
}
