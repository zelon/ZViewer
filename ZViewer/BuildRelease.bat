@echo off
SET NSIS_PATH="C:\Program Files\NSIS\makensis.exe"
SET VS_BATCH="C:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat"
SET START_DIR=%CD%

:CHECK_NSIS
if exist %NSIS_PATH% GOTO CHECK_VS_BATCH
echo Cannot find %NSIS_PATH%
GOTO END

:CHECK_VS_BATCH
if exist %VS_BATCH% GOTO SET_VERSION
echo Cannot find %VS_BATCH%
GOTO END

:SET_VERSION
python setVersion.py
IF %ERRORLEVEL% == 0 GOTO VS_BUILD
echo [FAILED] Set Version
GOTO END

:VS_BUILD
call %VS_BATCH% x86
devenv ZViewerWin32.sln /rebuild Release
IF %ERRORLEVEL% == 0 GOTO MAKE_PORTABLE
echo [FAILED] Visual Studio Build FAILED!!!!!
GOTO END

:MAKE_PORTABLE
cd output
python MakePortable.py
IF %ERRORLEVEL% == 0 GOTO MAKE_NSIS_SETUP
echo [FAILED] Make portable
GOTO END

:MAKE_NSIS_SETUP
%NSIS_PATH% ZViewer.nsi
IF %ERRORLEVEL% == 0 GOTO MAKE_DEBUG_INFO
echo [FAILED] NSIS failed
GOTO END

:MAKE_DEBUG_INFO
python MakeDebugInfo.py
IF %ERRORLEVEL% == 0 GOTO PREPARE_RELEASE
echo [FAILED] Making Debug info is failed
GOTO END

:PREPARE_RELEASE
python PrepareRelease.py
IF %ERRORLEVEL% == 0 GOTO SUCCESS
echo [FAILED] Preparing release is failed
GOTO END

:SUCCESS
echo -------------------------------------------------
echo [OK] Build for release is completed without error
echo -------------------------------------------------
GOTO END

:END
cd %START_DIR%
pause
