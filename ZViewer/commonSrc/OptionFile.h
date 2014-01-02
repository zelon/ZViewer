/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	OptionFile.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

typedef std::map < tstring, tstring > iniMap;

class COptionFile
{

public:
	COptionFile();
	~COptionFile();

	/// 데이터 파일을 읽어들여서 맵을 구성한다.
	static bool LoadFromFile(const tstring & strFilename, iniMap & settings);

	/// 맵 내용을 데이터 파일에 쓴다.
	static void SaveToFile(const tstring & strFilename, const iniMap & settings);

private:
};