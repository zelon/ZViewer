/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZOption.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once


class ZOption
{
	ZOption()
	{
		m_strOptionFilename = "option.ini";
		m_bOptionChanged = false;
		SetDefaultOption();

		LoadFromFile();
	}

public:
	static ZOption & GetInstance();

	~ZOption()
	{
		SaveToFile();
	}

	bool IsFullScreen() const
	{
		return m_bFullScreen;
	}
	void SetFullScreen(bool bFullScreen) { m_bFullScreen = bFullScreen; }

	//-----------------------------------------------------------
	// 아래는 프로그램 구동 후에 임시로 쓰이는 옵션들. 파일에 저장하지 않는다.

	/// 화면보다 큰 그림을 화면에 맞게 축소할 것인가
	bool m_bBigToSmallStretchImage;

	/// 화면보다 작은 그림을 화면에 맞게 확대할 것인가.
	bool m_bSmallToBigStretchImage;

	//// 그림의 시작 위치. 일본 만화의 경우 우측 상단부터 시작하기 위해서...
	bool m_bRightTopFirstDraw;

	/// 한 화면에 2장의 그림을 보여줄 것인가.
	bool m_bTwoInSaveView;

protected:
	/// 전체 화면 모드인가.m_bOptionChanged(false)
	bool m_bFullScreen;

private:
	//----------------------------------------------------------
	// 아래는 지속적으로 저장되는 옵션들. 파일에 저장한다.

	/// 최대 캐시되는 메모리
	int m_iMaximumCacheMemory;

	/// 최대 캐시되는 그림 파일
	int m_iMaximumCacheFileNum;

	std::string m_strOptionFilename;

	void SetDefaultOption();

	void LoadFromFile();
	void SaveToFile();

protected:
	/// 파일로부터 옵션을 읽어들여서 바뀐 옵션이 있나.
	bool m_bOptionChanged;

};