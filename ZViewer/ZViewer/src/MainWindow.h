#pragma once

#include "../commonSrc/windowLib/ZWindow.h"

class CMainWindow final : public ZWindow {
public:
  CMainWindow();
  ~CMainWindow();

  virtual void SetWndProc();

  HWND Create(HINSTANCE hInstance, HWND hParentHWND, int nCmdShow);
  static HMENU CreatePopupMenu();

  static void HandCursorProc();
};