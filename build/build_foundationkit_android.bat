@echo off
rem ***************************************************************************
rem *
rem * Copyright (C) 2012 - 2016, libo, <losemymind.libo@gmail.com>.
rem *
rem * This software is licensed as described in the file COPYING, which
rem * you should have received as part of this distribution. 
rem *
rem * You may opt to use, copy, modify, merge, publish, distribute and/or sell
rem * copies of the Software, and permit persons to whom the Software is
rem * furnished to do so, under the terms of the COPYING file.
rem *
rem * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
rem * KIND, either express or implied.
rem *
rem ***************************************************************************

%~d0
cd %~dp0
cd ../FoundationKit/proj.android
call ndk-build -j8

set PUBLISH_DIR="..\..\publish\Android\"

if exist %PUBLISH_DIR% rmdir %PUBLISH_DIR% /s /q

xcopy /Y /Q /S "..\FoundationKit\*.h" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKit\*.hpp" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\FoundationKit\*.inl" "%PUBLISH_DIR%include\FoundationKit\"
xcopy /Y /Q /S "..\NetFramework\*.ipp" "%PUBLISH_DIR%include\NetFramework\"
xcopy /Y /Q /S ".\obj\local\*.a" "%PUBLISH_DIR%libs\"


pause
