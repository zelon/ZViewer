#pragma once

class CDesktopWallPaper
{
public:
	enum eDesktopWallPaperStyle
	{
		eDesktopWallPaperStyle_CENTER,
		eDesktopWallPaperStyle_TILE,
		eDesktopWallPaperStyle_STRETCH
	};


public:
	CDesktopWallPaper(const std::string & strBitmapFilename)
	{
		m_strFileName = strBitmapFilename;
		m_eStyle = eDesktopWallPaperStyle_CENTER;
	}

	virtual ~CDesktopWallPaper(){}

	inline void SetWallPaperStyle(eDesktopWallPaperStyle style)
	{
		m_eStyle = style;
	}

	// 실제로 바탕화면을 바꾼다.
	void SetDesktopWallPaper();

protected:
	// 바탕화면으로 지정될 파일이름. bmp 여야 한다.
	std::string m_strFileName;
	eDesktopWallPaperStyle m_eStyle;
};