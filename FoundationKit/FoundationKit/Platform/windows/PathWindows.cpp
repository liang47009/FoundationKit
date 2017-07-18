
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
    WCHAR app_data_path[MAX_PATH + 1];
    ::GetTempPathW(MAX_PATH + 1, app_data_path);
    std::wstring result(app_data_path);
    WCHAR full_path[MAX_PATH + 1] = { 0 };
    ::GetModuleFileNameW(nullptr, full_path, MAX_PATH + 1);
    // Get filename of executable only, e.g. MyGame.exe
    WCHAR *base_name = wcsrchr(full_path, '\\');
    result += base_name;
    // Remove ".exe" extension, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame
    result = result.substr(0, result.rfind(L"."));
    result += DirectorySeparatorChar;
    // Create directory
    if (SUCCEEDED(SHCreateDirectoryEx(nullptr, result.c_str(), nullptr)))
    {
        TemporaryPath = StringUtils::wstring2UTF8string(app_data_path);
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
    WCHAR full_path[MAX_PATH + 1] = { 0 };
    ::GetModuleFileNameW(nullptr, full_path, MAX_PATH + 1);
    // Debug app uses executable directory; Non-debug app uses local app data directory
    //#ifndef _DEBUG
    // Get filename of executable only, e.g. MyGame.exe
    WCHAR *base_name = wcsrchr(full_path, '\\');
    std::wstring retPath;
    if (base_name)
    {
        WCHAR app_data_path[MAX_PATH + 1];
        // Get local app data directory, e.g. C:\Documents and Settings\username\Local Settings\Application Data
        if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_COMMON_DOCUMENTS, nullptr, SHGFP_TYPE_CURRENT, app_data_path)))
        {
            std::wstring ret(app_data_path);
            // Adding executable filename, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame.exe
            ret += base_name;
            // Remove ".exe" extension, e.g. C:\Documents and Settings\username\Local Settings\Application Data\MyGame
            ret = ret.substr(0, ret.rfind(L"."));
            ret += L"\\";
            // Create directory
            if (SUCCEEDED(SHCreateDirectoryEx(nullptr, ret.c_str(), nullptr)))
            {
                retPath = ret;
            }
        }
    }
    if (retPath.empty())
    {
        // If fetching of local app data directory fails, use the executable one
        retPath = full_path;
        // remove xxx.exe
        retPath = retPath.substr(0, retPath.rfind(L"\\") + 1);
    }
    DocumentsPath = StringUtils::wstring2UTF8string(retPath);
    return DocumentsPath;
}

NS_FK_END


