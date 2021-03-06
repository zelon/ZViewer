﻿#include "stdafx.h"
#include "ZOption.h"

ZOption & ZOption::GetInstance() {
  static ZOption inst;
  return inst;
}

ZOption::ZOption() {
  /// 아래 값은 ZViewerAgent 에서만 true 이다
  m_bDontSaveInstance = false;
}

tstring ZOption::GetOptionFileFullPath() {
  TCHAR buffer[1024] = { 0, };

  tstring file_path;

  /// C:\Documents and Settings\USERID\Local Settings\Application Data 의 위치를 얻어온다.
  if ( S_OK != SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA, NULL, SHGFP_TYPE_CURRENT, buffer) ) {
    file_path = GetProgramFolder();
  } else {
    file_path = buffer;
  }

  file_path += TEXT("\\zviewer.ini");

  return file_path;
}

void ZOption::LoadOption() {
  m_strOptionFilename = GetOptionFileFullPath();

  /// 기본적인 옵션은 설정해둔다.
  SetDefaultOption();

  /// Make the table that indicate which key-value is mapped to which variable
  InitializeOptionMappingTable();

  /// 기본적인 옵션에서 파일에서 불러온 설정을 덮어씌운다.
  LoadFromFile();
}

/// Make the table that indicate which key-value is mapped to which variable
void ZOption::InitializeOptionMappingTable()
{
  _InsertSaveOptionSetting(L"maximumcachememoryMB", &m_iMaximumCacheMemoryMB);
  _InsertSaveOptionSetting(L"maximumcachefilenum", &m_iMaxCacheImageNum);

  _InsertSaveOptionSetting(L"loop_view", &m_bLoopImages);

  _InsertSaveOptionSetting(L"stretch_small_to_big", &m_bSmallToBigStretchImage);
  _InsertSaveOptionSetting(L"stretch_big_to_small", &m_bBigToSmallStretchImage);

  _InsertSaveOptionSetting(L"use_open_cmd_shell", &m_bUseOpenCMDInShell);
  _InsertSaveOptionSetting(L"use_preview_shell", &m_bUsePreviewInShell);

  _InsertSaveOptionSetting(L"use_debug", &m_bUseDebug);

  _InsertSaveOptionSetting(L"use_auto_rotation", &m_bUseAutoRotation);

  _InsertSaveOptionSetting(L"last_copy_directory", &m_strLastCopyDirectory);
  _InsertSaveOptionSetting(L"last_move_directory", &m_strLastMoveDirectory);
}

/// 기본적인 옵션을 설정해둔다. 설치 후 처음 실행되었을 때 이 값을 기준으로 zviewer.ini 파일이 만들어진다.
void ZOption::SetDefaultOption() {
  m_bUsePreviewInShell = true;
  m_bUseOpenCMDInShell = false;
  is_always_on_top_window_ = false;
  is_slide_mode = false;
  m_iSlideModePeriodMiliSeconds = 5000;	///< Default slide mode period is 5 seconds
  m_bLoopImages = false;
  m_bUseAutoRotation = true;
  is_fullscreen_mode_ = false;
  m_bUseDebug = true;
  m_bBigToSmallStretchImage = false;
  m_bSmallToBigStretchImage = false;
  m_iMaximumCacheMemoryMB = 500;

  m_iMaxCacheImageNum = 10;

#ifdef _DEBUG
  m_bUseOpenCMDInShell = true;
#endif
}

void ZOption::LoadFromFile() {
  iniMap data;

  /// 파일로부터 설정 불러오기가 성공했을 때만 설정을 한다.
  if ( COptionFile::LoadFromFile(m_strOptionFilename, data) ) {
    for ( size_t i=0; i<m_saveOptions.size(); ++i ) {
      if ( data.count(m_saveOptions[i].getString()) > 0) {
        m_saveOptions[i].InsertMapToValue(data);
      }
    }
  }
}

void ZOption::SaveToFile() {
  /// To prevent from saving at ZViewerAgent
  if (m_bDontSaveInstance) {
    return;
  }

  iniMap data;

  for ( size_t i=0; i<m_saveOptions.size(); ++i ) {
    m_saveOptions[i].InsertValueToMap(data);
  }
  COptionFile::SaveToFile(m_strOptionFilename, data);
}
