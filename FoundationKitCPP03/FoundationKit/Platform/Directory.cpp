
#include <dirent.h> // for DIR
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Base/locale.hpp"
#include "FoundationKit/Platform/Directory.hpp"
#include "FoundationKit/Platform/Path.hpp"
NS_FK_BEGIN

std::string Directory::CurrentDirectory;

void InternalGetFiles(const std::string& path, std::vector<std::string>& files, Directory::SearchOption searchOption /*= SearchOption::TopDirectoryOnly*/)
{
    std::string finallyPath = path;
    if (*(finallyPath.end() - 1) != Path::DirectorySeparatorChar && *(finallyPath.end() - 1) != Path::AltDirectorySeparatorChar)
    {
        finallyPath += Path::AltDirectorySeparatorChar;
    }
    DIR* dir = opendir(finallyPath.c_str());
    if (!dir) return;
    dirent* entry = readdir(dir);
    while (entry)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            entry = readdir(dir);
            continue;
        }

        if (entry->d_type == DT_REG)
        {
            files.push_back(finallyPath + entry->d_name);
        }
        if (entry->d_type == DT_DIR && searchOption == Directory::SearchOption::AllDirectories)
        {
            std::string strChildDir = finallyPath + entry->d_name;
            InternalGetFiles(strChildDir, files, searchOption);
        }

        entry = readdir(dir);
    }
    closedir(dir);
}

void InternalGetFiles(const std::string& path, const Directory::EnumFileCallback& callback, Directory::SearchOption searchOption /*= SearchOption::TopDirectoryOnly*/)
{
    std::string finallyPath = path;
    if (*(finallyPath.end() - 1) != Path::DirectorySeparatorChar && *(finallyPath.end() - 1) != Path::AltDirectorySeparatorChar)
    {
        finallyPath += Path::AltDirectorySeparatorChar;
    }
    DIR* dir = opendir(finallyPath.c_str());
    if (!dir) return;
    dirent* entry = readdir(dir);
    while (entry)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            entry = readdir(dir);
            continue;
        }

        if (entry->d_type == DT_REG)
        {
            callback(finallyPath + entry->d_name);
        }
        if (entry->d_type == DT_DIR && searchOption == Directory::SearchOption::AllDirectories)
        {
            std::string strChildDir = finallyPath + entry->d_name;
            InternalGetFiles(strChildDir, callback, searchOption);
        }

        entry = readdir(dir);
    }
    closedir(dir);
}

void InternalGetDirectories(const std::string& path, std::vector<std::string>& dirs, Directory::SearchOption searchOption /*= SearchOption::TopDirectoryOnly*/)
{
    std::string finallyPath = path;
    if (*(finallyPath.end() - 1) != Path::DirectorySeparatorChar && *(finallyPath.end() - 1) != Path::AltDirectorySeparatorChar)
    {
        finallyPath += Path::AltDirectorySeparatorChar;
    }
    DIR* dir = opendir(finallyPath.c_str());
    if (!dir) return;
    dirent* entry = readdir(dir);
    while (entry)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            entry = readdir(dir);
            continue;
        }
        if (entry->d_type == DT_DIR)
        {
            dirs.push_back(finallyPath + entry->d_name);
            if (searchOption == Directory::SearchOption::AllDirectories)
            {
                InternalGetDirectories(finallyPath + entry->d_name, dirs, searchOption);
            }
        }
        entry = readdir(dir);
    }
    closedir(dir);
}


void Directory::GetFiles(const std::string& path, std::vector<std::string>& files, SearchOption searchOption /*= SearchOption::TopDirectoryOnly*/)
{
    scope_locale sl("");//for dirent.h wcstombs_s and mbstowcs_s method.
    InternalGetFiles(path, files, searchOption);
}

void FoundationKit::Directory::GetFiles(const std::string& path, const EnumFileCallback& callback, SearchOption searchOption /*= SearchOption::TopDirectoryOnly*/)
{
    scope_locale sl("");//for dirent.h wcstombs_s and mbstowcs_s method.
    InternalGetFiles(path, callback, searchOption);
}

void Directory::GetDirectories(const std::string& path, std::vector<std::string>& dirs, SearchOption searchOption /*= SearchOption::TopDirectoryOnly*/)
{
    scope_locale sl("");//for dirent.h wcstombs_s and mbstowcs_s method.
    InternalGetDirectories(path, dirs, searchOption);
}


NS_FK_END



