/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/

#include <fstream>
#include <unordered_map>
#include "FoundationKit/Base/lexical_cast.hpp"
#include "FoundationKit/Foundation/TimeZone.hpp"
#include "FoundationKit/Foundation/StringUtils.hpp"
#include "FoundationKit/Math/Math.hpp"
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
#define stat64 stat
#endif

NS_FK_BEGIN
#if PLATFORM_WINDOWS
const char PlatformNewLine[] ={ '\r', '\n' };
#elif (PLATFORM_MAC)||(PLATFORM_IOS)
const char PlatformNewLine[] ={ '\r'};
#else
const char PlatformNewLine[] = { '\n' };
#endif


namespace detail
{
    std::vector<uint8> ReadDataFromFile(const std::string& path, bool isText = false)
    {
        std::vector<uint8> FileAllBytes;
        do
        {
            size_t FileSize = static_cast<size_t>(File::GetSize(path));
            //FKLog("FileSize:%d", FileSize);
            BREAK_IF(FileSize <= 0);
            FILE* FileHandle = nullptr;
            if (isText)
                FileHandle = File::Open(path, "rt");
            else
                FileHandle = File::Open(path, "rb");

            //FKLog("FileHandle:%p", FileHandle);
            BREAK_IF(!FileHandle);
            FileAllBytes.resize(FileSize);
            size_t ReadCount = 0;
            while(ReadCount < FileSize)
            {
                size_t NumRead = fread((FileAllBytes.data()+ ReadCount), sizeof(uint8), 1024, FileHandle);
                if (NumRead == 0)
                {
                    if (errno == EINTR)
                        continue;
                    if (feof(FileHandle) == 0) // read file error.
                    {
                        FKLog("Error while read %s: %s\n", path.c_str(), strerror(errno));
                    }
                    break;
                }
                ReadCount += NumRead;
            }
            fclose(FileHandle);
        } while (false);
        return FileAllBytes;
    }

    bool WriteLinesToFile(const std::string& path, const std::vector<std::string>& contents, bool bAppend = false)
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

FILE* File::Open(const std::string& path, const char* mode/* = "r"*/)
{
    return fopen(path.c_str(), mode);
}

FILE* File::Open(const std::string& path, FileMode mode)
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

bool File::Close(FILE* file)
{
    return (fclose(file) == 0);
}

bool File::Close(int file)
{
#if PLATFORM_WINDOWS
    return (_close(file) == 0);
#else
    return (close(file) == 0);
#endif
}

bool File::AppendAllLines(const std::string& path, const std::vector<std::string>& contents)
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
std::vector<uint8> File::ReadAllBytesFromZip(const std::string& path, const std::string& fileName)
{
    std::vector<uint8> retData;
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
        retData.resize(fileInfo.uncompressed_size);
        memcpy(retData.data(), buffer, fileInfo.uncompressed_size);
    } while (0);

    if (file)
    {
        unzClose(file);
    }
    return retData;
}

std::vector<uint8> FoundationKit::File::ReadAllBytes(const std::string& path)
{
    return detail::ReadDataFromFile(path);
}

std::vector<std::string> File::ReadAllLines(const std::string& path)
{
    std::vector<std::string> lines;
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
    std::vector<uint8> FileTexts = detail::ReadDataFromFile(path, true);
    return std::string(reinterpret_cast<char*>(FileTexts.data()), FileTexts.size());
}

bool File::WriteAllBytes(const std::string& path, const char* bytes, size_t length)
{
    return detail::WriteDataToFile(path, bytes, length);
}

bool File::WriteAllLines(const std::string& path, const std::vector<std::string>& contents)
{
    return detail::WriteLinesToFile(path, contents);
}

bool File::WriteAllText(const std::string& path, const std::string& contents)
{
    return detail::WriteDataToFile(path, contents.c_str(), contents.size(), true);
}

bool File::Copy(const std::string& sourceFileName, const std::string& destFileName, bool overwrite /*= false*/)
{
    const int64 MaxBufferSize = 1024 * 1024;

    if (!overwrite && IsExists(destFileName))
    {
        FKLog("Target file is exist.");
        return false;
    }

    std::string dirPath = Path::GetPath(destFileName);
    Directory::Create(dirPath);

    FILE* FromFile = File::Open(sourceFileName, "rb");
    if (!FromFile)
    {
        return false;
    }
    FILE* ToFile = File::Open(destFileName, "wb");
    if (!ToFile)
    {
        return false;
    }
    int64 Size = File::GetSize(sourceFileName);
    if (Size < 1)
    {
        ASSERT_IF(Size != 0,"Get file size error.");
        return true;
    }
    size_t AllocSize = static_cast<size_t>(Math::Min<int64>(MaxBufferSize, Size));
    uint8* Buffer = new uint8[AllocSize];
    ASSERT_IF(Buffer == nullptr, "Out of memory.");
    bool OperationSucceeded = false;
    while (Size > 0)
    {
        size_t NumRead = fread(Buffer, sizeof(char), AllocSize, FromFile);
        if (NumRead == 0)
        {
            if (feof(FromFile) == 0) // read file error.
            {
                OperationSucceeded = false;
                FKLog("fread file error:%d", errno);
            }
            break;
        }
        size_t NumWrite = fwrite(Buffer, sizeof(char), NumRead, ToFile);
        if (NumWrite == 0 || NumWrite != NumRead)
        {
            OperationSucceeded = false;
            FKLog("fwrite file error:%d", errno);
            break;
        }
        Size -= NumRead;
    }
    delete[] Buffer;
    File::Close(FromFile);
    File::Close(ToFile);
    return OperationSucceeded;
}

int64 File::GetSize(const std::string& path)
{
    ASSERT_IF(path.empty(), "filepath must be not empty.");
    int64 ResultFileSize = -1;
    struct stat64 info;
    if (stat64(path.c_str(), &info) == 0)
    {
        ResultFileSize = info.st_size;
    }
    else
    {
        FILE* FileHandle = Open(path, "r");
        if (FileHandle)
        {
            fseek64(FileHandle, 0, SEEK_END);
            ResultFileSize = ftell64(FileHandle);
            fseek64(FileHandle, 0, SEEK_SET);
        }
    }

    if (ResultFileSize == 0 || ResultFileSize == -1)
    {
        //Get the size of a file by reading it until the end. This is needed
        //because files under /proc do not always return a valid size when
        //using fseek(0, SEEK_END) + ftell(). Nor can they be mmap()-ed.
        FILE* FileHandle = Open(path, "r");
        for (;;) 
        {
            char buff[256];
            size_t NumRead = fread(buff,1, sizeof(buff), FileHandle);
            if (NumRead == 0)
            {
                if (errno == EINTR)
                    continue;
                if (feof(FileHandle) == 0) // read file error.
                {
                    FKLog("Error while reading %s: %s\n", path.c_str(), strerror(errno));
                }
                break;
            }
            ResultFileSize += NumRead;
        }
    }
    return ResultFileSize;
}

bool File::SetSize(const std::string& path, size_t size)
{
#if PLATFORM_WINDOWS
    std::wstring widePath = StringUtils::string2wstring(path);
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

