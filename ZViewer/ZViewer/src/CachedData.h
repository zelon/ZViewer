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
#include "../../commonSrc/LockUtil.h"

typedef std::map < tstring, ZImage * >::iterator CacheMapIterator;
typedef std::map < tstring, ZImage * >::const_iterator CacheMapIterator_const;


class CachedData
{
public:
	CachedData()
	{
		m_lCacheSize = 0;
		m_numImageVectorSize = 0;
	
	}
	virtual ~CachedData()
	{
		ClearCachedImageData();
	}

	void ClearCachedImageData();
	size_t Size() const { return m_cacheData.size(); }

	void PrintCachedData() const;

	bool IsEmpty() const
	{
		CLockObjUtil lock(m_cacheLock);

		if ( m_cacheData.empty() ) return true;
		return false;
	}

	void InsertData(const tstring & strFilename, ZImage * pImage, bool bForceCache);

	void ShowCacheInfo() const;

	bool HasCachedData(const int index) const;

	bool HasCachedData(const tstring & strFilename) const
	{
		CLockObjUtil lock(m_cacheLock);
		return (m_cacheData.count(strFilename) > 0);
	}

	size_t GetIndex2FilenameMapSize() const
	{
		CLockObjUtil lock(m_cacheLock);
		return m_imageIndex2FilenameMap.size();
	}

	void SetNewFileList(const std::vector < FileData > & v);

	size_t GetImageVectorSize()
	{
		CLockObjUtil lock(m_cacheLock);
		return m_numImageVectorSize;
	}

	bool GetCachedData(const tstring & strFilename, ZImage * & pImage) const;
	bool ClearFarthestDataFromCurrent(const int iFarthestIndex);

	/// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
	int GetFarthestIndexFromCurrentIndex(volatile const int & iCurrentIndex);

	tstring GetFilenameFromIndex(const int iIndex)
	{
		if (m_imageIndex2FilenameMap.find(iIndex) == m_imageIndex2FilenameMap.end())
		{
			assert(false);
			return _T("");
		}
		return m_imageIndex2FilenameMap[iIndex];
	}

	void WaitCacheLock()
	{
		CLockObjUtil lock(m_cacheLock);
	}

	void SetImageVector(const std::vector < FileData > & v)
	{
		CLockObjUtil lock(m_cacheLock);

		m_numImageVectorSize = v.size();

		SetNewFileList(v);

		ClearCachedImageData();
		m_lCacheSize = 0;

		DebugPrintf(TEXT("imageVecSize : %d"), m_numImageVectorSize);
	}

	const long GetCachedTotalSize() const
	{
		CLockObjUtil lock(m_cacheLock);
		return m_lCacheSize;
	}

protected:
	std::map < tstring, ZImage * > m_cacheData;		///< 실제로 캐쉬된 데이터를 가지고 있는 맵
	std::map < int , tstring > m_imageIndex2FilenameMap;	///< 이미지 파일의 인덱스 번호,파일이름 맵
	std::map < tstring, int > m_imageFilename2IndexMap;		///< 이미지 파일이름,인덱스 번호 맵

	size_t m_numImageVectorSize;

	/// 캐시된 데이터 용량
	long m_lCacheSize;

	mutable CLockObj m_cacheLock;

private:
	/// 복사 생성자와 = 연산자 금지
	CachedData & operator=(const CachedData &);
	CachedData(const CachedData &);
};

