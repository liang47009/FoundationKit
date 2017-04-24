@echo off
REM
REM

%~d0
cd %~dp0

set DEVENV=%VS80COMNTOOLS%..\IDE\devenv

::Build project FoundationKit
call "%DEVENV%" ..\FoundationKitCPP03\proj.win32\FoundationKitCPP03.sln /rebuild "Debug|win32" /project FoundationKit /projectconfig "Debug|win32"
call "%DEVENV%" ..\FoundationKitCPP03\proj.win32\FoundationKitCPP03.sln /rebuild "Release|win32" /project FoundationKit /projectconfig "Release|win32"

::Build Solution FoundationKitCPP03.sln
::call "%DEVENV%" ..\FoundationKitCPP03\proj.win32\FoundationKitCPP03.sln /rebuild "Debug|win32"
::call "%DEVENV%" ..\FoundationKitCPP03\proj.win32\FoundationKitCPP03.sln /rebuild "Release|win32"

if exist "..\publish\vs2005\" rmdir "..\publish\vs2005\" /s /q

xcopy /Y /Q /S "..\FoundationKitCPP03\FoundationKit\*.h" "..\publish\vs2005\include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKitCPP03\FoundationKit\*.hpp" "..\publish\vs2005\include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKitCPP03\FoundationKit\*.inl" "..\publish\vs2005\include\FoundationKit\"

xcopy /Y /Q /S "..\FoundationKitCPP03\proj.win32\Debug\*.lib" "..\publish\vs2005\libs\Debug\"
xcopy /Y /Q /S "..\FoundationKitCPP03\proj.win32\Release\*.lib" "..\publish\vs2005\libs\Release\"

pause