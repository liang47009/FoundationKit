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
    /**
     * @param[in] FilePath  File full path.
     * @return If true break.
     */
    typedef std::function<bool(const std::string& FilePath)> EnumFileCallback;
    enum class ESearchOption
    {
        TopDirectoryOnly = 0,
        AllDirectories = 1
    };

   /**
    *  Implement on platform.
    *  Creates a directory.
    *
    *  @param path The path of the directory, it must be an absolute path.
    *  @return True if the directory have been created successfully, false if not.
    */
    static bool Create(const std::string& path);

   /**
    *  Implement on platform.
    *  Removes a directory.
    *
    *  @param path  The full path of the directory, it must be an absolute path.
    *  @return True if the directory have been removed successfully, false if not.
    */
    static bool Remove(const std::string& path);

   /**
    *  Implement on platform.
    *  @return True if the directory moved, false if not.
    */
    static bool Move(const std::string& sourceDirName, const std::string& destDirName);

   /**
    *  Implement on platform.
    *  Checks whether the path is a directory.
    *
    *  @param path The path of the directory, it could be a relative or an absolute path.
    *  @return True if the directory exists, false if not.
    */
    static bool Exists(const std::string& path);

   /**
    *  Implement on platform.
    *  Sets the application's current working directory to the specified directory.
    *  @path: The path to which the current working directory is set.
    */
    static bool SetCurrentDirectory(const std::string& path);

   /**
    *  Implement on platform.
    *  Gets the current working directory of the application.
    *  @return A string that contains the path of the current working directory, and does not
    *          end with a (/).
    */
    static std::string GetCurrentDirectory();

   /**
    *  Get all files from a directory
    *
    *  @param path The path of the directory.
    */
    static void GetFiles(const std::string& path, std::vector<std::string>& files, ESearchOption searchOption = ESearchOption::TopDirectoryOnly);
    static void GetFiles(const std::string& path, const EnumFileCallback& callback, ESearchOption searchOption = ESearchOption::TopDirectoryOnly);

   /**
    *  Get all dirs from a directory
    *
    *  @param path The path of the directory.
    */
    static void GetDirectories(const std::string& path, std::vector<std::string>& dirs, ESearchOption searchOption = ESearchOption::TopDirectoryOnly);


protected:
    static std::string CurrentDirectory;
};
NS_FK_END
#endif // FOUNDATIONKIT_PLATFORM_DIRECTORY_HPP
