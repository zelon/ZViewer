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
#include "ZOption.h"

void CachedData::ClearCachedImageData()
{
	CLockObjUtil lock(m_cacheLock);
	m_cacheData.clear();
	m_lCacheSize = 0;
}

void CachedData::ShowCacheInfo() const
{
	CLockObjUtil lock(m_cacheLock);
	for ( auto it = m_cacheData.begin(); it != m_cacheData.end(); ++it )
	{
		DebugPrintf(TEXT("[%s] %dbyte"), it->first.c_str(), it->second->GetImageSize());
	}
}

bool CachedData::HasCachedData(const int index) const
{
	CLockObjUtil lock(m_cacheLock);

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
	CLockObjUtil lock(m_cacheLock);

	for ( const auto& pair : m_cacheData ) {
		DebugPrintf(TEXT("Cacaed Filename : %s"), pair.first.c_str());
	}
}

/// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
int CachedData::GetFarthestIndexFromCurrentIndex(volatile const int & iCurrentIndex)
{
	CLockObjUtil lock(m_cacheLock);
	assert(m_imageFilename2IndexMap.size() == m_imageIndex2FilenameMap.size());

	int iFarthestIndex = iCurrentIndex;
	int iDistanceMax = 0;
	int iDistance = 0;
	int iTempIndex = 0;

	tstring strFarthest;

	for (auto it = m_cacheData.begin(); it != m_cacheData.end(); ++it)
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

void CachedData::SetNewFileList(const std::vector < FileData > & v)
{
	CLockObjUtil lock(m_cacheLock);

	m_imageIndex2FilenameMap.clear();
	m_imageFilename2IndexMap.clear();
	for ( size_t i = 0; i < v.size(); ++i)
	{
		m_imageIndex2FilenameMap.insert(std::make_pair((int)i, v[i].m_strFileName));
		m_imageFilename2IndexMap.insert(std::make_pair(v[i].m_strFileName, (int)i));
	}

	DebugPrintf(TEXT("m_imageFilename2IndexMap : %d"), m_imageFilename2IndexMap.size());
	DebugPrintf(TEXT("imageIndex2FilenameMapSize : %d"), m_imageIndex2FilenameMap.size());

	assert(m_imageFilename2IndexMap.size() == m_imageIndex2FilenameMap.size());
}

std::shared_ptr<ZImage> CachedData::GetCachedData(const tstring& strFilename) const
{
	CLockObjUtil lock(m_cacheLock);

	auto it = m_cacheData.find(strFilename);
	if ( it == m_cacheData.end() )
	{
		assert(!"Can't get NOT CACHED");
		return nullptr;
	}
	else
	{
		DebugPrintf(TEXT("Cache hit"));
	}

	return it->second;
}

bool CachedData::ClearFarthestDataFromCurrent(const int iFarthestIndex)
{
	CLockObjUtil lock(m_cacheLock);

	std::map < int , tstring >::iterator itFileName = m_imageIndex2FilenameMap.find(iFarthestIndex);
	if ( itFileName == m_imageIndex2FilenameMap.end() )
	{
		assert(!"Can't find the cache data.");
		return false;
	}

	auto it = m_cacheData.find(itFileName->second);

	if ( it != m_cacheData.end() )
	{
		if ( NULL == it->second )
		{
			assert(it->second);
			return false;
		}
		m_lCacheSize -= it->second->GetImageSize();
		
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


void CachedData::InsertData(const tstring & strFilename, std::shared_ptr<ZImage> pImage, bool bForceCache)
{
	/// 이미 캐시되어 있으면 캐시하지 않는다.
	if ( HasCachedData(strFilename) ) {
		assert(false);
		return;
	}

	if ( false == bForceCache )
	{
		/// 용량을 체크해서 이 이미지를 캐시했을 때 제한을 넘어섰으면 캐시하지 않는다.
		if ( (GetCachedTotalSize() + pImage->GetImageSize()) /1024/1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() )
		{
			DebugPrintf(_T("-- 이 이미지를 캐시하면 용량제한을 넘어서 캐시하지 않습니다 -- : %s"), strFilename.c_str());
			return;
		}
	}

#ifndef _DEBUG
	try
	{
#endif
		std::chrono::system_clock::time_point startTime = std::chrono::system_clock::now();

		CLockObjUtil lock(m_cacheLock);
		{
			if ( m_cacheData.find(strFilename) != m_cacheData.end() ) {
				assert(!"--- 이미 캐시에 있는 파일을 다시 넣으려고 합니다 ---");
				return;
			}
			m_cacheData[strFilename] = pImage;
		}
		m_lCacheSize += pImage->GetImageSize();

		DebugPrintf(TEXT("%s added to cache"), strFilename.c_str());
		std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
		long long diffTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

		DebugPrintf(TEXT("Cache insert time : %d filename(%s)"), diffTime, strFilename.c_str());

#ifndef _DEBUG
	}
	catch ( ... )
	{
		return;
	}
#endif

}
