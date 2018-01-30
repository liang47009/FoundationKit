/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/

#include <fstream>
#include <unordered_map>
#include "FoundationKit/Base/lexical_cast.hpp"
#include "FoundationKit/Foundation/TimeZone.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/Directory.hpp"
#include "unzip.h"

#if PLATFORM_WINDOWS
#include <io.h>
#define stat64 _stati64
#define ftell64 _ftelli64
#define fseek64 _fseeki64
#else
#include <unistd.h>
#define ftell64 ftello
#define fseek64 fseeko
#endif

NS_FK_BEGIN
#if PLATFORM_WINDOWS
const char PlatformNewLine[] ={ '\r', '\n' };
#elif (PLATFORM_ANDROID) ||(PLATFORM_LINUX)
const char PlatformNewLine[] ={'\n'};
#elif (PLATFORM_MAC)||(PLATFORM_IOS)
const char PlatformNewLine[] ={ '\r'};
#endif


namespace detail
{
    mutablebuf ReadDataFromFile(const std::string& path, bool isText = false)
    {
        mutablebuf  FileAllBytes;
        do
        {
            FILE* FileHandle = nullptr;
            if (isText)
                FileHandle = File::Open(path, "rt");
            else
                FileHandle = File::Open(path, "rb");
            BREAK_IF(!FileHandle);
            fseek(FileHandle, 0, SEEK_END);
            size_t FileSize = ftell(FileHandle);
            fseek(FileHandle, 0, SEEK_SET);
            FileAllBytes.allocate(FileSize);
            fread(FileAllBytes.data(), 1, FileSize, FileHandle);
            fclose(FileHandle);
        } while (false);
        return FileAllBytes;
    }

    bool WriteLinesToFile(const std::string& path, const File::FileLineType& contents, bool bAppend = false)
    {
        bool result = false;
        do
        {
            FILE* FileHandle = nullptr;
            if (bAppend)
                FileHandle = File::Open(path, "ab");
            else
                FileHandle = File::Open(path, "wb");
            BREAK_IF(!FileHandle);
            for (auto& line : contents)
            {
                fwrite(line.c_str(), 1, line.size(), FileHandle);
                fwrite(PlatformNewLine, 1, sizeof(PlatformNewLine), FileHandle);
            }
            fclose(FileHandle);
            result = true;
        } while (false);
        return result;
    }

    bool WriteDataToFile(const std::string& path, const char* bytes, size_t length, bool isText = false)
    {
        FILE* FileHandle = nullptr;
        if (isText)
            FileHandle = File::Open(path, "wt");
        else
            FileHandle = File::Open(path, "wb");
        if (!FileHandle) return false;
        size_t WriteSize = fwrite(bytes, 1, length, FileHandle);
        fclose(FileHandle);
        return (WriteSize == length);
    }
} //namespace detail

#if PLATFORM_ANDROID
extern FILE* AndroidOpenAsset(const char * path, const char * mode);
#endif

FILE* File::Open(const std::string& path, const char* mode/* = "r"*/, bool isAsset/* = false*/)
{
    FILE* FileHandle = nullptr;
#if PLATFORM_ANDROID
    if (isAsset)
    {
        FileHandle = AndroidOpenAsset(path.c_str(), mode);
    }
    else
#endif
    {
        UNUSED_ARG(isAsset);
        FileHandle = fopen(path.c_str(), mode);
    }
    return FileHandle;
}

FILE* File::Open(const std::string& path, FileMode mode/* = FileMode::ReadOnly*/)
{
    const char* fileMode = nullptr;
    switch (mode) {
    case FileMode::ReadOnly:
        fileMode = "rb";
        break;
    case FileMode::WriteOnly:
        fileMode = "wb";
        break;
    case FileMode::ReadWrite:
        fileMode = "r+b";
        break;
    case FileMode::Append:
        fileMode = "ab";
        break;
    }
    return fopen(path.c_str(), fileMode);
}

int File::Open(const std::string& path, int mode)
{
#if _WIN32
    return _open(path.c_str(), mode);
#else
    return open(path.c_str(), mode);
#endif
}

bool File::Copy(const std::string& sourceFileName, const std::string& destFileName, bool overwrite /*= false*/)
{
    bool ret = false;
    do
    {
        if (sourceFileName == destFileName)
        {
            FKLog("Can not copy self.");
            break;
        }

        if (!overwrite && IsExists(destFileName))
        {
            FKLog("Target file is exist.");
            break;
        }

        std::string dirPath = Path::GetDirectoryPath(destFileName);
        Directory::Create(dirPath);
        int64 srcFileSize = GetSize(sourceFileName);
        BREAK_IF(srcFileSize == -1);
        FILE *fpSrc = Open(sourceFileName, "rb");
        BREAK_IF(!fpSrc);
        FILE *fpDes = Open(destFileName, "wb");
        BREAK_IF(!fpDes);
        const size_t BUFF_SIZE = 1024*1024*10; //10M
        char* read_buff = new char[BUFF_SIZE];
        size_t TotalReadSize = 0;
        size_t FileOriginalSize = static_cast<size_t>(srcFileSize);
        ret = true;
        while (TotalReadSize < FileOriginalSize)
        {
            size_t NumRead = fread(read_buff, sizeof(char), BUFF_SIZE, fpSrc);
            if (NumRead == 0) 
            {
                if (feof(fpSrc) == 0) // read file error.
                {
                    ret = false;
                    FKLog("fread file error:%d", errno);
                }
                break;
            }
            size_t NumWrite = fwrite(read_buff, sizeof(char), NumRead, fpDes);
            if (NumWrite == 0 || NumWrite != NumRead)
            {
                ret = false;
                FKLog("fwrite file error:%d", errno);
            }
            fflush(fpDes);
            TotalReadSize += NumRead;
        }
        delete[] read_buff;
        fclose(fpSrc);
        fclose(fpDes);
    } while (false);
    return ret;
}

int64 File::GetSize(const std::string& path)
{
    ASSERT_IF(path.empty(), "filepath must be not empty.");
    int64 ResultFileSize = -1;
    struct stat64 info;
    int result = stat64(path.c_str(), &info);
    if (result == 0)
    {
        ResultFileSize = info.st_size;
    }
    else
    {
        FKLog("%s,Try use fopen->fseek64-ftell64-fseek64.", ErrnoToString(errno,"stat").c_str());
        FILE* FileHandle = Open(path, "r");
        if (FileHandle)
        {
            fseek64(FileHandle, 0, SEEK_END);
            ResultFileSize = ftell64(FileHandle);
            fseek64(FileHandle, 0, SEEK_SET);
        }
    }
    return ResultFileSize;
}

bool File::SetSize(const std::string& path, size_t size)
{
#if PLATFORM_WINDOWS
    std::wstring widePath = StringUtils::string2UTF8wstring(path);
    HANDLE hFile = CreateFileW(widePath.c_str(), GENERIC_WRITE, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        //LONG sizehigh = 0;
        //LONG sizelow = (size & 0xffffffff);
        LARGE_INTEGER largeSize;
        largeSize.QuadPart = size;
        if (SetFilePointerEx(hFile, largeSize, NULL, FILE_BEGIN))
        {
            if (SetEndOfFile(hFile))
            {
                CloseHandle(hFile);
                return true;
            }
        }
        CloseHandle(hFile);
    }
#else
    if (truncate(path.c_str(), size) == 0)
        return true;
#endif
    return false;
}

bool File::AppendAllLines(const std::string& path, const FileLineType& contents)
{
    return detail::WriteLinesToFile(path, contents, true);
}

bool File::AppendAllText(const std::string& path, const std::string& contents)
{
    bool result = false;
    do
    {
        FILE* FileHandle = Open(path.c_str(), "at");
        BREAK_IF(!FileHandle);
        fwrite(contents.c_str(), 1, contents.size(), FileHandle);
        fclose(FileHandle);
        result = true;
    } while (false);
    return result;
}

#if (PLATFORM_IOS)
#define MINIZIP_FROM_SYSTEM
#endif
mutablebuf File::ReadAllBytesFromZip(const std::string& path, const std::string& fileName)
{
    mutablebuf retData;
    unzFile file = nullptr;
    do
    {
        BREAK_IF(path.empty());
        file = unzOpen(path.c_str());
        BREAK_IF(!file);
        // FIXME: Other platforms should use upstream minizip like mingw-w64
#ifdef MINIZIP_FROM_SYSTEM
        int ret = unzLocateFile(file, fileName.c_str(), NULL);
#else
        int ret = unzLocateFile(file, fileName.c_str(), 1);
#endif
        BREAK_IF(UNZ_OK != ret);
        char filePathA[260];
        unz_file_info fileInfo;
        ret = unzGetCurrentFileInfo(file, &fileInfo, filePathA, sizeof(filePathA), nullptr, 0, nullptr, 0);
        BREAK_IF(UNZ_OK != ret);
        ret = unzOpenCurrentFile(file);
        BREAK_IF(UNZ_OK != ret);
        unsigned char * buffer = new unsigned char[fileInfo.uncompressed_size];
        int readedSize = unzReadCurrentFile(file, buffer, static_cast<unsigned>(fileInfo.uncompressed_size));
        ASSERTED(readedSize == 0 || readedSize == (int)fileInfo.uncompressed_size, "the file size is wrong");
        UNUSED_ARG(readedSize);
        unzCloseCurrentFile(file);
        retData.assign(buffer, fileInfo.uncompressed_size, true);

    } while (0);

    if (file)
    {
        unzClose(file);
    }
    return retData;
}

mutablebuf FoundationKit::File::ReadAllBytes(const std::string& path)
{
    return detail::ReadDataFromFile(path);
}

File::FileLineType File::ReadAllLines(const std::string& path)
{
    FileLineType lines;
    std::string line;
    std::ifstream infile;
    infile.open(path);
    if (!infile.good()) return lines;
    while (!infile.eof()) // To get you all the lines.
    {
        std::getline(infile, line); // Saves the line in STRING.
        lines.push_back(line);
    }
    infile.close();
    return lines;
}

std::string File::ReadAllText(const std::string& path)
{
    mutablebuf FileTexts = detail::ReadDataFromFile(path, true);
    return std::string(FileTexts.c_str(), FileTexts.size());
}

bool File::WriteAllBytes(const std::string& path, const char* bytes, size_t length)
{
    return detail::WriteDataToFile(path, bytes, length);
}

bool File::WriteAllLines(const std::string& path, const FileLineType& contents)
{
    return detail::WriteLinesToFile(path, contents);
}

bool File::WriteAllText(const std::string& path, const std::string& contents)
{
    return detail::WriteDataToFile(path, contents.c_str(), contents.size(), true);
}

DateTime File::GetCreationTime(const std::string& path)
{
    return GetCreationTimeUtc(path).ToLocalTime();
}

DateTime File::GetCreationTimeUtc(const std::string& path)
{
    DateTime dt;
    struct stat64 info;
    int result = stat64(path.c_str(), &info);
    ASSERTED(!result, ErrnoToString(errno, "stat").c_str());
    if (result == 0)
    {
        dt = DateTime::FromUnixTimestamp(info.st_ctime, ETimeKind::Utc);
    }
    return dt;
}

DateTime File::GetLastAccessTime(const std::string& path)
{
    return GetLastAccessTimeUtc(path).ToLocalTime();
}

DateTime File::GetLastAccessTimeUtc(const std::string& path)
{
    DateTime dt;
    struct stat64 info;
    int result = stat64(path.c_str(), &info);
    ASSERTED(!result, ErrnoToString(errno, "stat").c_str());
    if (result == 0)
    {
        dt = DateTime::FromUnixTimestamp(info.st_atime, ETimeKind::Utc);
    }
    return dt;
}

DateTime File::GetLastWriteTime(const std::string& path)
{
    return GetLastWriteTimeUtc(path).ToLocalTime();
}

DateTime File::GetLastWriteTimeUtc(const std::string& path)
{
    DateTime dt;
    struct stat64 info;
    int result = stat64(path.c_str(), &info);
    ASSERTED(!result, ErrnoToString(errno, "stat").c_str());
    if (result == 0)
    {
        dt = DateTime::FromUnixTimestamp(info.st_mtime, ETimeKind::Utc);
    }
    return dt;
}

std::string File::ErrnoToString(int error, const std::string& operation)
{
    const char* opstr = operation.c_str();
    switch (error)
    {
    case ENAMETOOLONG:
        return StringUtils::Format("%s failed because file path(name) is too long", opstr);
    case ENOTDIR:
        return StringUtils::Format("%s failed:  path is not a directory,from directory to non-directory", opstr);
    case EISDIR:
        return StringUtils::Format("%s failed: from non-directory to directory", opstr);
    case EXDEV:
        return StringUtils::Format("%s failed: to and from are on different file systems", opstr);
    case EIO:
        return StringUtils::Format("%s failed: I/O error updating directory", opstr);
    case EROFS:
        return StringUtils::Format("%s failed: read only file system", opstr);
    case EFAULT:
        return StringUtils::Format("%s failed: segmentation fault(Bad address)", opstr);
    case EINVAL:
        return StringUtils::Format("%s failed: from is a parent of to, or rename of . or ..", opstr);
    case ENOTEMPTY:
        return StringUtils::Format("%s failed: to is a directory and not empty", opstr);
    case EPERM:
        return StringUtils::Format("%s failed because the operation was not permitted", opstr);
    case ENOENT:
        return StringUtils::Format("%s failed because the file or directory does not exist", opstr);
    case ENOMEM:
        return StringUtils::Format("%s failed because there was not enough memory available", opstr);
    case EACCES:
        return StringUtils::Format("%s failed because permission for the file was denied", opstr);
    case EBADF:
        return StringUtils::Format("%s failed because file descriptor is invalid.", opstr);
    case EEXIST:
        return StringUtils::Format("%s failed because the file already exists", opstr);
    case ENOSPC:
        return StringUtils::Format("%s failed because there is no disk space left. Please free some disk space and continue.", opstr);
    case ELOOP:
        return StringUtils::Format("%s failed: too many symbolic links encountered while traversing the path.", opstr);
#if PLATFORM_IOS
    case EAUTH:
        return StringUtils::Format("%s failed because of an authentication failure", opstr);
    case ENEEDAUTH:
        return StringUtils::Format("%s failed because you need an authenticator", opstr);
    case EDQUOT:
        return StringUtils::Format("%s failed: quota limit reached", opstr);
#endif
    default:
        return StringUtils::Format("%s failed with error: %s", opstr, strerror(error));
    }
}

NS_FK_END

