
#pragma once

class ShortCut
{
public:
	static ShortCut & GetInstance(void);
	ShortCut();

	HACCEL MakeAccelTable();

private:
	enum ModifierKeyType
	{
		CTRL,
		SHIFT,
		ALT,
	};

	void initializeShortCutData(void);

	void insertShortCutData(const WORD key, WORD sendid);
	void insertShortCutData(const ModifierKeyType modifier, const WORD key, WORD sendid);
	void insertShortCutData(const ModifierKeyType modifier1, const ModifierKeyType modifier2, const WORD key, WORD sendid);

	class ShortCutData
	{
	public:
		std::vector < const ModifierKeyType > m_modifierKeys;
		WORD m_key;
		WORD m_sendID;

		const bool isAllKeyMatched(const WPARAM pressedKey) const;
		const bool isModifierKeyMatched() const;
	};

	std::vector < ShortCutData > m_shortcutData;
};

