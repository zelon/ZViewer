/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZResourceManager.h
*
*                                       http://www.wimy.com
*********************************************************************/


#pragma once

/// 다중 언어 지원을 위해 HINSTANCE 및 리소스를 관리하는 클래스
class ZResourceManager
{
	ZResourceManager();

public:
	~ZResourceManager();

	static ZResourceManager & GetInstance();

	void SetHandleInstance(HINSTANCE hInstance)
	{
		m_hInstance = hInstance;
	}

	HINSTANCE GetHInstance() const
	{
		return m_hInstance;
	}

	//const tstring GetString(UINT iStringID);

	HINSTANCE m_hInstance;

private:
	std::map < UINT, tstring > m_stringMap;	/// ID 에 해당하는 String 을 캐시한다.
};

