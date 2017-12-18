#include <fstream>
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
                FileHandle = fopen(path.c_str(), "rt");
            else
                FileHandle = fopen(path.c_str(), "rb");
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
                FileHandle = fopen(path.c_str(), "ab");
            else
                FileHandle = fopen(path.c_str(), "wb");
            BREAK_IF(!FileHandle);
            File::FileLineType::const_iterator line = contents.begin();
            for (line; line != contents.end(); ++line)
            {
                fwrite((*line).c_str(), 1, (*line).size(), FileHandle);
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
            FileHandle = fopen(path.c_str(), "wt");
        else
            FileHandle = fopen(path.c_str(), "wb");
        if (!FileHandle) return false;
        size_t WriteSize = fwrite(bytes, 1, length, FileHandle);
        fclose(FileHandle);
        return (WriteSize == length);
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
        FILE *fpSrc = fopen(sourceFileName.c_str(), "rb");
        BREAK_IF(!fpSrc);
        FILE *fpDes = fopen(destFileName.c_str(), "wb");
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
    ASSERTED(!path.empty(), "filepath must be not empty.");
    struct stat info;
    int result = stat(path.c_str(), &info);
    if (result != 0)
    {
        return -1;
    }
    else
    {
        return (int64)(info.st_size);
    }
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
        FILE* FileHandle = fopen(path.c_str(), "at");
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
    infile.open(path.c_str());
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

NS_FK_END


