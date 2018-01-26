@echo off
REM
REM

%~d0
cd %~dp0

set DEVENV=%VS80COMNTOOLS%..\IDE\devenv

::Build project FoundationKit
call "%DEVENV%" ..\FoundationKitCPP03\proj.win32\FoundationKitCPP03.sln /rebuild "Debug|win32" /project FoundationKit /projectconfig "Debug|win32"
call "%DEVENV%" ..\FoundationKitCPP03\proj.win32\FoundationKitCPP03.sln /rebuild "Release|win32" /project FoundationKit /projectconfig "Release|win32"

set PUBLISH_DIR="..\publish\vs2005\FoundationKit\"

if exist %PUBLISH_DIR% rmdir %PUBLISH_DIR% /s /q

xcopy /Y /Q /S "..\FoundationKitCPP03\FoundationKit\*.h" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKitCPP03\FoundationKit\*.hpp" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKitCPP03\FoundationKit\*.inl" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKitCPP03\proj.win32\*.lib" "%PUBLISH_DIR%prebuilt\vs2005\"
pause