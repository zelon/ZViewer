/* ------------------------------------------------------------------------
 *
 * CachdeData.h
 *
 * 2008.12.13 KIM JINWOOK
 *
 * ------------------------------------------------------------------------
 */

#pragma once

#include "../../commonSrc/ZImage.h"
#include "../../commonSrc/LockUtil.h"
#include "FileMap.h"

class CachedData final : NonCopyable {
public:
  CachedData() : m_lCacheSize(0), m_numImageVectorSize(0) {
    /* do nothing */
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

  void InsertData(const tstring& strFilename, std::shared_ptr<ZImage> image, const bool bForceCache);

  void ShowCacheInfo() const;

  bool HasCachedDataByIndex(const int index) const;

  bool HasCachedDataByFilename(const tstring & strFilename) const;

  size_t GetIndex2FilenameMapSize() const {
    CLockObjUtil lock(m_cacheLock);
    return file_map_.size();
  }

  size_t GetImageVectorSize()
  {
    CLockObjUtil lock(m_cacheLock);
    return m_numImageVectorSize;
  }

  std::shared_ptr<ZImage> GetCachedData(const tstring& strFilename) const;
  bool ClearFarthestDataFromCurrent(const int iFarthestIndex);

  /// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
  int GetFarthestIndexFromCurrentIndex(volatile const int & iCurrentIndex);

  tstring GetFilenameFromIndex(const int index) {
    CLockObjUtil lock(m_cacheLock);
    return file_map_.FindFilenameByIndex(index);
  }

  void WaitCacheLock() {
    CLockObjUtil lock(m_cacheLock);
  }

  void SetImageVector(const std::vector < FileData > & filedata_list) {
    CLockObjUtil lock(m_cacheLock);

    m_numImageVectorSize = filedata_list.size();

    file_map_.Set(filedata_list);

    ClearCachedImageData();
    m_lCacheSize = 0;

    DebugPrintf(TEXT("imageVecSize : %d"), m_numImageVectorSize);
  }

  const long GetCachedTotalSize() const {
    CLockObjUtil lock(m_cacheLock);
    return m_lCacheSize;
  }

private:
  std::map< tstring/*filename*/, std::shared_ptr<ZImage> > m_cacheData;
  FileMap file_map_;

  size_t m_numImageVectorSize;

  /// 캐시된 데이터 용량
  long m_lCacheSize;

  mutable CLockObj m_cacheLock;
};

