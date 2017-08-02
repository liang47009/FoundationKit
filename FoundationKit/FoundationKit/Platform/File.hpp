/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PLATFORM_FILE_HPP
#define FOUNDATIONKIT_PLATFORM_FILE_HPP

#include <string>
#include <vector>
#include <sys/stat.h>
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Base/multiple_buffer.hpp"

NS_FK_BEGIN

// Specifies how the operating system should open a file.
enum class FileMode
{
    // Specifies that the operating system should create a new file. 
    // If the file already exists,will fail.
    CreateNew = 1,

    // Specifies that the operating system should create a new file. If the file already
    // exists, it will be overwritten. FileMode::Create is equivalent to requesting that 
    // if the file does not exist, use FileMode::CreateNew; otherwise, use FileMode::Truncate.
    Create = 2,

    // Specifies that the operating system should open an existing file. The ability
    // to open the file is dependent on the value specified by the System.IO.FileAccess
    // enumeration. A System.IO.FileNotFoundException exception is thrown if the file
    // does not exist.
    Open = 3,

    // Specifies that the operating system should open a file if it exists; otherwise,
    // a new file should be created. If the file is opened with FileAccess.Read, System.Security.Permissions.FileIOPermissionAccess.Read
    // permission is required. If the file access is FileAccess.Write, System.Security.Permissions.FileIOPermissionAccess.Write
    // permission is required. If the file is opened with FileAccess.ReadWrite, both
    // System.Security.Permissions.FileIOPermissionAccess.Read and System.Security.Permissions.FileIOPermissionAccess.Write
    // permissions are required.
    OpenOrCreate = 4,

    // Specifies that the operating system should open an existing file. When the file
    // is opened, it should be truncated so that its size is zero bytes. This requires
    // System.Security.Permissions.FileIOPermissionAccess.Write permission. Attempts
    // to read from a file opened with FileMode.Truncate cause an System.ArgumentException
    // exception.
    Truncate = 5,

    // Opens the file if it exists and seeks to the end of the file, or creates a new
    // file. 
    Append = 6
};

// Defines constants for read, write, or read/write access to a file.
enum class FileAccess
{
    // Read access to the file. Data can be read from the file. Combine with Write for
    // read/write access.
    Read = 1,

    // Write access to the file. Data can be written to the file. Combine with Read
    // for read/write access.
    Write = 2,

    // Read and write access to the file. Data can be written to and read from the file.
    ReadWrite = 3
};

class File
{
public:
    typedef std::vector<std::string> FileLineType;

    static bool Copy(const std::string& sourceFileName, const std::string& destFileName, bool overwrite = false);

   /**
    *  Implement on platform.
    *  Move a file from sourceFileName to destFileName directory.
    *
    *  @param sourceFileName  The current full path of the file. Includes path and name.
    *  @param destFileName  The new full path of the file. Includes path and name.
    *  @return true if the file have been renamed successfully, false if not.
    */
    static bool Move(const std::string& sourceFileName, const std::string& destFileName);

    //Implement on platform.
    static bool Delete(const std::string& path);

    //Implement on platform.
    static bool IsExists(const std::string& path);

   /**
    *  Implement on platform.
    *  Renames a file under the given directory.
    *
    *  @param sourceFileName  The current full path of the file. Includes path and name.
    *  @param destFileName  The new full path of the file. Includes path and name.
    *  @return true if the file have been renamed successfully, false if not.
    */
    static bool Rename(const std::string& sourceFileName, const std::string& destFileName);

    // Implement on platform.
    static int64 GetSize(const std::string& path);

    static bool AppendAllLines(const std::string& path, const FileLineType& contents);
    static bool AppendAllText(const std::string& path, const std::string& contents);
    static mutable_buffer ReadAllBytes(const std::string& path);
    static mutable_buffer ReadAllBytesFromZip(const std::string& path, const std::string& fileName);
    static std::string ReadAllText(const std::string& path);
    static FileLineType ReadAllLines(const std::string& path);
    static bool WriteAllBytes(const std::string& path, const char* bytes, size_t length);
    static bool WriteAllLines(const std::string& path, const FileLineType& contents);
    static bool WriteAllText(const std::string& path, const std::string& contents);

    //static DateTime GetCreationTime(const std::string& path);
    //static DateTime GetCreationTimeUtc(const std::string& path);
    //static DateTime GetLastAccessTime(const std::string& path);
    //static DateTime GetLastAccessTimeUtc(const std::string& path);
    //static DateTime GetLastWriteTime(const std::string& path);
    //static DateTime GetLastWriteTimeUtc(const std::string& path);
    //static void SetCreationTime(const std::string& path, DateTime creationTime);
    //static void SetCreationTimeUtc(const std::string& path, DateTime creationTimeUtc);
    //static void SetLastAccessTime(const std::string& path, DateTime lastAccessTime);
    //static void SetLastAccessTimeUtc(const std::string& path, DateTime lastAccessTimeUtc);
    //static void SetLastWriteTime(const std::string& path, DateTime lastWriteTime);
    //static void SetLastWriteTimeUtc(const std::string& path, DateTime lastWriteTimeUtc);
};


NS_FK_END





#endif // END OF FOUNDATIONKIT_PLATFORM_FILE_HPP
