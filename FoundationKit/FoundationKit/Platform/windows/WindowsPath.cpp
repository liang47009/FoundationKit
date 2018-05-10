/****************************************************************************
Copyright (c) 2017 libo All rights reserved.

losemymind.libo@gmail.com

****************************************************************************/
#include "FoundationKit/GenericPlatformMacros.hpp"
#if (PLATFORM_WINDOWS)
#include <Windows.h>
#include <Shlobj.h>
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
NS_FK_BEGIN

std::string Path::GetTempPath()
{
    if (TemporaryPath.size() > 0)
    {
        return TemporaryPath;
    }
    WCHAR TempPath[MAX_PATH + 1];
    ::GetTempPathW(MAX_PATH + 1, TempPath);

    // Always expand the temp path in case windows returns short directory names.
    TCHAR FullTempPath[MAX_PATH];
    ZeroMemory(FullTempPath, sizeof(TCHAR) * MAX_PATH);
    ::GetLongPathName(TempPath, FullTempPath, MAX_PATH);

    std::wstring result(FullTempPath);
    WCHAR full_path[MAX_PATH + 1] = { 0 };
    ::GetModuleFileNameW(nullptr, full_path, MAX_PATH + 1);
    // Get filename of executable only, e.g. MyGame.exe
    WCHAR *base_name = wcsrchr(full_path, '\\');
    result += (++base_name); // Remove '\\'
    // Remove ".exe" extension, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame
    result = result.substr(0, result.rfind(L"."));
    result += DirectorySeparatorChar;
    // Create directory
    if (SUCCEEDED(SHCreateDirectoryEx(nullptr, result.c_str(), nullptr)))
    {
        TemporaryPath = StringUtils::wstring2string(result);
    }
    return TemporaryPath;
}

std::string Path::GetDocumentsPath()
{
    if (DocumentsPath.size() > 0)
    {
        return DocumentsPath;
    }
    // Get full path of executable, e.g. c:\Program Files (x86)\My Game Folder\MyGame.exe
    WCHAR ModuleFullPath[MAX_PATH + 1] = { 0 };
    ::GetModuleFileNameW(nullptr, ModuleFullPath, MAX_PATH + 1);
    // Debug app uses executable directory; Non-debug app uses local app data directory
    //#ifndef _DEBUG
    // Get filename of executable only, e.g. MyGame.exe
    WCHAR *AppBaseName = wcsrchr(ModuleFullPath, '\\');
    std::wstring FullDocumentsPath;
    if (AppBaseName)
    {
        WCHAR UserDocumentPath[MAX_PATH + 1];
        // Get user document directory, e.g. C:\User\username\Documents
        if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_PERSONAL /*CSIDL_COMMON_DOCUMENTS*/, nullptr, SHGFP_TYPE_CURRENT, UserDocumentPath)))
        {
            FullDocumentsPath = UserDocumentPath;
            // Adding executable filename, e.g. C:\User\username\Documents\MyGame.exe
            FullDocumentsPath += AppBaseName;
            // Remove ".exe" extension, e.g. C:\User\username\Documents\MyGame
            FullDocumentsPath = FullDocumentsPath.substr(0, FullDocumentsPath.rfind(L"."));
            FullDocumentsPath += L"\\";
            // Create directory
            if (!SUCCEEDED(SHCreateDirectoryEx(nullptr, FullDocumentsPath.c_str(), nullptr)))
            {
                FullDocumentsPath.clear();
            }
        }
    }
    if (FullDocumentsPath.empty())
    {
        // If fetching of local app data directory fails, use the executable one
        FullDocumentsPath = ModuleFullPath;
        // remove xxx.exe
        FullDocumentsPath = FullDocumentsPath.substr(0, FullDocumentsPath.rfind(L"\\") + 1);
    }
    DocumentsPath = StringUtils::wstring2string(FullDocumentsPath);
    return DocumentsPath;
}

std::string Path::GetApplicationPath()
{
    char utf8ExeDir[MAX_PATH+1] = { 0 };
    if (_wpgmptr != NULL)
    {
        WCHAR *pUtf16ExePath = nullptr;
        _get_wpgmptr(&pUtf16ExePath);
        // We need only directory part without exe
        //WCHAR *pUtf16DirEnd = wcsrchr(pUtf16ExePath, L'\\');
        //WideCharToMultiByte(CP_UTF8, 0, pUtf16ExePath, pUtf16DirEnd - pUtf16ExePath + 1, utf8ExeDir, sizeof(utf8ExeDir), nullptr, nullptr);
        WideCharToMultiByte(CP_UTF8, 0, pUtf16ExePath, static_cast<int>(wcslen(pUtf16ExePath)), utf8ExeDir, sizeof(utf8ExeDir), nullptr, nullptr);
    }
    else
    {
        char *pUtf16ExePath = nullptr;
        _get_pgmptr(&pUtf16ExePath);
        //char *pUtf16DirEnd = strrchr(pUtf16ExePath, '\\');
        //memcpy(utf8ExeDir, pUtf16ExePath, pUtf16DirEnd - pUtf16ExePath + 1);
        memcpy(utf8ExeDir, pUtf16ExePath, strlen(pUtf16ExePath));
    }
    return utf8ExeDir;
}


NS_FK_END

#endif // OF #if (PLATFORM_WINDOWS)



