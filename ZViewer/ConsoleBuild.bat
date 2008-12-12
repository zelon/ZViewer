@echo off
call "c:\Program Files\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86
IF "%1" == "rebuild" GOTO REBUILD
IF "%1" == "clean" GOTO CLEAN
devenv ZViewerWin32.sln /build ZViewerDebug
GOTO END

:REBUILD
devenv ZViewerWin32.sln /rebuild ZViewerDebug
GOTO END

:CLEAN
devenv ZViewerWin32.sln /clean ZViewerDebug
GOTO END

:END

