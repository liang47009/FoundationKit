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

    std::string ErrnoToString(int err)
    {

        static std::unordered_map<int,std::string> ErrnoTranslationMap =
        {
            { EACCES ,"Search permission is denied for one of the directories in the path prefix of file path."},
            { EBADF ,"Invalid file descriptor."},
            { EFAULT ,"Bad address."},
            { ELOOP, "Too many symbolic links encountered while traversing the path." },
            { ENAMETOOLONG , "File path is too long."},
            { ENOENT,"A component of file path does not exist, or file path is an empty string and AT_EMPTY_PATH was not specified in flags." },
            { ENOMEM ,"Out of memory (i.e., kernel memory)."},
            { ENOTDIR ," A component of the path prefix of file path is not a directory."},
            { EOVERFLOW ,"Path or file descriptor refers to a file whose size, inode number,\n \
                          or number of blocks cannot be represented in, respectively, \n \
                          the types off_t, ino_t, or blkcnt_t.This error can occur when, \n \
                          for example, an application compiled on a 32 - bit platform without \n \
                          - D_FILE_OFFSET_BITS = 64 calls stat() on a file whose size \n \
                          exceeds(1 << 31) - 1 bytes."},
             { EINVAL ,"Invalid flag specified in flags."},
             { ENOTDIR ,"pathname is relative and dirfd is a file descriptor referring to a file other than a directory."}
        };
        std::unordered_map<int, std::string>::iterator FoundIter = ErrnoTranslationMap.find(err);
        if (FoundIter != ErrnoTranslationMap.end())
        {
            return FoundIter->second;
        }
        return lexical_cast<std::string>(err);
    }

} //namespace detail

#if PLATFORM_ANDROID
extern FILE* AndroidOpenAsset(const char * path, const char * mode);
#endif

FILE* File::Open(const std::string& path, const char* mode, bool isAsset/* = false*/)
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
        const size_t BUFF_SIZE = 1024;
        char read_buff[BUFF_SIZE];
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
        fclose(fpSrc);
        fclose(fpDes);
    } while (false);
    return ret;
}

int64 File::GetSize(const std::string& path)
{
    ASSERT_IF(path.empty(), "filepath must be not empty.");
    int64 ResultFileSize = -1;
    struct stat info;
    int result = stat(path.c_str(), &info);
    if (result == 0)
    {
        ResultFileSize = (int64)(info.st_size);
    }
    else
    {
        FKLog("stat falied:%s,Try use fopen->fseek-ftell-fseek.", detail::ErrnoToString(errno).c_str());
        FILE* FileHandle = Open(path, "r");
        if (FileHandle)
        {
            fseek(FileHandle, 0, SEEK_END);
            ResultFileSize = ftell(FileHandle);
            fseek(FileHandle, 0, SEEK_SET);
        }
    }
    return ResultFileSize;
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
    return TimeZone::ToLocalTime(GetCreationTimeUtc(path));
}

DateTime File::GetCreationTimeUtc(const std::string& path)
{
    DateTime dt;
    struct stat info;
    int result = stat(path.c_str(), &info);
    ASSERTED(!result, StringUtils::Format("stat falied:%s", detail::ErrnoToString(errno).c_str()).c_str());
    if (result == 0)
    {
        dt = DateTime::FromUnixTimestamp(info.st_ctime, ETimeKind::Utc);
    }
    return dt;
}

DateTime File::GetLastAccessTime(const std::string& path)
{
    return TimeZone::ToLocalTime(GetLastAccessTimeUtc(path));
}

DateTime File::GetLastAccessTimeUtc(const std::string& path)
{
    DateTime dt;
    struct stat info;
    int result = stat(path.c_str(), &info);
    ASSERTED(!result, StringUtils::Format("stat falied:%s", detail::ErrnoToString(errno).c_str()).c_str());
    if (result == 0)
    {
        dt = TimeZone::ToLocalTime(DateTime::FromUnixTimestamp(info.st_atime, ETimeKind::Utc));
    }
    return dt;
}

DateTime File::GetLastWriteTime(const std::string& path)
{
    return TimeZone::ToLocalTime(GetLastWriteTimeUtc(path));
}

DateTime File::GetLastWriteTimeUtc(const std::string& path)
{
    DateTime dt;
    struct stat info;
    int result = stat(path.c_str(), &info);
    ASSERTED(!result, StringUtils::Format("stat falied:%s", detail::ErrnoToString(errno).c_str()).c_str());
    if (result == 0)
    {
        dt = TimeZone::ToLocalTime(DateTime::FromUnixTimestamp(info.st_mtime, ETimeKind::Utc));
    }
    return dt;
}

NS_FK_END

