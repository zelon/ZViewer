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

#include "FreeImagePlus.h"
#include "CommonFunc.h"

class IImage
{
public:

	virtual WORD GetWidth() = 0;
	virtual WORD GetHeight() = 0;
	virtual WORD GetBPP() = 0;
	virtual BYTE * GetData() = 0;
	virtual BITMAPINFO * GetBitmapInfo() = 0;
	virtual bool IsValid() = 0;
	virtual bool LoadFromFile(const std::string & strFilename) = 0;
	virtual bool Resize(WORD newWidth, WORD newHeight) = 0;
	virtual bool ConvertTo32Bit() = 0;

};


class ZImage : public IImage
{
public:

public:
	static bool StartupLibrary(){ return true; }
	static bool CleanupLibrary(){ return true; }
	static bool IsValidImageFileExt(const char * szFilename);
	static const char * GetLibraryVersion()
	{
		return FreeImage_GetVersion();
	}

	WORD GetWidth() { return m_image.getWidth(); }
	WORD GetHeight() { return m_image.getHeight(); }
	WORD GetBPP() { return m_image.getBitsPerPixel(); }
	BYTE * GetData() { return m_image.accessPixels(); }
	BITMAPINFO * GetBitmapInfo() { return m_image.getInfo(); }
	bool IsValid() { return (m_image.isValid() == TRUE); }
	bool LoadFromFile(const std::string & strFilename)
	{
		DebugPrintf("LoadFromFile : %s", strFilename.c_str());

		try
		{
			if ( m_image.load(strFilename.c_str()) == TRUE )
			{
				return true;
			}
		}
		catch ( ... )
		{
#pragma message("TODO : 이 부분에 exception 이 발생하는 이유를 찾아야함...")
			return false;
		}
		return false;
	}

	bool SaveToFile(const std::string & strFilename, int iFlag)
	{
		return ( TRUE == m_image.save(strFilename.c_str(), iFlag));
	}

	inline void Rotate(double dAngle) { m_image.rotate(dAngle);	}

	bool Resize(WORD newWidth, WORD newHeight)
	{
		return ( m_image.rescale(newWidth, newHeight, FILTER_BOX) == TRUE );
	}

	bool ConvertTo32Bit() { return ( TRUE == m_image.convertTo32Bits() ); }

	long GetImageSize()
	{
		return m_image.getImageSize();
	}

private:
	fipImage m_image;








	/*
	ZImage()
	{
	m_bValid = false;
	}

	static bool StartupLibrary()
	{
	ilInit();
	iluInit();
	ilutRenderer(ILUT_WIN32);

	return true;
	}

	static bool CleanupLibrary()
	{
	}

	WORD GetWidth()
	{
	return m_devilimage.Width();
	}
	WORD GetHeight()
	{
	return m_devilimage.Height();
	}
	WORD GetBPP()
	{
	return m_devilimage.Bpp();
	}
	BYTE * GetData()
	{
	return ilWin32::GetPadData(m_devilimage);
	}
	BITMAPINFO * GetBitmapInfo()
	{
	ilWin32::GetInfo(m_devilimage, &m_info);
	return &m_info;
	}
	bool IsValid()
	{
	return ( m_bValid );
	}
	bool LoadFromFile(const std::string & strFilename)
	{
	char szFilename[MAX_PATH];
	_snprintf(szFilename, sizeof(szFilename), strFilename.c_str());

	if ( m_devilimage.Load(szFilename, IL_TYPE_UNKNOWN) == TRUE )
	{
	m_bValid = true;
	}
	return m_bValid;
	}
	bool Resize(WORD newWidth, WORD newHeight)
	{
	return ( m_devilimage.Resize(newWidth, newHeight,3) == TRUE);
	}
	bool ConvertTo32Bit()
	{
	return true;
	}

	private:
	ilImage m_devilimage;
	BITMAPINFO m_info;
	bool m_bValid;
	*/
};