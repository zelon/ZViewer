
#pragma once

#include <Windows.h>

#include <vector>


enum ShortCutResult
{
	ShortCutResult_EXECUTED,
	ShortCutResult_NOT_EXECUTED,
};

class ShortCut
{
public:
	static ShortCut & GetInstance(void);
	ShortCut();

	bool CheckModifier(const std::vector < unsigned short > & modifier);
	ShortCutResult DoShortCut(WPARAM wParam);

	HACCEL MakeAccelTable();

protected:
	void InitializeShortCutData(void);

	void insertShortCutData(unsigned short key, int sendid);
	void insertShortCutData(unsigned short modifier, unsigned short key, int sendid);
	void insertShortCutData(unsigned short modifier1, unsigned short modifier2, unsigned short key, int sendid);

	class ShortCutData
	{
	public:
		std::vector < unsigned short > m_modifier;
		unsigned short m_key;
		int m_sendID;
	};

	std::vector < ShortCutData > m_shortcutData;
};

