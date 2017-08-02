/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PLATFORM_DIRECTORY_HPP
#define FOUNDATIONKIT_PLATFORM_DIRECTORY_HPP
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
    *  Implement on platform.
    *  Creates a directory.
    *
    *  @param dirPath The path of the directory, it must be an absolute path.
    *  @return True if the directory have been created successfully, false if not.
    */
    static bool CreateDirectory(const std::string& path);

   /**
    *  Implement on platform.
    *  Removes a directory.
    *
    *  @param dirPath  The full path of the directory, it must be an absolute path.
    *  @return True if the directory have been removed successfully, false if not.
    */
    static bool RemoveDirectory(const std::string& path);

   /**
    *  Implement on platform.
    *  @return True if the directory moved, false if not.
    */
    static bool Move(const std::string& sourceDirName, const std::string& destDirName);

   /**
    *  Implement on platform.
    *  Checks whether the path is a directory.
    *
    *  @param dirPath The path of the directory, it could be a relative or an absolute path.
    *  @return True if the directory exists, false if not.
    */
    static bool IsExists(const std::string& path);

   /**
    *  get all files from a directory
    *
    *  @param dirPath The path of the directory.
    */
    static void GetFiles(const std::string& path, std::vector<std::string>& files, SearchOption searchOption = SearchOption::TopDirectoryOnly);
    static void GetFiles(const std::string& path, const EnumFileCallback& callback, SearchOption searchOption = SearchOption::TopDirectoryOnly);

   /**
    *  get all dirs from a directory
    *
    *  @param dirPath The path of the directory.
    */
    static void GetDirectories(const std::string& path, std::vector<std::string>& dirs, SearchOption searchOption = SearchOption::TopDirectoryOnly);

};
NS_FK_END
#endif // FOUNDATIONKIT_PLATFORM_DIRECTORY_HPP
