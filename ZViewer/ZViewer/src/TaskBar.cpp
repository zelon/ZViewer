#include "stdafx.h"
#include "TaskBar.h"

void TaskBar::Show() {
  const HWND handle = FindWindow(TEXT("Shell_TrayWnd"), TEXT(""));

  if ( handle != INVALID_HANDLE_VALUE ) {
    ShowWindow(handle, SW_SHOW);
  }
}

void TaskBar::Hide() {
  const HWND handle = FindWindow(TEXT("Shell_TrayWnd"), TEXT(""));

  if ( handle != INVALID_HANDLE_VALUE ) {
    ShowWindow(handle, SW_HIDE);
  }
}

