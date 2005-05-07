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

	const std::string GetString(UINT iStringID);

	HINSTANCE m_hInstance;

private:
	std::map < UINT, std::string > m_stringMap;	/// ID 에 해당하는 String 을 캐시한다.
};