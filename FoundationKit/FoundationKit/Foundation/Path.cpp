#include <algorithm>
#include <chrono> // for GetTempFileName create tmp file name.
#include "FoundationKit/Foundation/Path.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#if TARGET_PLATFORM == PLATFORM_WINDOWS
#include <Windows.h>
#include <Shlobj.h>
#endif

NS_FK_BEGIN
const char Path::AltDirectorySeparatorChar = '/';

#if TARGET_PLATFORM == PLATFORM_WINDOWS
const char Path::DirectorySeparatorChar = '\\';
#else
const char Path::DirectorySeparatorChar = '/';
#endif

const char Path::InvalidPathChars[] = { '\"', '<', '>', '|', '\0'
, (char)1, (char)2, (char)3, (char)4, (char)5, (char)6
, (char)7, (char)8, (char)9, (char)10, (char)11, (char)12
, (char)13, (char)14, (char)15, (char)16, (char)17, (char)18
, (char)19, (char)20, (char)21, (char)22, (char)23, (char)24
, (char)25, (char)26, (char)27, (char)28, (char)29, (char)30, (char)31 };

const char Path::InvalidFileNameChars[] = { '\"', '<', '>', '|', '\0'
, (char)1, (char)2, (char)3, (char)4, (char)5, (char)6
, (char)7, (char)8, (char)9, (char)10, (char)11, (char)12
, (char)13, (char)14, (char)15, (char)16, (char)17, (char)18
, (char)19, (char)20, (char)21, (char)22, (char)23, (char)24
, (char)25, (char)26, (char)27, (char)28, (char)29, (char)30
, (char)31, ':', '*', '?', '\\', '/' };

const char Path::PathSeparator = ';';

const char Path::VolumeSeparatorChar = ':';

std::string Path::DocumentsPath = "";

std::string Path::ConvertPathFormatToUnixStyle(const std::string& path)
{
    std::string ret = path;
    size_t len = ret.length();
    for (size_t i = 0; i < len; ++i)
    {
        if (ret[i] == '\\')
        {
            ret[i] = '/';
        }
    }
    return ret;
}

std::string Path::ConvertPathFormatToPlatformStyle(const std::string& path)
{
    std::string ret = path;
    size_t len = ret.length();
    for (size_t i = 0; i < len; ++i)
    {
        if (ret[i] == '\\' || ret[i] == '/')
        {
            ret[i] = DirectorySeparatorChar;
        }
    }
    return ret;
}


std::string Path::ChangeExtension(const std::string& path, const std::string& extension)
{
    std::string resultPath = path;
    if (!resultPath.empty())
    {
        for (int i = path.size(); --i >= 0;)
        {
            char ch = path[i];
            if (ch == '.') 
            {
                resultPath = path.substr(0, i);
                break;
            }
            if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar) break;
        }
        if (!extension.empty() && path.size() != 0) 
        {
            if (extension[0] != '.')
            {
                resultPath += ".";
            }
            resultPath += extension;
        }
    }
    return resultPath;
}

std::string Path::Combine(const std::vector<std::string>& paths)
{
    size_t pathCount = paths.size();
    size_t allPathSize = 0;
    for (size_t i = 0; i < pathCount; ++i)
    {
        allPathSize += paths[i].size();
    }
    std::string result;
    result.reserve(allPathSize + 1);
    for (size_t i = 0; i < pathCount; ++i)
    {
        result += paths[i];
    }
    return result;
}

std::string Path::Combine(const std::string& path1, const std::string& path2)
{
    std::string result;
    if (path1.empty())
        return path2;

    if (path2.empty())
        return path1;

    if (IsPathRooted(path2))
        return path2;

    char p1end = path1[path1.size() - 1];
    if (p1end != DirectorySeparatorChar && p1end != AltDirectorySeparatorChar && p1end != VolumeSeparatorChar)
        return path1 + DirectorySeparatorChar + path2;
    return path1 + path2;
}

std::string Path::Combine(const std::string& path1, const std::string& path2, const std::string& path3)
{
    std::string result = Combine(path1, path2);
    result = Combine(result, path3);
    return result;
}

std::string Path::Combine(const std::string& path1, const std::string& path2, const std::string& path3, const std::string& path4)
{
    std::string result = Combine(path1, path2);
    result = Combine(result, path3);
    result = Combine(result, path4);
    return result;
}


std::string Path::GetDirectoryName(const std::string& path)
{
    std::string result = GetFileName(path);
    result = path.substr(0, path.size() - result.size()-1);
    return result;
}

std::string Path::GetExtension(const std::string& path)
{
    std::string fileExtension;
    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos)
    {
        fileExtension = path.substr(pos, path.size());
        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), ::tolower);
    }
    return fileExtension;
}

std::string Path::GetFileName(const std::string& path)
{
    if (!path.empty())
    {
        int length = path.size();
        for (int i = length; --i >= 0;) 
        {
            char ch = path[i];
            if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar)
                return path.substr(i + 1, length - i - 1);
        }
    }
    return path;
}

std::string Path::GetFileNameWithoutExtension(const std::string& path)
{
    std::string result = GetFileName(path);
    if (!result.empty())
    {
        size_t i;
        if ((i = result.find_last_of('.')) != std::string::npos)
            result = result.substr(0, i);
    }
    return result;
}

const char* Path::GetInvalidFileNameChars()
{
    return InvalidFileNameChars;
}

const char* Path::GetInvalidPathChars()
{
    return InvalidPathChars;
}

std::string Path::GetTempFileName()
{
    std::string result = GetTempPath();
    auto timepoint = std::chrono::high_resolution_clock::now();
    result += std::to_string(timepoint.time_since_epoch().count());
    result += ".tmp";
    return result;
}

std::string Path::GetTempPath()
{
    std::string result;
#if TARGET_PLATFORM == PLATFORM_WINDOWS
    WCHAR app_data_path[MAX_PATH + 1];
    ::GetTempPathW(MAX_PATH + 1, app_data_path);
    result = StringUtils::wstring2UTF8string(app_data_path);
#else
#error This function is not impl
#endif
    return result;
}


bool Path::HasExtension(const std::string& path)
{
    if (!path.empty()) {
        for (size_t i = path.size(); --i >= 0;) 
        {
            char ch = path[i];
            if (ch == '.') 
            {
                if (i != path.size() - 1)
                    return true;
                else
                    return false;
            }
            if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar) break;
        }
    }
    return false;
}

bool Path::IsPathRooted(std::string path)
{
    if (!path.empty())
    {
        int length = path.size();
        if ((length >= 1 && (path[0] == DirectorySeparatorChar || path[0] == AltDirectorySeparatorChar)) || (length >= 2 && path[1] == VolumeSeparatorChar))
            return true;
    }
    return false;
}

bool Path::IsAbsolutePath(const std::string& path)
{
#if (TARGET_PLATFORM==PLATFORM_WINDOWS)
    if ((path.length() > 2 &&
        ((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' && path[0] <= 'Z')) &&
        path[1] == ':') || (path[0] == '/' && path[1] == '/'))
    {
        return true;
    }
    return false;
#else
    return (path[0] == '/');
#endif
}

std::string Path::GetDocumentsPath()
{
    if (DocumentsPath.size() > 0)
    {
        return DocumentsPath;
    }

#if TARGET_PLATFORM == PLATFORM_WINDOWS
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
        if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_LOCAL_APPDATA, nullptr, SHGFP_TYPE_CURRENT, app_data_path)))
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
    
#elif (TARGET_PLATFORM == PLATFORM_IOS) ||(TARGET_PLATFORM == PLATFORM_MAC)
    // save to document folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex : 0];
    // or
    //NSString *documentsDirectory = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents"];
    DocumentsPath = [documentsDirectory UTF8String];
    DocumentsPath.append("/");
#elif TARGET_PLATFORM == PLATFORM_ANDROID
    struct stat st;
    stat(DocumentsPath.c_str(), &st);
    if (!S_ISDIR(st.st_mode)) {
        mkdir(DocumentsPath.c_str(), 0744);
    }
#endif
    return DocumentsPath;
}
void Path::SetDocumentsPath(const std::string& path)
{
    DocumentsPath = path;
}


NS_FK_END



