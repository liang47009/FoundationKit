#include <fstream>
#include "FoundationKit/Platform/File.hpp"
#include "FoundationKit/Platform/Path.hpp"
#include "FoundationKit/Platform/Directory.hpp"

#if (TARGET_PLATFORM==PLATFORM_WINDOWS)
#include <Windows.h>
#endif
#include "unzip.h"

NS_FK_BEGIN
#if TARGET_PLATFORM == PLATFORM_WINDOWS
const char PlatformNewLine[] ={ '\r', '\n' };
#elif (TARGET_PLATFORM == PLATFORM_ANDROID) ||(TARGET_PLATFORM == PLATFORM_LINUX)
const char PlatformNewLine[] ={'\n'};
#elif (TARGET_PLATFORM == PLATFORM_MAC)||(TARGET_PLATFORM == PLATFORM_IOS)
const char PlatformNewLine[] ={ '\r'};
#endif

mutable_buffer ReadDataFromFile(const std::string& path, bool isText = false)
{
    mutable_buffer  FileAllBytes;
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
        FileAllBytes.reallocate(FileSize);
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
        fopen(path.c_str(), "wt");
    else
        fopen(path.c_str(), "wb");
    if (!FileHandle) return false;
    size_t WriteSize = fwrite(bytes, 1, length, FileHandle);
    return (WriteSize == length);
}


bool File::Copy(const std::string& sourceFileName, const std::string& destFileName, bool overwrite /*= false*/)
{
    bool ret = false;
    do
    {
        if (sourceFileName == destFileName)
        {
            ret = true;
            break;
        }

        if (!overwrite && IsExists(destFileName))
        {
            ret = false;
            break;
        }

        std::string dirPath = Path::GetDirectoryName(destFileName);
        Directory::CreateDirectory(dirPath);

        int64 srcFileSize = GetSize(sourceFileName);
        BREAK_IF(srcFileSize == -1);
        FILE *fpSrc = fopen(sourceFileName.c_str(), "rb");
        BREAK_IF(!fpSrc);
        FILE *fpDes = fopen(destFileName.c_str(), "wb");
        BREAK_IF(!fpDes);
        const size_t BUFF_SIZE = 1024;
        char read_buff[BUFF_SIZE];
        size_t fileOriginalSize = static_cast<size_t>(srcFileSize);
        size_t readedSize = 0;
        size_t unreadSize = fileOriginalSize;
        size_t writedSize = 0;
        while (readedSize < fileOriginalSize)
        {
            if (unreadSize > BUFF_SIZE)
            {
                readedSize += fread(read_buff, sizeof(char), BUFF_SIZE, fpSrc);
                writedSize += fwrite(read_buff, 1, BUFF_SIZE, fpDes);
            }
            else
            {
                readedSize += fread(read_buff, sizeof(char), unreadSize, fpSrc);
                writedSize += fwrite(read_buff, 1, unreadSize, fpDes);
            }
            unreadSize = fileOriginalSize - readedSize;
        }
        fclose(fpSrc);
        fclose(fpDes);
        ret = (readedSize == writedSize);
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
    return WriteLinesToFile(path, contents, true);
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

mutable_buffer FoundationKit::File::ReadAllBytes(const std::string& path)
{
    return ReadDataFromFile(path);
}

#if (TARGET_PLATFORM==PLATFORM_IOS)
#define MINIZIP_FROM_SYSTEM
#endif
mutable_buffer File::ReadAllBytesFromZip(const std::string& path, const std::string& fileName)
{
    mutable_buffer retData;
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

std::string File::ReadAllText(const std::string& path)
{
    mutable_buffer FileTexts = ReadDataFromFile(path, true);
    return std::string(FileTexts.c_str(), FileTexts.size());
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

bool File::WriteAllBytes(const std::string& path, const char* bytes, size_t length)
{
    return WriteDataToFile(path, bytes, length);
}

bool File::WriteAllLines(const std::string& path, const FileLineType& contents)
{
    return WriteLinesToFile(path, contents);
}

bool File::WriteAllText(const std::string& path, const std::string& contents)
{
    return WriteDataToFile(path, contents.c_str(), contents.size(), true);
}

NS_FK_END


