
#include "stdafx.h"
#include "UnicodeFile.h"

/// 파일을 연다.
bool CUnicodeFile::open(const tstring & strFileName, eFileOpenMode openMode)
{
	char filename[FILENAME_MAX] = { 0 };

#ifdef _UNICODE
	WideCharToMultiByte(CP_THREAD_ACP, 0, strFileName.c_str(), (int)strFileName.size(), filename, 256, NULL, NULL);
#else
	SPrintf(filename, FILENAME_MAX, strFileName.c_str());
#endif

	if ( openMode == eFileOpenMode_READ )
	{
		m_inputStream.open(filename, std::ios_base::binary);

		if ( m_inputStream.is_open() == false )
		{
			return false;
		}

		m_inputStream.seekg(-1, std::ios_base::end);

		size_t pos = m_inputStream.tellg();
		if ( pos < 2 || pos % 2 != 1)
		{
			assert(!"not unicode file");
			m_inputStream.close();
			return false;
		}
		m_inputStream.seekg(0, std::ios_base::beg);

		wchar_t boom;
		m_inputStream.read((char*)&boom, sizeof(boom));

		switch ( boom )
		{
		case eBoomCode_LITTLEENDIAN:
			m_bBigEndian = false;
			break;

		case eBoomCode_BIGENDIAN:
			m_bBigEndian = true;
			break;

		default:
			m_inputStream.close();
			return false;
		}
	}
	else if ( openMode == eFileOpenMode_WRITE )
	{
		m_outputStream.open(filename, std::ios_base::binary);

		wchar_t boom = eBoomCode_LITTLEENDIAN;
		m_outputStream.write((char*)&boom, sizeof(boom));

		m_bBigEndian = false;
	}

	return true;
}


bool CUnicodeFile::getLine(std::wstring & str)
{
	// 멀티바이트 버퍼에 한줄씩 얻어 담아야 한다.
	str.resize(0);
	wchar_t aCh = L'\0';

	if ( m_inputStream.eof() )
	{
		return false;
	}

	while( m_inputStream.eof() == false )
	{
		m_inputStream.read((char*)&aCh,2);

		/// 더이상 못 읽었으면 이 줄은 끝난 것이다.
		if ( -1 == m_inputStream.tellg() ) break;

		if( m_bBigEndian )
		{
			// 빅엔디안이라면 리틀엔디안으로 변환한다.
			// 인텔계열은 리틀엔디안이기때문이다.
			aCh = _changeEndian(aCh);
		}

		if (EOF == aCh )// 파일의 끝인경우 
		{
			if(str.empty())
			{
				return false;
			}		

			break;
		}

		/// end line 이 나오면 이 줄은 끝난 것이다.
		if( L'\n' == aCh)
		{
			break;
		}
		else if( L'\r' == aCh )
		{
			continue;
		}

		str += aCh;
	}

	return true;
}


bool CUnicodeFile::getLine(std::string & str)
{
	if ( m_inputStream.eof() ) return false;

	char buff[256];
	m_inputStream.getline(buff, 256);

	str = buff;
	return true;
}