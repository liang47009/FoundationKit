cd /d %~dp0

IF EXIST "Win32" (
    rmdir "Win32" /s /q
)

IF EXIST "x64" (
    rmdir "x64" /s /q
)

IF EXIST "Release" (
    rmdir "Release" /s /q
)

IF EXIST "Debug" (
    rmdir "Debug" /s /q
)

IF EXIST "publish" (
    rmdir "publish" /s /q
)

IF EXIST "Rubbish" (
    rmdir "Rubbish" /s /q
)

IF EXIST "FoundationKit\proj.android\libs" (
    rmdir "FoundationKit\proj.android\libs" /s /q
)

IF EXIST "FoundationKit\proj.android\obj" (
    rmdir "FoundationKit\proj.android\obj" /s /q
)

IF EXIST "FoundationKit\proj.android\bin" (
    rmdir "FoundationKit\proj.android\bin" /s /q
)

IF EXIST "FoundationKit\proj.android\gen" (
    rmdir "FoundationKit\proj.android\gen" /s /q
)

IF EXIST "FoundationKit\proj.android_studio\.gradle" (
    rmdir "FoundationKit\proj.android_studio\.gradle" /s /q
)

IF EXIST "FoundationKit\proj.android_studio\.idea" (
    rmdir "FoundationKit\proj.android_studio\.idea" /s /q
)

IF EXIST "FoundationKit\proj.android_studio\build" (
    rmdir "FoundationKit\proj.android_studio\build" /s /q
)

IF EXIST "FoundationKit\proj.android_studio\app\build" (
    rmdir "FoundationKit\proj.android_studio\app\build" /s /q
)

IF EXIST "FoundationKit\proj.android_studio\app\.externalNativeBuild" (
    rmdir "FoundationKit\proj.android_studio\app\.externalNativeBuild" /s /q
)

IF EXIST "FoundationKitUnitTest\proj.android\obj" (
    rmdir "FoundationKitUnitTest\proj.android\obj" /s /q
)

IF EXIST "FoundationKitUnitTest\proj.android\bin" (
    rmdir "FoundationKitUnitTest\proj.android\bin" /s /q
)

IF EXIST "FoundationKitUnitTest\proj.android\gen" (
    rmdir "FoundationKitUnitTest\proj.android\gen" /s /q
)

IF EXIST "FoundationKitUnitTest\proj.android\libs\armeabi" (
    rmdir "FoundationKitUnitTest\proj.android\libs\armeabi" /s /q
)

IF EXIST "FoundationKitUnitTest\proj.android\libs\armeabi-v7a" (
    rmdir "FoundationKitUnitTest\proj.android\libs\armeabi-v7a" /s /q
)

IF EXIST "FoundationKitUnitTest\proj.android\libs\x86" (
    rmdir "FoundationKitUnitTest\proj.android\libs\x86" /s /q
)

IF EXIST "FoundationKitCPP03\proj.win32\Debug" (
    rmdir "FoundationKitCPP03\proj.win32\Debug" /s /q
)

IF EXIST "FoundationKitCPP03\proj.win32\Release" (
    rmdir "FoundationKitCPP03\proj.win32\Release" /s /q
)

pause