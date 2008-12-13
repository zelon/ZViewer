/* ------------------------------------------------------------------------
 *
 * CachdeData.h
 *
 * 2008.12.13 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include <map>

#include "../../commonSrc/ZImage.h"

typedef std::map < tstring, ZImage >::iterator CacheMapIterator;
typedef std::map < tstring, ZImage >::const_iterator CacheMapIterator_const;


class CachedData
{
public:
	CachedData() {}
	virtual ~CachedData(){}

	void Clear() { m_cacheData.clear(); }
	size_t Size() const { return m_cacheData.size(); }

	void PrintCachedData() const;

	bool IsEmpty() const
	{
		if ( m_cacheData.empty() ) return true;
		return false;
	}

	void InsertData(const tstring & strFilename, ZImage & image)
	{
		m_cacheData[strFilename] = image;
	}

	void ShowCacheInfo() const;

	bool HasCachedData(const int index) const;

	bool HasCachedData(const tstring & strFilename) const
	{
		return (m_cacheData.count(strFilename) > 0);
	}

	size_t GetIndex2FilenameMapSize() const { return m_imageIndex2FilenameMap.size(); }
	void SetNewFileList(const std::vector < FileData > & v, const size_t & numImageVectorSize);

	bool GetCachedData(const tstring & strFilename, ZImage & image) const;
	bool ClearFarthestDataFromCurrent(const int iFarthestIndex, long & cacheSize);

	/// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
	int GetFarthestIndexFromCurrentIndex(volatile const int & iCurrentIndex);

	tstring GetFilenameFromIndex(const int iIndex) { return m_imageIndex2FilenameMap[iIndex]; }

protected:
	std::map < tstring, ZImage > m_cacheData;		///< 실제로 캐쉬된 데이터를 가지고 있는 맵
	std::map < int , tstring > m_imageIndex2FilenameMap;	///< 이미지 파일의 인덱스 번호,파일이름 맵
	std::map < tstring, int > m_imageFilename2IndexMap;		///< 이미지 파일이름,인덱스 번호 맵

private:
	/// 복사 생성자와 = 연산자 금지
	CachedData & operator=(const CachedData &);
	CachedData(const CachedData &);
};

