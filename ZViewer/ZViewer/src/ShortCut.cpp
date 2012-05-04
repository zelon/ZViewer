
#include "stdafx.h"

#include "ShortCut.h"

#include "../commonSrc/CommonFunc.h"
#include "src/ZMain.h"
#include "../commonSrc/ZOption.h"

ShortCutResult ShortCut::DoShortCut(const WPARAM pressedKey)
{
	switch ( pressedKey )
	{
	case 'E':
	case VK_OEM_COMMA:
		{
			if ( IsPressedVirtualKey(VK_CONTROL) )
			{
				ZMain::GetInstance().PrevFolder();
			}
			else
			{
				ZMain::GetInstance().PrevImage();
			}
		}
		break;

	case 'R':
	case VK_OEM_PERIOD:
		{
			if ( IsPressedVirtualKey(VK_CONTROL) )
			{
				ZMain::GetInstance().NextFolder();
			}
			else
			{
				ZMain::GetInstance().NextImage();
			}
		}
		break;

	case VK_ESCAPE:
		{
			if ( ZOption::GetInstance().IsFullScreen() )	// 현재 풀스크린이면 원래 화면으로 돌아간다.
			{
				ZMain::GetInstance().ToggleFullScreen();
			}
			else
			{
				ZMain::GetInstance().CloseProgram();
			}
		}
		break;

	default:
		return ShortCutResult_NOT_EXECUTED;
	}
	return ShortCutResult_EXECUTED;
}