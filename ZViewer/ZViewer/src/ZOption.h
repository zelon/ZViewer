
#pragma once

#include "ZINIOption.h"

class ZOption
{
public:

	ZOption()
	{
		SetDefaultOption();
	}

	/// 전체 화면 모드인가.
	bool m_bFullScreen;

	/// 화면보다 큰 그림을 화면에 맞게 축소할 것인가
	bool m_bBigToSmallStretchImage;

	/// 화면보다 작은 그림을 화면에 맞게 확대할 것인가.
	bool m_bSmallToBigStretchImage;

	//// 그림의 시작 위치. 일본 만화의 경우 우측 상당부터 시작하기 위해서...
	bool m_bRightTopFirstDraw;

	void SetDefaultOption();
};