@echo off
SET START_DIR=%CD%
python setVersion.py
IF %ERRORLEVEL% == 0 GOTO VS_BUILD
GOTO FAILED_VERSION

:VS_BUILD
call "C:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
devenv ZViewerWin32.sln /rebuild Release
IF %ERRORLEVEL% == 0 GOTO MAKE_PORTABLE
GOTO VS_FAILED

:MAKE_PORTABLE
cd output
python MakePortable.py
IF %ERRORLEVEL% == 0 GOTO MAKE_NSIS_SETUP
GOTO FAILED_MAKE_PORTABLE

:MAKE_NSIS_SETUP
"C:\Program Files\NSIS\makensis.exe" ZViewer.nsi
IF %ERRORLEVEL% == 0 GOTO SUCCESS
GOTO NSIS_FAILED

:SUCCESS
echo -------------------------------------------------
echo [OK] Build for release is completed without error
echo -------------------------------------------------
GOTO END

:FAILED_VERSION
echo [FAILED] Set Version
GOTO END

:VS_FAILED
echo [FAILED] Visual Studio Build FAILED!!!!!
GOTO END

:FAILED_MAKE_PORTABLE
echo [FAILED] Make portable
GOTO END

:NSIS_FAILED
echo [FAILED] NSIS failed
GOTO END

:END
cd %START_DIR%
