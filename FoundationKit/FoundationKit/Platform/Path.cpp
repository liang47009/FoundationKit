/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.

  losemymind.libo@gmail.com

****************************************************************************/
#include <algorithm>
#include <chrono> // for GetTempFileName create tmp file name.
#include <cctype>
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Platform/Path.hpp"


NS_FK_BEGIN
const char Path::AltDirectorySeparatorChar = '/';

#if PLATFORM_WINDOWS
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
std::string Path::TemporaryPath = "";

std::string Path::NormalizePath(const std::string& path)
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


std::string Path::PlatformPath(const std::string& path)
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

std::string Path::GetPath(const std::string& InPath)
{
    std::string result = GetFileName(InPath);
    result = InPath.substr(0, InPath.size() - result.size());
    return result;
}


std::string Path::GetFileName(const std::string& path)
{
    if (!path.empty())
    {
        int length = static_cast<int>(path.size());
        for (int i = length; --i >= 0;) 
        {
            char ch = path[i];
            if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar)
                return path.substr(i + 1, length - i - 1);
        }
    }
    return path;
}

std::string Path::GetBaseFilename(const std::string& InPath)
{
    std::string result = GetFileName(InPath);
    if (!result.empty())
    {
        size_t i;
        if ((i = result.find_last_of('.')) != std::string::npos)
            result = result.substr(0, i);
    }
    return result;
}

bool Path::HasExtension(const std::string& path)
{
    if (!path.empty()) {
        for (int i = static_cast<int>(path.size()); --i >= 0;)
        {
            char ch = path[i];
            if (ch == '.')
            {
                if (i != static_cast<int>(path.size()) - 1)
                    return true;
                else
                    return false;
            }
            if (ch == DirectorySeparatorChar || ch == AltDirectorySeparatorChar || ch == VolumeSeparatorChar) break;
        }
    }
    return false;
}

std::string Path::GetExtension(const std::string& path)
{
    std::string fileExtension;
    size_t pos = path.find_last_of('.');
    if (pos != std::string::npos)
    {
        fileExtension = path.substr(pos, path.size());
        std::transform(fileExtension.begin(), fileExtension.end(), fileExtension.begin(), [](unsigned char c) { return static_cast<unsigned char>(std::tolower(c)); });
    }
    return fileExtension;
}

std::string Path::ChangeExtension(const std::string& path, const std::string& extension)
{
    std::string resultPath = path;
    if (!resultPath.empty())
    {
        size_t DotPos = resultPath.find_last_of('.');
        if (DotPos != std::string::npos)
        {
            resultPath = resultPath.substr(0, DotPos);
        }

        if (!extension.empty())
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

const char* Path::GetInvalidFileNameChars()
{
    return InvalidFileNameChars;
}

const char* Path::GetInvalidPathChars()
{
    return InvalidPathChars;
}

char Path::GetDefaultPathSeparator()
{
    return DirectorySeparatorChar;
}

std::string Path::GetTempFile()
{
    std::string result = GetTempPath();
    auto timepoint = std::chrono::high_resolution_clock::now();
    result += StringUtils::Tostring(timepoint.time_since_epoch().count());
    result += ".tmp";
    return result;
}


bool Path::IsPathRooted(std::string path)
{
    if (!path.empty())
    {
        size_t length = path.size();
        if ((length >= 1 && (path[0] == DirectorySeparatorChar || path[0] == AltDirectorySeparatorChar)) || (length >= 2 && path[1] == VolumeSeparatorChar))
            return true;
    }
    return false;
}


std::string Path::GetRelativePath(const std::string& basePath, const std::string& fullPath)
{
    std::string finalBasePath = NormalizePath(basePath);
    std::string finalFullPath = NormalizePath(fullPath);
    std::vector<std::string> basePathDirectories = StringUtils::Split(finalBasePath, "/");
    std::vector<std::string> fullPathDirectories = StringUtils::Split(finalFullPath, "/");
    size_t dirCount = basePathDirectories.size() < fullPathDirectories.size() ? basePathDirectories.size() : fullPathDirectories.size();
    size_t lastCommonDirIndex = 0;
    for (size_t index = 0; index < dirCount; ++index)
    {
        if (basePathDirectories[index] == fullPathDirectories[index])
            lastCommonDirIndex = index;
        else
            break;
    }

    if (lastCommonDirIndex == 0)
        return fullPath;

    std::string RelativePath;
    for (size_t index = lastCommonDirIndex + 1; index < basePathDirectories.size(); ++index)
        RelativePath += "../";

    for (size_t index = lastCommonDirIndex + 1; index < fullPathDirectories.size()-1; ++index)
        RelativePath += fullPathDirectories[index] + "/";
    RelativePath += fullPathDirectories[fullPathDirectories.size() - 1];
    return RelativePath;
}


std::string Path::GetAbsolutePath(const std::string& path)
{
    std::string normalizePath = NormalizePath(path);
    std::string fileName = GetFileName(normalizePath);
    std::string finalPath = GetPath(normalizePath);
    std::vector<std::string> pathNodes = StringUtils::Split(finalPath, "/");
    std::vector<std::string> absolutePaths;
    for (auto& pathNode : pathNodes)
    {
        if (pathNode == ".")
        {
            continue;
        }
        if (pathNode == "..")
        {
            if (absolutePaths.size() > 1)
            {
                absolutePaths.erase(absolutePaths.end() - 1);
            }
        }
        else
        {
            absolutePaths.push_back(pathNode);
        }
    }
    finalPath = "";
    for (auto& absoluteNode : absolutePaths)
    {
        finalPath += absoluteNode;
        finalPath += "/";
    }
    finalPath += fileName;
    return finalPath;
}

NS_FK_END



