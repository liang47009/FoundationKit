@echo off
REM
REM

%~d0
cd %~dp0

set DEVENV=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\IDE\devenv

if exist "..\Win32" rmdir "..\Win32" /s /q

::Build project NetFramework
call "%DEVENV%" ..\FoundationKitFramework.sln /rebuild "Debug|win32" /project NetFramework /projectconfig "Debug|win32"
call "%DEVENV%" ..\FoundationKitFramework.sln /rebuild "Release|win32" /project NetFramework /projectconfig "Release|win32"

set PUBLISH_DIR="..\publish\vs2017\NetFramework\"

if exist %PUBLISH_DIR% rmdir %PUBLISH_DIR% /s /q

xcopy /Y /Q /S "..\NetFramework\NetFramework\*.h" "%PUBLISH_DIR%include\NetFramework\"
xcopy /Y /Q /S "..\NetFramework\NetFramework\*.hpp" "%PUBLISH_DIR%include\NetFramework\"
xcopy /Y /Q /S "..\NetFramework\NetFramework\*.inl" "%PUBLISH_DIR%include\NetFramework\"
xcopy /Y /Q /S "..\NetFramework\NetFramework\*.ipp" "%PUBLISH_DIR%include\NetFramework\"
xcopy /Y /Q /S "..\Win32\*.lib" "%PUBLISH_DIR%prebuilt\"

pause