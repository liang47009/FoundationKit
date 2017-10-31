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

::Build project NetFramework
call "%DEVENV%" ..\FoundationKitFramework.sln /rebuild "Debug|win32" /project NetFramework /projectconfig "Debug|win32"
call "%DEVENV%" ..\FoundationKitFramework.sln /rebuild "Release|win32" /project NetFramework /projectconfig "Release|win32"

set PUBLISH_DIR="..\publish\vs2015\"

if exist %PUBLISH_DIR% rmdir %PUBLISH_DIR% /s /q

xcopy /Y /Q /S "..\FoundationKit\FoundationKit\*.h" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKit\FoundationKit\*.hpp" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKit\FoundationKit\*.inl" "%PUBLISH_DIR%include\FoundationKit\"

xcopy /Y /Q /S "..\NetFramework\NetFramework\*.h" "%PUBLISH_DIR%include\NetFramework\"
xcopy /Y /Q /S "..\NetFramework\NetFramework\*.hpp" "%PUBLISH_DIR%include\NetFramework\"
xcopy /Y /Q /S "..\NetFramework\NetFramework\*.inl" "%PUBLISH_DIR%include\NetFramework\"
xcopy /Y /Q /S "..\NetFramework\NetFramework\*.ipp" "%PUBLISH_DIR%include\NetFramework\"

xcopy /Y /Q /S "..\Win32\Debug\*.lib" "%PUBLISH_DIR%libs\Debug\"
xcopy /Y /Q /S "..\Win32\Release\*.lib" "%PUBLISH_DIR%libs\Release\"

xcopy /Y /Q /S "..\Win32\Debug\*.dll" "%PUBLISH_DIR%libs\Debug\"
xcopy /Y /Q /S "..\Win32\Release\*.dll" "%PUBLISH_DIR%libs\Release\"

pause