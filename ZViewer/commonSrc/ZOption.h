/*********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZOption.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include <vector>

#include "CommonFunc.h"
#include "OptionFile.h"
#include "StringToValue.h"

class ZOption
{
	ZOption();

	void _InsertSaveOptionSetting(TCHAR * str, bool * value)
	{
		tstring val(str);
		StringToValue v(val, value);
		m_saveOptions.push_back(v);
	}

	void _InsertSaveOptionSetting(TCHAR * str, int * value)
	{
		tstring val(str);
		StringToValue v(val, value);
		m_saveOptions.push_back(v);
	}

public:
	static ZOption & GetInstance();

	~ZOption()
	{
		SaveToFile();
	}

	/// 기본 옵션을 파일로부터 불러온다.
	void LoadOption();

	bool IsFullScreen() const
	{
		return m_bFullScreen;
	}

	void SetDontSave() { m_bDontSaveInstance = true; }
	void SetFullScreen(bool bFullScreen) { m_bFullScreen = bFullScreen; }

	int GetMaxCacheMemoryMB() const { return m_iMaximumCacheMemoryMB; }
	void SetMaxCacheMemoryMB(const int iMB) { m_iMaximumCacheMemoryMB = iMB; }

	const bool IsLoopImages() const { return m_bLoopImages; }
	void ToggleLoopImages() { m_bLoopImages = !m_bLoopImages; }

	const bool IsUseDebug() const { return m_bUseDebug;}

	const bool IsBigToSmallStretchImage() const { return m_bBigToSmallStretchImage; }
	void ToggleBigToSmallStretchImage() { m_bBigToSmallStretchImage = !m_bBigToSmallStretchImage; }

	const bool IsSmallToBigStretchImage() const { return m_bSmallToBigStretchImage; }
	void ToggleSmallToBigStretchImage() { m_bSmallToBigStretchImage = !m_bSmallToBigStretchImage; }

	const bool IsUseAutoRotation() const { return m_bUseAutoRotation; }
	void SetAutoRotation(const bool bUseAutoRotation) { m_bUseAutoRotation = bUseAutoRotation; }

	const bool IsUsingPreviewModeInShell() const { return m_bUsePreviewInShell; }
	const bool IsUsingOpenCMDInShell() const { return m_bUseOpenCMDInShell; }
	//-----------------------------------------------------------
	// 아래는 프로그램 구동 후에 임시로 쓰이는 옵션들. 파일에 저장하지 않는다.

	//// 그림의 시작 위치. 일본 만화의 경우 우측 상단부터 시작하기 위해서...
	bool m_bRightTopFirstDraw;

	/// 한 화면에 2장의 그림을 보여줄 것인가.
	bool m_bTwoInSaveView;

	/// 최대 캐시할 이미지 갯수. 여기서 절반반큼 왼쪽, 오른쪽으로 간다.
	int m_iMaxCacheImageNum;

	/// 슬라이드 모드인가
	bool m_bSlideMode;

	/// 슬라이드 모드의 시간(mili seconds)
	DWORD m_iSlideModePeriodMiliSeconds;

	DWORD m_dwLastSlidedTime;

	/// Always on Top window
	bool m_bAlwaysOnTop;
protected:

	/// Is Full screen mode?
	bool m_bFullScreen;

private:
	//----------------------------------------------------------
	// 아래는 지속적으로 저장되는 옵션들. 파일에 저장한다.

	/// exif 정보에 따른 자동 회전을 할 것인가.
	bool m_bUseAutoRotation;

	/// 마지막 그림 파일에서 다음으로 가면 처음 그림 파일로 갈 것인가?
	bool m_bLoopImages;

	/// 디버그 정보를 남기나
	bool m_bUseDebug;

	/// 화면보다 큰 그림을 화면에 맞게 축소할 것인가
	bool m_bBigToSmallStretchImage;

	/// 화면보다 작은 그림을 화면에 맞게 확대할 것인가.
	bool m_bSmallToBigStretchImage;

	/// 캐시되는 최대 메모리량
	int m_iMaximumCacheMemoryMB;

	tstring m_strOptionFilename;

	/// Shell 에서 미리 보기를 사용할 것인가.
	bool m_bUsePreviewInShell;

	/// Shell 에서 cmd 창 열기를 사용할 것인가.
	bool m_bUseOpenCMDInShell;

	/// 기본적인 옵션을 설정해둔다. 설치 후 처음 실행되었을 때 이 값을 기준으로 zviewer.ini 파일이 만들어진다.
	void SetDefaultOption();

	void LoadFromFile();
	void SaveToFile();

	/// Make the table that indicate which key-value is mapped to which variable
	void InitializeOptionMappingTable();

	/// 이 값이 false 이면 singleton 을 끝낼 때 값을 저장하지 않는다(ZviewerAgent 의 종료 시점을 정확히 알 수 없기 때문)
	bool m_bDontSaveInstance;

protected:
	/// 파일로부터 옵션을 읽어들여서 바뀐 옵션이 있나.
	bool m_bOptionChanged;

	std::vector < StringToValue > m_saveOptions;

};

