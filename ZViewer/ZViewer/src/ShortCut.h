
#pragma once

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

	ShortCutResult DoShortCut(WPARAM wParam);

	HACCEL MakeAccelTable();

private:
	void initializeShortCutData(void);

	void insertShortCutData(unsigned short key, int sendid);
	void insertShortCutData(unsigned short modifier, unsigned short key, int sendid);
	void insertShortCutData(unsigned short modifier1, unsigned short modifier2, unsigned short key, int sendid);

	class ShortCutData
	{
	public:
		std::vector < unsigned short > m_modifierKeys;
		unsigned short m_key;
		int m_sendID;

		const bool isAllKeyMatched(const WPARAM pressedKey) const;
		const bool isModifierKeyMatched() const;
	};

	std::vector < ShortCutData > m_shortcutData;
};

