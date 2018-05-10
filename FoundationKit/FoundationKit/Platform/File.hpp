/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_PLATFORM_FILE_HPP
#define FOUNDATIONKIT_PLATFORM_FILE_HPP

#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "FoundationKit/Foundation/DateTime.hpp"
#include "FoundationKit/Base/mutablebuf.hpp"

NS_FK_BEGIN

enum class FileMode
{
    ReadOnly = 0,
    WriteOnly = 1,
    ReadWrite = 2,
    Append = 3
};

class File
{
public:
    static FILE* Open(const std::string& path, const char* mode="r");
    static FILE* Open(const std::string& path, FileMode mode/* = FileMode::ReadOnly*/);
    static int   Open(const std::string& path, int mode/* = O_RDONLY | O_BINARY*/);
    static bool  Close(FILE* file);
    static bool  Close(int file);
    static bool AppendAllLines(const std::string& path, const std::vector<std::string>& contents);
    static bool AppendAllText(const std::string& path, const std::string& contents);
    static std::vector<uint8> ReadAllBytesFromZip(const std::string& path, const std::string& fileName);
    static std::vector<uint8> ReadAllBytes(const std::string& path);
    static std::vector<std::string> ReadAllLines(const std::string& path);
    static std::string ReadAllText(const std::string& path);
    static bool WriteAllBytes(const std::string& path, const char* bytes, size_t length);
    static bool WriteAllLines(const std::string& path, const std::vector<std::string>& contents);
    static bool WriteAllText(const std::string& path, const std::string& contents);

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


    /**
     * Copy file form sourceFileName to destFileName location.
     * @param sourceFileName
     *         The file source full path.
     * @param destFileName
     *         destination full path.
     * @param overwrite
     *         Whether overwrite if file exist.
     * @return true if the file have been copy successfully, false if not.
     *
     */
    static bool Copy(const std::string& sourceFileName, const std::string& destFileName, bool overwrite = false);

    /** Return the size of the file, or -1 if it doesn't exist. **/
    static int64 GetSize(const std::string& path);
    static bool  SetSize(const std::string& path, size_t size);
    static DateTime GetCreationTime(const std::string& path);
    static DateTime GetCreationTimeUtc(const std::string& path);
    static DateTime GetLastAccessTime(const std::string& path);
    static DateTime GetLastAccessTimeUtc(const std::string& path);
    static DateTime GetLastWriteTime(const std::string& path);
    static DateTime GetLastWriteTimeUtc(const std::string& path);
private:
    static std::string ErrnoToString(int error, const std::string& operation);
};

NS_FK_END

#endif // END OF FOUNDATIONKIT_PLATFORM_FILE_HPP
