IF EXIST "Debug" (
    rmdir "Debug" /s /q
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

pause