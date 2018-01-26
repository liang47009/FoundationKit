@echo off
REM
REM

%~d0
cd %~dp0

set DEVENV=%VS140COMNTOOLS%..\IDE\devenv

if exist "..\Win32" rmdir "..\Win32" /s /q

::Build project FoundationKit
call "%DEVENV%" ..\FoundationKitFramework.sln /rebuild "Debug|win32" /project FoundationKit /projectconfig "Debug|win32"
call "%DEVENV%" ..\FoundationKitFramework.sln /rebuild "Release|win32" /project FoundationKit /projectconfig "Release|win32"

set PUBLISH_DIR="..\publish\vs2015\FoundationKit\"

if exist %PUBLISH_DIR% rmdir %PUBLISH_DIR% /s /q

xcopy /Y /Q /S "..\FoundationKit\FoundationKit\*.h" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKit\FoundationKit\*.hpp" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKit\FoundationKit\*.inl" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKit\FoundationKit\*.ipp" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\Win32\*.lib" "%PUBLISH_DIR%prebuilt\"

pause