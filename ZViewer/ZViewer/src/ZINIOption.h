#pragma once

#include <map>

class ZINIOption
{
public:
	ZINIOption();
	~ZINIOption();

	/// 데이터를 읽어들여서 맵을 구성한다.
	void LoadFromFile(const std::string & strFilename);
	void SaveToFile(const std::string & strFilename);

	const std::string GetValue(const std::string & strKey);
private:
	std::map < std::string, std::string > m_iniMap;
};