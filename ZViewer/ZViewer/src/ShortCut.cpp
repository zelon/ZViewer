
#include "stdafx.h"

#include "ShortCut.h"

#include "../commonSrc/CommonFunc.h"
#include "resource.h"
#include "src/ZMain.h"

using namespace std;

#define VK_PAGE_UP		VK_PRIOR
#define VK_PAGE_DOWN	VK_NEXT
#define VK_X			(0x58)

const bool ShortCut::ShortCutData::isAllKeyMatched(const WPARAM pressedKey) const
{
	if (pressedKey != m_key)
	{
		return false;
	}

	return this->isModifierKeyMatched();
}

const bool ShortCut::ShortCutData::isModifierKeyMatched() const
{
	bool bNeedControl = false;
	bool bNeedShift = false;

	bool modifierOK = true;

	for (const auto & modifierKey : m_modifierKeys)
	{
		if (modifierKey == CTRL) bNeedControl = true;
		if (modifierKey == SHIFT) bNeedShift = true;
	}

	if (bNeedControl != IsPressedVirtualKey(VK_CONTROL))
	{
		modifierOK = false;
	}

	if (bNeedShift != IsPressedVirtualKey(VK_SHIFT))
	{
		modifierOK = false;
	}

	return modifierOK;
}

ShortCut& ShortCut::GetInstance(void)
{
	static ShortCut inst;
	return inst;
}

ShortCut::ShortCut()
{
	initializeShortCutData();
}

void ShortCut::insertShortCutData(const WORD key, const WORD sendid)
{
	ShortCutData data;
	data.m_key = key;
	data.m_sendID = sendid;

	m_shortcutData.push_back(data);
}

void ShortCut::insertShortCutData(const ModifierKeyType modifier, const WORD key, const WORD sendid)
{
	ShortCutData data;
	data.m_modifierKeys.push_back(modifier);
	data.m_key = key;
	data.m_sendID = sendid;

	m_shortcutData.push_back(data);
}

void ShortCut::insertShortCutData(const ModifierKeyType modifier1, const ModifierKeyType modifier2, const WORD key, const WORD sendid)
{
	ShortCutData data;
	data.m_modifierKeys.push_back(modifier1);
	data.m_modifierKeys.push_back(modifier2);
	data.m_key = key;
	data.m_sendID = sendid;

	m_shortcutData.push_back(data);
}

HACCEL ShortCut::MakeAccelTable()
{
	vector < ACCEL > accels;

	for ( const auto & data : m_shortcutData )
	{
		ACCEL aAccel = { 0, };

		for (const ModifierKeyType & modKey : data.m_modifierKeys)
		{
			if (modKey == CTRL)
			{
				aAccel.fVirt |= FCONTROL;
			}
			else if (modKey == SHIFT)
			{
				aAccel.fVirt |= FSHIFT;
			}
			else if (modKey == ALT)
			{
				aAccel.fVirt |= FALT;
			}
		}

		aAccel.fVirt |= FVIRTKEY;
		aAccel.key = data.m_key;
		aAccel.cmd = data.m_sendID;

		accels.push_back(aAccel);
	} 

	/// returned handle must be destroyed
	return CreateAcceleratorTable(&accels[0], static_cast<int>(accels.size()));
}

void ShortCut::initializeShortCutData(void)
{
	m_shortcutData.reserve(50);

	insertShortCutData(VK_DOWN,				ID_ACCELERATOR_DOWN);
    insertShortCutData(VK_LEFT,				ID_ACCELERATOR_LEFT);
    insertShortCutData(VK_RIGHT,			ID_ACCELERATOR_RIGHT);
    insertShortCutData(VK_UP,				ID_ACCELERATOR_UP);

	insertShortCutData(CTRL, 'C',			ID_COPY_TO_CLIPBOARD);

	insertShortCutData(SHIFT, VK_DELETE,	ID_DELETETHISFILE);
    insertShortCutData(VK_DELETE,			ID_DELETETHISFILE_RECYCLEBIN);

	insertShortCutData(CTRL, SHIFT, 'C',	ID_FILE_COPYTO);
	insertShortCutData(CTRL, SHIFT, 'M',	ID_FILE_MOVETO);

    insertShortCutData(VK_F1,				ID_HELP_ABOUT);
    insertShortCutData(VK_F5,				ID_FILE_RESCAN_FOLDER);
    insertShortCutData(VK_F9,				ID_SHOW_CACHED_FILENAME);

	insertShortCutData(CTRL, 'O',			ID_MAINMENU_FILE_OPEN);
	insertShortCutData(CTRL, 'S',			ID_FILE_SAVE_AS_NEW_EXT);

	insertShortCutData(CTRL, 'W',			ID_MAINMENU_FILE_EXIT);

	insertShortCutData(VK_HOME,				ID_MOVE_FIRSTIMAGE);
    insertShortCutData(VK_END,				ID_MOVE_LASTIMAGE);

	insertShortCutData(CTRL, VK_LEFT,		ID_MOVE_PREV_JUMP);
	insertShortCutData(CTRL, VK_RIGHT,		ID_MOVE_NEXT_JUMP);

	insertShortCutData(VK_PAGE_UP,			ID_MOVE_PREVIMAGE);
	insertShortCutData('E',					ID_MOVE_PREVIMAGE);

    insertShortCutData(VK_PAGE_DOWN,		ID_MOVE_NEXTIMAGE);
    insertShortCutData('R',					ID_MOVE_NEXTIMAGE);
    insertShortCutData(VK_SPACE,			ID_MOVE_NEXTIMAGE);

	insertShortCutData(CTRL, VK_PAGE_UP,	ID_MOVE_PREVFOLDER);
	insertShortCutData(CTRL, 'E',			ID_MOVE_PREVFOLDER);
	insertShortCutData(CTRL, VK_PAGE_DOWN,	ID_MOVE_NEXTFOLDER);
	insertShortCutData(CTRL, 'R',			ID_MOVE_NEXTFOLDER);

    insertShortCutData(SHIFT, VK_BACK,		ID_MOVE_REDOIMAGEPOSITION);
    insertShortCutData(VK_BACK,				ID_MOVE_UNDOIMAGEPOSITION);

	insertShortCutData(VK_RETURN,			ID_VIEW_FULLSCREEN);

    insertShortCutData(/*']'*/ VK_OEM_6,	ID_VIEW_ROTATECLOCKWISE);
    insertShortCutData(/*'['*/ VK_OEM_4,	ID_VIEW_ROTATECOUNTERCLOCKWISE);

	insertShortCutData(VK_ESCAPE,			ID_ESC_KEY_OPERATION);
	insertShortCutData(ALT, 'X',			ID_MAINMENU_FILE_EXIT);
}
