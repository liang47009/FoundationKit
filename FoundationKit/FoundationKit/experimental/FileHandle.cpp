#include "FoundationKit/Platform/FileHandle.hpp"
NS_FK_BEGIN


FileHandle::FileHandle(FILE * Handle)
{
    NativeFileHandle = Handle;
}

FileHandle::~FileHandle()
{
    if (NativeFileHandle)
    {
        fclose(NativeFileHandle);
    }
}

int64 FileHandle::Tell()
{
    return ftell(NativeFileHandle);
}

bool FileHandle::Seek(int64 NewPosition)
{
    return fseek(NativeFileHandle, NewPosition, SEEK_SET) == NewPosition;
}

bool FileHandle::SeekFromEnd(int64 NewPositionRelativeToEnd /*= 0*/)
{
    return fseek(NativeFileHandle, NewPositionRelativeToEnd, SEEK_END)== NewPositionRelativeToEnd;
}

bool FileHandle::SeekFromCur(int64 NewPositionRelativeToCur /*= 0*/)
{
    return fseek(NativeFileHandle, NewPositionRelativeToCur, SEEK_CUR)== NewPositionRelativeToCur;
}

bool FileHandle::Read(uint8* Destination, int64 BytesToRead)
{
    return fread(Destination, 1, BytesToRead, NativeFileHandle) == BytesToRead;
}

bool FileHandle::Write(const uint8* Source, int64 BytesToWrite)
{
    return fwrite(Source, 1, BytesToWrite, NativeFileHandle) == BytesToWrite;
}

void FileHandle::Flush()
{
    fflush(NativeFileHandle);
}

NS_FK_END




