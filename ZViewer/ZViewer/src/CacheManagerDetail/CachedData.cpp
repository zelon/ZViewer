#include "stdafx.h"
#include "CachedData.h"

#include "ZImage.h"
#include "ZOption.h"

void CachedData::ClearCachedImageData() {
  CLockObjUtil lock(m_cacheLock);
  m_cacheData.clear();
  cached_bytes_total_ = 0;
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

size_t CachedData::GetFilelistSize() const {
  CLockObjUtil lock(m_cacheLock);
  return file_map_.size();
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
    if (iTempIndex == -1) {// not in the folder. TODO: To be uncached
      continue;
    }

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

int CachedData::GetIndexFromFilename(const tstring& filename) {
  CLockObjUtil lock(m_cacheLock);
  return file_map_.FindIndexByFilename(filename);
}

void CachedData::SetFilelist(const std::vector < FileData > & filelist) {
  CLockObjUtil lock(m_cacheLock);

  file_map_.Set(filelist);

  ClearCachedImageData();
  cached_bytes_total_ = 0;

  DebugPrintf(TEXT("filelist size: %d"), filelist.size());
}

const long CachedData::GetCachedTotalBytes() const
{
  CLockObjUtil lock(m_cacheLock);
  return cached_bytes_total_;
}

std::shared_ptr<ZImage> CachedData::GetCachedData(const tstring& strFilename) const {
  CLockObjUtil lock(m_cacheLock);

  auto it = m_cacheData.find(strFilename);
  if ( it == m_cacheData.end() ) {
    return nullptr;
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
  if ( it != m_cacheData.end() ) {
    if ( NULL == it->second ) {
      assert(it->second);
      return false;
    }
    cached_bytes_total_ -= it->second->GetImageSize();
    
    m_cacheData.erase(it);

    DebugPrintf(TEXT("Farthest one clear"));
  } else {
    assert(!"Can't find the cache data.");
    return false;
  }
  return true;
}

// return true if cached
bool CachedData::InsertData(const tstring& strFilename, std::shared_ptr<ZImage> pImage, const bool bForceCache) {
  /// 이미 캐시되어 있으면 캐시하지 않는다.
  if ( HasCachedDataByFilename(strFilename) ) {
    assert(false);
    return true;
  }

  if ( false == bForceCache ) {
    /// 용량을 체크해서 이 이미지를 캐시했을 때 제한을 넘어섰으면 캐시하지 않는다.
    if ( (GetCachedTotalBytes() + pImage->GetImageSize()) /1024/1024 > ZOption::GetInstance().GetMaxCacheMemoryMB() )
    {
      DebugPrintf(_T("-- 이 이미지를 캐시하면 용량제한을 넘어서 캐시하지 않습니다 -- : %s"), strFilename.c_str());
      return false;
    }
  }

  CLockObjUtil lock(m_cacheLock);
  {
    if ( m_cacheData.find(strFilename) != m_cacheData.end() ) {
      assert(!"--- 이미 캐시에 있는 파일을 다시 넣으려고 합니다 ---");
      return true;
    }
    m_cacheData[strFilename] = pImage;
  }
  cached_bytes_total_ += pImage->GetImageSize();

  return true;
}

