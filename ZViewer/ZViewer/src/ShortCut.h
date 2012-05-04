
#pragma once

#include <Windows.h>

enum ShortCutResult
{
	ShortCutResult_EXECUTED,
	ShortCutResult_NOT_EXECUTED,
};

class ShortCut
{
public:
	static ShortCutResult DoShortCut(WPARAM wParam);
};
