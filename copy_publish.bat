
set PROJECT_DIR=E:\WorkSpace\SnailMobileLibrary\ThirdParty

xcopy /Y /Q /S ".\publish\Android\include\*.h" "%PROJECT_DIR%\FoundationKit\include\android\"
xcopy /Y /Q /S ".\publish\Android\include\*.hpp" "%PROJECT_DIR%\FoundationKit\include\android\"
xcopy /Y /Q /S ".\publish\Android\include\*.inl" "%PROJECT_DIR%\FoundationKit\include\android\"
echo F|xcopy /Y /Q ".\publish\Android\libs\armeabi\libfoundationkit.a" "%PROJECT_DIR%\FoundationKit\prebuilt\Android\armeabi\libfoundationkit.a"
echo F|xcopy /Y /Q ".\publish\Android\libs\armeabi-v7a\libfoundationkit.a" "%PROJECT_DIR%\FoundationKit\prebuilt\Android\armeabi-v7a\libfoundationkit.a"
echo F|xcopy /Y /Q ".\publish\Android\libs\x86\libfoundationkit.a" "%PROJECT_DIR%\FoundationKit\prebuilt\Android\x86\libfoundationkit.a"

xcopy /Y /Q /S ".\publish\iOS\include\*.h" "%PROJECT_DIR%\FoundationKit\include\ios\"
xcopy /Y /Q /S ".\publish\iOS\include\*.hpp" "%PROJECT_DIR%\FoundationKit\include\ios\"
xcopy /Y /Q /S ".\publish\iOS\include\*.inl" "%PROJECT_DIR%\FoundationKit\include\ios\"
echo F|xcopy /Y /Q ".\publish\iOS\libs\Debug-iphoneos\libFoundationKit_iOS.a" "%PROJECT_DIR%\FoundationKit\prebuilt\ios\Debug-iphoneos\libfoundationkit.a"
echo F|xcopy /Y /Q ".\publish\iOS\libs\Release-iphoneos\libFoundationKit_iOS.a" "%PROJECT_DIR%\FoundationKit\prebuilt\ios\Release-iphoneos\libfoundationkit.a"

xcopy /Y /Q /S ".\publish\Mac\include\*.h" "%PROJECT_DIR%\FoundationKit\include\Mac\"
xcopy /Y /Q /S ".\publish\Mac\include\*.hpp" "%PROJECT_DIR%\FoundationKit\include\Mac\"
xcopy /Y /Q /S ".\publish\Mac\include\*.inl" "%PROJECT_DIR%\FoundationKit\include\Mac\"
echo F|xcopy /Y /Q ".\publish\Mac\libs\Debug\libFoundationKit_Mac.a" "%PROJECT_DIR%\FoundationKit\prebuilt\Mac\Debug\libfoundationkit.a"
echo F|xcopy /Y /Q ".\publish\Mac\libs\Release\libFoundationKit_Mac.a" "%PROJECT_DIR%\FoundationKit\prebuilt\Mac\Release\libfoundationkit.a"

xcopy /Y /Q /S ".\publish\vs2015\include\*.h" "%PROJECT_DIR%\FoundationKit\include\win32\"
xcopy /Y /Q /S ".\publish\vs2015\include\*.hpp" "%PROJECT_DIR%\FoundationKit\include\win32\"
xcopy /Y /Q /S ".\publish\vs2015\include\*.inl" "%PROJECT_DIR%\FoundationKit\include\win32\"
echo F|xcopy /Y /Q ".\publish\vs2015\libs\Debug\FoundationKit.lib" "%PROJECT_DIR%\FoundationKit\prebuilt\win32\Debug\libfoundationkit.lib"
echo F|xcopy /Y /Q ".\publish\vs2015\libs\Release\FoundationKit.lib" "%PROJECT_DIR%\FoundationKit\prebuilt\win32\Release\libfoundationkit.lib"

pause

