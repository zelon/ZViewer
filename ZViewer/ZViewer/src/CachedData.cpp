/* ------------------------------------------------------------------------
 *
 * CachedData.cpp
 *
 * 2008.12.13 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#include "stdafx.h"

#include "CachedData.h"

void CachedData::Clear()
{
	CacheMapIterator it;

	for ( ; ; )
	{
		if ( m_cacheData.empty() ) break;
		it = m_cacheData.begin();
		if ( it->second )
		{
			delete it->second;
		}
		m_cacheData.erase(it);
	}

	m_cacheData.clear();
}

void CachedData::ShowCacheInfo() const
{
	CacheMapIterator_const it;

	for ( it = m_cacheData.begin(); it != m_cacheData.end(); ++it )
	{
		const ZImage * pImage = it->second;

		DebugPrintf(TEXT("[%s] %dbyte"), it->first.c_str(), pImage->GetImageSize());
	}
}

bool CachedData::HasCachedData(const int index) const
{
	/// 인덱스를 파일명으로 바꾼다.
	std::map < int, tstring >::const_iterator it = m_imageIndex2FilenameMap.find(index);
	if ( it != m_imageIndex2FilenameMap.end() )
	{
		const tstring & strNextFilename = it->second;
		if ( true == HasCachedData(strNextFilename) )
		{
			DebugPrintf(TEXT("다음 파일 캐쉬됨"));
			return true;
		}
	}
	else
	{
		assert(false);
	}
	return false;
}

void CachedData::PrintCachedData() const
{
	CacheMapIterator_const it, endIt = m_cacheData.end();

	for ( it = m_cacheData.begin(); it != endIt; ++it)
	{
		DebugPrintf(TEXT("Cacaed Filename : %s"), it->first.c_str());
	}
}

/// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
int CachedData::GetFarthestIndexFromCurrentIndex(volatile const int & iCurrentIndex)
{
	CacheMapIterator it, endIt = m_cacheData.end();

	int iFarthestIndex = iCurrentIndex;
	int iDistanceMax = 0;
	int iDistance = 0;
	int iTempIndex = 0;

	tstring strFarthest;

	for ( it = m_cacheData.begin(); it != endIt; ++it)
	{
		iTempIndex = m_imageFilename2IndexMap[it->first];
		iDistance = abs(iTempIndex - iCurrentIndex);

		if ( iDistance > iDistanceMax )
		{
			iDistanceMax = iDistance;
			iFarthestIndex = iTempIndex;
			strFarthest = it->first;
		}
	}

	assert(iFarthestIndex >= 0 );

	return iFarthestIndex;
}

void CachedData::SetNewFileList(const std::vector < FileData > & v, const size_t & numImageVectorSize)
{
	m_imageIndex2FilenameMap.clear();
	m_imageFilename2IndexMap.clear();
	for ( size_t i = 0; i < numImageVectorSize; ++i)
	{
		m_imageIndex2FilenameMap.insert(std::make_pair((int)i, v[i].m_strFileName));
		m_imageFilename2IndexMap.insert(std::make_pair(v[i].m_strFileName, (int)i));
	}

	DebugPrintf(TEXT("imageIndex2FilenameMapSize : %d"), m_imageIndex2FilenameMap.size());
}

bool CachedData::GetCachedData(const tstring & strFilename, ZImage * & pImage) const
{
	CacheMapIterator_const it;

	it = m_cacheData.find(strFilename);
	if ( it == m_cacheData.end() )
	{
		assert(!"Can't get NOT CACHED");
		return false;
	}
	else
	{
		DebugPrintf(TEXT("Cache hit"));
	}

	TIMECHECK_START("GetCacheData");
	pImage = it->second;
	TIMECHECK_END();

	return true;
}

bool CachedData::ClearFarthestDataFromCurrent(const int iFarthestIndex, long & cacheSize)
{
	tstring strFindFileName = m_imageIndex2FilenameMap[iFarthestIndex];
	CacheMapIterator it = m_cacheData.find(m_imageIndex2FilenameMap[iFarthestIndex]);

	if ( it != m_cacheData.end() )
	{
		if ( NULL == it->second )
		{
			assert(it->second);
			return false;
		}
		cacheSize -= it->second->GetImageSize();
		
		delete (it->second);
		m_cacheData.erase(it);

		DebugPrintf(TEXT("Farthest one clear"));
	}
	else
	{
		assert(!"Can't find the cache data.");
		return false;
	}
	return true;
}
