
#include "stdafx.h"
#include "OptionFile.h"
#include <fstream>

using namespace std;

void COptionFile::LoadFromFile(const std::string & strFilename, iniMap & settings)
{
	ifstream fin(strFilename.c_str());

	char szData[256];
	while ( !fin.eof())
	{
		memset(szData, 0, sizeof(szData));

		fin.getline(szData, 256);

		if ( szData[0] != '\0' )	/// 데이터를 받은게 있다.
		{
			if ( szData[0] == '#' || szData[0] == '/' )	// 처음 바이트가 # 이거나 / 이면 무시한다.
			{
				continue;
			}

			if ( strlen(szData) <= 3 )	// 데이터로 볼 수 없는 길이면 무시한다.
			{
				continue;
			}

			std::string strData = szData;

			std::string::size_type pos = strData.find("=");

			if ( pos == strData.npos )	// 처음 '=' 을 찾을 수 없으면 이상한 줄이다.
			{
				continue;
			}

			std::string strValue = strData.substr(pos+1);

			strData.resize(pos);
			std::string strKey = strData;

			_ASSERTE(strKey.empty() == false);
			_ASSERTE(strValue.empty() == false);

			settings[strKey] = strValue;
		}
	}
}

void COptionFile::SaveToFile(const std::string & strFilename, const iniMap & settings)
{
	
}
