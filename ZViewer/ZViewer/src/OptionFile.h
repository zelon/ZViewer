#pragma once

#include <map>

typedef std::map < std::string, std::string > iniMap;

class COptionFile
{

public:
	COptionFile();
	~COptionFile();

	/// 데이터를 읽어들여서 맵을 구성한다.
	static void LoadFromFile(const std::string & strFilename, iniMap & settings);
	static void SaveToFile(const std::string & strFilename, const iniMap & settings);

private:
};