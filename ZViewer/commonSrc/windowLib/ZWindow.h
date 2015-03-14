#pragma once

class ZWindow {
public:
  ZWindow();
  virtual ~ZWindow();

  virtual void SetWndProc() = 0;

  virtual HWND Create(HINSTANCE hInstance, HWND hParentHWND, int nCmdShow, const tstring & dlgName, HMENU hMenu);

  WPARAM DoModal() {
    return MsgProc();
  }

  virtual void SetHWND(HWND hWnd) {
    m_hWindow = hWnd;
  }

  void SetAccel(const HACCEL accel_handle) { m_hAccel = accel_handle; }

protected:
  WPARAM MsgProc();

  HWND m_hWindow;

  WNDPROC m_wndProc;

private:
  HACCEL m_hAccel;
};