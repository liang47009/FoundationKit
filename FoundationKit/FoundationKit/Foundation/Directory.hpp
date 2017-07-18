/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_DIRECTORY_HPP
#define FOUNDATIONKIT_DIRECTORY_HPP
#include <string>
#include <vector>
#include <functional>
#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN
class Directory
{
public:
    typedef std::function<void(const std::string&)> EnumFileCallback;
    enum class SearchOption
    {
        TopDirectoryOnly = 0,
        AllDirectories = 1
    };

   /**
    *  Creates a directory.
    *
    *  @param dirPath The path of the directory, it must be an absolute path.
    *  @return True if the directory have been created successfully, false if not.
    */
    static bool CreateDirectory(const std::string& path);

   /**
    *  Removes a directory.
    *
    *  @param dirPath  The full path of the directory, it must be an absolute path.
    *  @return True if the directory have been removed successfully, false if not.
    */
    static bool RemoveDirectory(const std::string& path);

    static void Move(const std::string& sourceDirName, const std::string& destDirName);

   /**
    *  Checks whether the path is a directory.
    *
    *  @param dirPath The path of the directory, it could be a relative or an absolute path.
    *  @return True if the directory exists, false if not.
    */
    static bool IsExist(const std::string& path);

   /**
    *  Checks whether the 'path' is a directory.
    *
    *  @param path The path of the directory, it could be a relative or an absolute path.
    *  @return True if the 'path' is a directory, false if not.
    */
    static bool IsDirectory(const std::string& path);

   /**
    *  get all files from a directory
    *
    *  @param dirPath The path of the directory.
    */
    static void GetFiles(const std::string& path, std::vector<std::string>& files, SearchOption searchOption = SearchOption::TopDirectoryOnly);
    static void GetFiles(const std::string& path, bool includeChild, const EnumFileCallback& callback);

   /**
    *  get all dirs from a directory
    *
    *  @param dirPath The path of the directory.
    */
    static void GetDirectories(const std::string& path, std::vector<std::string>& dirs, SearchOption searchOption = SearchOption::TopDirectoryOnly);
};
NS_FK_END
#endif // FOUNDATIONKIT_DIRECTORY_HPP
