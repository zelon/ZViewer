
#pragma once

#include <windows.h>
#include <fstream>
#include <string>

class CUnicodeFile
{
public:

	enum eFileOpenMode
	{
		eFileOpenMode_READ,
		eFileOpenMode_WRITE,
	};

	enum eBoomCode
	{
		eBoomCode_BIGENDIAN    = 0xfffe,
		eBoomCode_LITTLEENDIAN = 0xfeff,
	};

	CUnicodeFile()
	{

	}

	~CUnicodeFile()
	{

	}

	/// 파일을 연다.
	bool open(const tstring & strFileName, eFileOpenMode openMode);

	/// 파일의 내용을 한줄 얻어온다. \r, \n 은 포함하지 않는다.
	bool getLine(std::wstring & str);

	bool getLine(std::string & str);

	void write(const std::wstring & str)
	{
		m_outputStream.write((char*)str.c_str(), (std::streamsize)(str.size() * sizeof(wchar_t)));
	}

	void write(const std::string & str)
	{
		m_outputStream.write((char*)str.c_str(), (std::streamsize)(str.size() * sizeof(char)));
	}

	void writeLine(const tstring & str)
	{
		write(str);
		write(TEXT("\r\n"));
	}

protected:

	/// 엔디안을 변경한다. 빅엔디안 <-> 리틀엔디안
	const wchar_t _changeEndian(const wchar_t src)
	{
		BYTE hiByte = HIBYTE(((WORD)src));
		BYTE loByte = LOBYTE(((WORD)src));

		return  (wchar_t)((((WORD)(loByte)) << 8) | ((WORD)(hiByte)));
	}

	std::ifstream m_inputStream;		///< 파일로부터 읽기 위한 스트림
	std::ofstream m_outputStream;		///< 파일에 쓰기 위한 스트림

	eFileOpenMode m_openMode;

	bool m_bBigEndian;
};