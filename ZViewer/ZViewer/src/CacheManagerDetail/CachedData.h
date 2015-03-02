#pragma once

#include "../../commonSrc/LockUtil.h"
#include "FileMap.h"

class ZImage;

class CachedData final : NonCopyable {
public:
  CachedData() : cached_bytes_total_(0) {
    /* do nothing */
  }

  void ClearCachedImageData();
  size_t GetCachedCount() const;

  void PrintCachedData() const;

  bool IsEmpty() const;

  bool InsertData(const tstring& strFilename, std::shared_ptr<ZImage> image, const bool bForceCache);

  void ShowCacheInfo() const;

  bool HasCachedDataByIndex(const int index) const;
  bool HasCachedDataByFilename(const tstring & strFilename) const;

  size_t GetFilelistSize() const;

  std::shared_ptr<ZImage> GetCachedData(const tstring& strFilename) const;
  bool ClearFarthestDataFromCurrent(const int iFarthestIndex);

  /// 캐시되어 있는 데이터들 중 현재 인덱스로부터 가장 멀리있는 인덱스를 얻는다.
  int GetFarthestIndexFromCurrentIndex(volatile const int & iCurrentIndex);

  tstring GetFilenameFromIndex(const int index);

  void SetFilelist(const std::vector < FileData > & filedata_list);

  const long GetCachedTotalBytes() const;

private:
  std::map< tstring/*filename*/, std::shared_ptr<ZImage> > m_cacheData;
  FileMap file_map_;

  long cached_bytes_total_;

  mutable CLockObj m_cacheLock;
};

