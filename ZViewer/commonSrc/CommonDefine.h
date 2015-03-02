#pragma once

/// 일반적으로 대충쓰는 버퍼의 크기
const int COMMON_BUFFER_SIZE = 512;

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#define		EXIF_ROTATION_0		"top, left side"
#define		EXIF_ROTATION_90	"right side, top"
#define		EXIF_ROTATION_180	"bottom, right side"
#define		EXIF_ROTATION_270	"left side, bottom"


const tstring g_strVersion = TEXT("0.9.0");
const tstring g_strHomepage = TEXT("http://zviewer.wimy.com");

struct TagData {
  std::string m_strKey;
  std::string m_strValue;
};

struct FileData {
  tstring m_strFileName;	// 파일명
  _FILETIME m_timeModified;	// 최근 수정된 날짜
  DWORD m_nFileSize;			// 파일크기
};

struct ExtSetting {
  unsigned int m_numIconIndex;		// icon dll 에서의 index
  tstring m_strExt;				// 파일의 확장자
};


class CFileDataSort_OnlyFilenameCompare {
public:
  bool operator()(const FileData & a, const FileData & b) {
    /*
    // 파일명 비교를 할 때 모두 소문자로 바꿔서 비교한다.
    char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
    _snprintf(szTempA, sizeof(szTempA), GetOnlyFileNameWithoutExt(a.m_strFileName).c_str());
    _snprintf(szTempB, sizeof(szTempB), GetOnlyFileNameWithoutExt(b.m_strFileName).c_str());
    strlwr(szTempA);
    strlwr(szTempB);

    return (strcmp(szTempB, szTempA) > 0);
    */
    return (_tcsicmp(b.m_strFileName.c_str(), a.m_strFileName.c_str()) > 0);
  }
};

class CFileDataSort_OnlyFilenameCompare_XP {
public:
  bool operator()(const FileData & a, const FileData & b) {
    /*
    // 파일명 비교를 할 때 모두 소문자로 바꿔서 비교한다.
    char szTempA[FILENAME_MAX], szTempB[FILENAME_MAX];
    _snprintf(szTempA, sizeof(szTempA), GetOnlyFileNameWithoutExt(a.m_strFileName).c_str());
    _snprintf(szTempB, sizeof(szTempB), GetOnlyFileNameWithoutExt(b.m_strFileName).c_str());
    strlwr(szTempA);
    strlwr(szTempB);

    return (StrCmp(szTempB, szTempA) > 0);
    */

    return (_tcsicmp(b.m_strFileName.c_str(), a.m_strFileName.c_str()) > 0);
  }
};

class CFileDataSort_FileSize {
public:
  bool operator()(const FileData & a, const FileData & b) {
    return a.m_nFileSize > b.m_nFileSize;
  }
};

class CFileDataSort_LastModifiedTime {
public:
  bool operator()(const FileData & a, const FileData & b) {
    // 돌려주는 부등호가 > 이면 최근것이 앞에, < 이면 최근것이 뒤에 간다.
    if (a.m_timeModified.dwHighDateTime == b.m_timeModified.dwHighDateTime) {
      return a.m_timeModified.dwLowDateTime >= b.m_timeModified.dwLowDateTime;
    }
    return a.m_timeModified.dwHighDateTime >= b.m_timeModified.dwHighDateTime;
  }
};

class NonCopyable {
protected:
  NonCopyable() { /* do nothgin */ }

private:
  NonCopyable(const NonCopyable &);
  const NonCopyable operator=(const NonCopyable &);
};

enum eFileSortOrder {
  eFileSortOrder_FILENAME,
  eFileSortOrder_LAST_MODIFY_TIME,
  eFileSortOrder_FILESIZE,
};

