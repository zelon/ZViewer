/********************************************************************
*
* Created by zelon(Kim, Jinwook Korea)
* 
*   2005. 5. 7
*	ZImage.h
*
*                                       http://www.wimy.com
*********************************************************************/

#pragma once

#include <strsafe.h>
#include "../lib/FreeImagePlus.h"
#include "CommonFunc.h"

/// FreeImagePlus 의 클래스인 fipWinImage 를 래핑하여 함수 등을 쓰기 쉽게 하는 클래스
class ZImage
{
public:
	static bool StartupLibrary(){ return true; }
	static bool CleanupLibrary(){ return true; }
	static bool IsValidImageFileExt(const TCHAR * szFilename);
	static const char * GetLibraryVersion()
	{
		return FreeImage_GetVersion();
	}

	void clear()
	{
		m_image.clear();
	}

	WORD GetWidth() { return m_image.getWidth(); }
	WORD GetHeight() { return m_image.getHeight(); }

	WORD GetOriginalWidth() { return m_originalWidth; }
	WORD GetOriginalHeight() { return m_originalHeight; }

	WORD GetBPP() { return m_image.getBitsPerPixel(); }
	BYTE * GetData() { return m_image.accessPixels(); }
	BITMAPINFO * GetBitmapInfo() { return m_image.getInfo(); }
	bool IsValid() { return (m_image.isValid() == TRUE); }
	bool LoadFromFile(const tstring & strFilename)
	{
		//DebugPrintf("LoadFromFile : %s", strFilename.c_str());

		char buffer[256] = {0};

#ifdef _UNICODE
		if ( 0 == WideCharToMultiByte(CP_THREAD_ACP, 0, strFilename.c_str(), (int)strFilename.size(), buffer, 256, NULL, NULL) )
		{
			DWORD dwError = GetLastError();
			_ASSERTE(false);
			return false;
		}
#else
		StringCchPrintf(buffer, sizeof(buffer), strFilename.c_str());
#endif

		try
		{
			if ( m_image.load(buffer) == TRUE )
			{
				m_originalWidth = m_image.getWidth();
				m_originalHeight = m_image.getHeight();
				m_originalSize = m_image.getImageSize();
				return true;
			}
		}
		catch ( ... )
		{
#pragma message("TODO : 이 부분에 exception 이 발생하는 이유를 찾아야함...")
			_ASSERTE(false);
			return false;
		}
		return false;
	}

	bool SaveToFile(const tstring & strFilename, int iFlag)
	{
#ifdef _UNICODE
		char buffer[256];
		WideCharToMultiByte(CP_THREAD_ACP, 0, strFilename.c_str(), (int)strFilename.size(), buffer, 256, NULL, NULL);
		return ( TRUE == m_image.save(buffer, iFlag));
#else
		return ( TRUE == m_image.save(strFilename.c_str(), iFlag));
#endif

	}

	void Rotate(double dAngle) { m_image.rotate(dAngle); }

	bool Resize(WORD newWidth, WORD newHeight)
	{
		return ( m_image.rescale(newWidth, newHeight, FILTER_BOX) == TRUE );
	}

	bool ConvertTo32Bit() { return ( TRUE == m_image.convertTo32Bits() ); }

	long GetImageSize()
	{
		return m_image.getImageSize();
	}

	long GetOriginalImageSize()
	{
		return m_originalSize;
	}

	bool isTransparent()
	{
		return (m_image.isTransparent() == TRUE);
	}

	const fipWinImage & getFipImage()
	{
		return m_image;
	}

private:
	fipWinImage m_image;

	/// Image's original width - before resizing
	WORD m_originalWidth;

	/// Image's original height - before resizing
	WORD m_originalHeight;

	/// Image's original size - before resizing
	long m_originalSize;
};