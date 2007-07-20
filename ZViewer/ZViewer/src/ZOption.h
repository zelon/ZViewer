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

#include "CommonFunc.h"
#include "OptionFile.h"

class StringToValue
{
	enum eValueType
	{
		eValueType_INT,
		eValueType_BOOL,
	};

public:
	StringToValue(tstring & val, int * iValue)
	{
		_Init(val, iValue, eValueType_INT);
	}

	StringToValue(tstring & val, bool * bValue)
	{
		_Init(val, bValue, eValueType_BOOL);
	}


	void InsertValueToMap(iniMap & mapData)
	{
		switch ( m_valueType )
		{
		case eValueType_BOOL:
			{
				bool * pData = (bool*)m_pData;
				mapData[m_str] = (*pData) ? TEXT("true") : TEXT("false");

			}
			break;

		case eValueType_INT:
			{
				int * pData = (int*)m_pData;
				mapData[m_str] = toString(*pData);
			}
			break;
		default:
			_ASSERTE(false);
		}
	}

	void InsertMapToValue(iniMap & mapData)
	{
		switch ( m_valueType )
		{
		case eValueType_BOOL:
			{
				iniMap::const_iterator it = mapData.find(m_str);

				if ( it == mapData.end() )
				{
					return;
				}

				const tstring value = it->second;

				bool * pData = (bool*)m_pData;
				*pData = (it->second == TEXT("true"));

			}
			break;

		case eValueType_INT:
			{
				iniMap::const_iterator it = mapData.find(m_str);

				if ( it == mapData.end() )
				{
					return;
				}

				const tstring value = it->second;

				int * pData = (int*)m_pData;
				*pData = _tstoi(value.c_str());
			}
			break;
		default:
			_ASSERTE(false);
		}
	}

	const tstring & getString() const { return m_str; }
protected:

	void _Init(tstring & str, void * pData, eValueType valueType)
	{
		m_str = str;
		m_valueType = valueType;
		m_pData = pData;
	}

	void * m_pData;
	tstring m_str;
	eValueType m_valueType;
};


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

	bool IsFullScreen() const
	{
		return m_bFullScreen;
	}
	void SetFullScreen(bool bFullScreen) { m_bFullScreen = bFullScreen; }

	int GetMaxCacheMemoryMB() const { return m_iMaximumCacheMemoryMB; }
	void SetMaxCacheMemoryMB(const int iMB) { m_iMaximumCacheMemoryMB = iMB; }

	const bool IsLoopImages() const { return m_bLoopImages; }
	void ToggleLoopImages() { m_bLoopImages = !m_bLoopImages; }

	const bool IsBigToSmallStretchImage() const { return m_bBigToSmallStretchImage; }
	void ToggleBigToSmallStretchImage() { m_bBigToSmallStretchImage = !m_bBigToSmallStretchImage; }

	const bool IsSmallToBigStretchImage() const { return m_bSmallToBigStretchImage; }
	void ToggleSmallToBigStretchImage() { m_bSmallToBigStretchImage = !m_bSmallToBigStretchImage; }

	const bool IsUseCache() const { return m_bUseCache; }
	//-----------------------------------------------------------
	// 아래는 프로그램 구동 후에 임시로 쓰이는 옵션들. 파일에 저장하지 않는다.

	//// 그림의 시작 위치. 일본 만화의 경우 우측 상단부터 시작하기 위해서...
	bool m_bRightTopFirstDraw;

	/// 한 화면에 2장의 그림을 보여줄 것인가.
	bool m_bTwoInSaveView;

	/// 최대 캐시할 이미지 갯수. 여기서 절반반큼 왼쪽, 오른쪽으로 간다.
	int m_iMaxCacheImageNum;

protected:
	/// 전체 화면 모드인가.m_bOptionChanged(false)
	bool m_bFullScreen;

private:
	//----------------------------------------------------------
	// 아래는 지속적으로 저장되는 옵션들. 파일에 저장한다.

	/// 캐쉬 시스템을 쓸것인가... 메모리가 적은 컴퓨터에서는 끌 수 있도록 하기 위해서
	bool m_bUseCache;

	/// 마지막 그림 파일에서 다음으로 가면 처음 그림 파일로 갈 것인가?
	bool m_bLoopImages;

	/// 화면보다 큰 그림을 화면에 맞게 축소할 것인가
	bool m_bBigToSmallStretchImage;

	/// 화면보다 작은 그림을 화면에 맞게 확대할 것인가.
	bool m_bSmallToBigStretchImage;

	/// 캐시되는 최대 메모리량
	int m_iMaximumCacheMemoryMB;

	tstring m_strOptionFilename;

	void SetDefaultOption();

	void LoadFromFile();
	void SaveToFile();

	void SetSaveOptions();
protected:
	/// 파일로부터 옵션을 읽어들여서 바뀐 옵션이 있나.
	bool m_bOptionChanged;

	std::vector < StringToValue > m_saveOptions;

};

