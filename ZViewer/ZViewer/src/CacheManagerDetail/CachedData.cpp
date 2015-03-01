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

#include "ZImage.h"
#include "ZOption.h"

void CachedData::ClearCachedImageData() {
  CLockObjUtil lock(m_cacheLock);
  m_cacheData.clear();
  m_lCacheSize = 0;
}

size_t CachedData::GetCachedCount() const {
  CLockObjUtil lock(m_cacheLock);
  return m_cacheData.size();
}

void CachedData::ShowCacheInfo() const {
  CLockObjUtil lock(m_cacheLock);
  for ( auto it = m_cacheData.begin(); it != m_cacheData.end(); ++it ) {
    DebugPrintf(TEXT("[%s] %dbyte"), it->first.c_str(), it->second->GetImageSize());
  }
}

bool CachedData::HasCachedDataByIndex(const int index) const {
  CLockObjUtil lock(m_cacheLock);

  const tstring filename = file_map_.FindFilenameByIndex(index);
  if (filename.empty() == false) {
    return HasCachedDataByFilename(filename);
  }
  return false;
}

bool CachedData::HasCachedDataByFilename(const tstring& strFilename) const {
  CLockObjUtil lock(m_cacheLock);
  return (m_cacheData.count(strFilename) > 0);
}

size_t CachedData::GetIndex2FilenameMapSize() const {
  CLockObjUtil lock(m_cacheLock);
  return file_map_.size();
}

size_t CachedData::GetImageVectorSize() {
  CLockObjUtil lock(m_cacheLock);
  return m_numImageVectorSize;
}

void CachedData::PrintCachedData() const {
  CLockObjUtil lock(m_cacheLock);

  for ( const auto& pair : m_cacheData ) {
    DebugPrintf(TEXT("Cacaed Filename : %s"), pair.first.c_str());
  }
}

bool CachedData::IsEmpty() const {
  CLockObjUtil lock(m_cacheLock);
  return m_cacheData.empty();
}

/// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
int CachedData::GetFarthestIndexFromCurrentIndex(volatile const int & iCurrentIndex) {
  CLockObjUtil lock(m_cacheLock);

  int iFarthestIndex = iCurrentIndex;
  int iDistanceMax = 0;
  int iDistance = 0;
  int iTempIndex = 0;

  tstring strFarthest;

  for (auto it = m_cacheData.begin(); it != m_cacheData.end(); ++it) {
    iTempIndex = file_map_.FindIndexByFilename(it->first);
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

tstring CachedData::GetFilenameFromIndex(const int index) {
  CLockObjUtil lock(m_cacheLock);
  return file_map_.FindFilenameByIndex(index);
}

void CachedData::WaitCacheLock() {
  CLockObjUtil lock(m_cacheLock);
}

void CachedData::SetImageVector(const std::vector < FileData > & filedata_list) {
  CLockObjUtil lock(m_cacheLock);

  m_numImageVectorSize = filedata_list.size();

  file_map_.Set(filedata_list);

  ClearCachedImageData();
  m_lCacheSize = 0;

  DebugPrintf(TEXT("imageVecSize : %d"), m_numImageVectorSize);
}

const long CachedData::GetCachedTotalSize() const
{
  CLockObjUtil lock(m_cacheLock);
  return m_lCacheSize;
}

std::shared_ptr<ZImage> CachedData::GetCachedData(const tstring& strFilename) const {
  CLockObjUtil lock(m_cacheLock);

  auto it = m_cacheData.find(strFilename);
  if ( it == m_cacheData.end() ) {
    assert(!"Can't get NOT CACHED");
    return nullptr;
  }
  else
  {
    DebugPrintf(TEXT("Cache hit"));
  }

  return it->second;
}

bool CachedData::ClearFarthestDataFromCurrent(const int iFarthestIndex) {
  CLockObjUtil lock(m_cacheLock);

  const tstring filename = file_map_.FindFilenameByIndex(iFarthestIndex);
  if (filename.empty()) {
    assert(!"Can't find the cache data.");
    return false;
  }

  auto it = m_cacheData.find(filename);
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

void CachedData::InsertData(const tstring& strFilename, std::shared_ptr<ZImage> pImage, const bool bForceCache) {
  /// 이미 캐시되어 있으면 캐시하지 않는다.
  if ( HasCachedDataByFilename(strFilename) ) {
    assert(false);
    return;
  }

  if ( false == bForceCache ) {
    /// 용량을 체크해서 이 이미지를 캐시했을 때 제한을 넘어섰으면 캐시하지 않는다.
    if ( (GetCachedTotalSize() + pImage->GetImageSize()) /1024/1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() )
    {
      DebugPrintf(_T("-- 이 이미지를 캐시하면 용량제한을 넘어서 캐시하지 않습니다 -- : %s"), strFilename.c_str());
      return;
    }
  }

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
}

