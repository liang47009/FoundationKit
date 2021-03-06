/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/

#include <mutex>
#include <vector>
#include <stdio.h>
#include <cassert>
#include "zlib.h"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
#include "FoundationKit/Foundation/Compression.hpp"
#include "FoundationKit/Foundation/File.hpp"
#include "FoundationKit/Base/scope_guard.hpp"

NS_FK_BEGIN

static void *zipAlloc(void *opaque, unsigned int size, unsigned int num)
{
    UNUSED_ARG(opaque);
    return ::malloc(size * num);
}

static void zipFree(void *opaque, void *p)
{
    UNUSED_ARG(opaque);
    ::free(p);
}

static int doInflate(z_stream* stream, std::vector<uint8>& head, size_t blockSize) {
    if (stream->avail_out == 0) 
    {
        size_t index = head.size();
        head.resize(head.size() + blockSize);
        stream->next_out = &head[index];
        stream->avail_out = static_cast<uInt>(blockSize);
    }

    int status = inflate(stream, Z_NO_FLUSH);

    switch (status)
    {
    case Z_OK:
    case Z_STREAM_END:
        break;
    case Z_BUF_ERROR:
    case Z_NEED_DICT:
    case Z_DATA_ERROR:
    case Z_MEM_ERROR:
        FKLog("doInflate inflate error:%d, msg:%s", status, stream->msg);
    default:
        FKLog("uncompressMemory inflate error:%d, msg:%s", status, stream->msg);
    }
    return status;
}


/** Time spent compressing data in seconds. */
long long Compression::compressorTime = 0;

/** Time spent uncompressing data in seconds. */
long long Compression::uncompressorTime = 0;

int32_t Compression::defaultBufferLength = int32_t(4) << 20; // 4MiB

bool Compression::CompressMemory(CompressionFlags Flags, std::vector<uint8>& CompressedBuffer, const mutablebuf& UncompressedBuffer)
{
    static std::mutex zlibScopeMutex;
    std::lock_guard<std::mutex> scopeLock(zlibScopeMutex);
    // Keep track of time spent uncompressing memory.
    ElapsedTimer CompressorStartTime;
    bool bOperationSucceeded = false;
    do 
    {
        z_stream stream;
        stream.zalloc = &zipAlloc;
        stream.zfree  = &zipFree;
        stream.opaque = Z_NULL;

        // Using deflateInit2() to support gzip.  "The windowBits parameter is the
        // base two logarithm of the maximum window size (...) The default value is
        // 15 (...) Add 16 to windowBits to write a simple gzip header and trailer
        // around the compressed data instead of a zlib wrapper. The gzip header
        // will have no file name, no extra data, no comment, no modification time
        // (set to zero), no header crc, and the operating system will be set to 255
        // (unknown)."
        int windowBits = DEFAULT_ZLIB_BIT_WINDOW + (Flags == COMPRESSION_GZIP ? 16 : 0);

        int status = deflateInit2(&stream,Z_DEFAULT_COMPRESSION,Z_DEFLATED,windowBits, MAX_MEM_LEVEL,Z_DEFAULT_STRATEGY);
        if (status != Z_OK)
        {
            FKLog("compressMemory deflateInit2 error:%d, msg:%s", status, stream.msg);
            break;
        }
        SCOPE_EXIT
        {
            status = deflateEnd(&stream);
            if (status != Z_OK)
            {
                FKLog("compressMemory deflateEnd error:%d, msg:%s", status, stream.msg);
            }
            else
            {
                bOperationSucceeded = true;
            }
        };

        uLong uncompressedLength = static_cast<uLong>(UncompressedBuffer.size());
        stream.next_in           = const_cast<uint8*>(UncompressedBuffer.data());
        stream.avail_in          = static_cast<uInt>(uncompressedLength);
        std::vector<uint8> tempData(uncompressedLength);
        stream.next_out  = static_cast<uint8*>(tempData.data());;
        stream.avail_out = static_cast<uInt>(uncompressedLength);
        while ((status   = deflate(&stream, Z_FINISH)) == Z_OK);
        if (status != Z_STREAM_END)
        {
            FKLog("compressMemory deflate error:%d, msg:%s", status, stream.msg);
            break;
        }
        CompressedBuffer.resize(stream.total_out);
        memcpy(CompressedBuffer.data(), tempData.data(), stream.total_out);
    } while (false);
    // Keep track of compression time and stats.
    compressorTime += CompressorStartTime.Milliseconds();
    return bOperationSucceeded;
}

bool Compression::UncompressMemory(CompressionFlags Flags, std::vector<uint8>& UncompressedBuffer, const mutablebuf& CompressedBuffer)
{
    static std::mutex zlibScopeMutex;
    std::lock_guard<std::mutex> scopeLock(zlibScopeMutex);
    // Keep track of time spent uncompressing memory.
    ElapsedTimer UncompressorStartTime;

    bool bOperationSucceeded = false;
    do 
    {
        z_stream stream;
        stream.zalloc = &zipAlloc;
        stream.zfree  = &zipFree;
        stream.opaque = nullptr;
        // "The windowBits parameter is the base two logarithm of the maximum window
        // size (...) The default value is 15 (...) add 16 to decode only the gzip
        // format (the zlib format will return a Z_DATA_ERROR)."
        int windowBits = DEFAULT_ZLIB_BIT_WINDOW + (Flags == COMPRESSION_GZIP ? 16 : 0);

        int status = inflateInit2(&stream, windowBits);
        if (status != Z_OK)
        {
            FKLog("Compression inflateInit2 error:%d, msg:%s", status, stream.msg);
            break;
        }

        SCOPE_EXIT
        {
            status = inflateEnd(&stream);
            if (status != Z_OK)
            {
                FKLog("uncompressMemory inflateEnd error:%d, msg:%s", status, stream.msg);
            }
            else
            {
                bOperationSucceeded = true;
            }
        };

        stream.next_in   = const_cast<uint8*>(CompressedBuffer.data());
        stream.avail_in  = static_cast<uInt>(CompressedBuffer.size());
        stream.next_out  = nullptr;
        stream.avail_out = 0;
        stream.total_in  = 0;
        stream.total_out = 0;
        std::vector<uint8>  UncompressedData;
        while ((status = doInflate(&stream, UncompressedData, defaultBufferLength)) == Z_OK);
        if (status != Z_STREAM_END)
        {
            FKLog("uncompressMemory doInflate error:%d, msg:%s", status, stream.msg);
            break;
        }
        UncompressedBuffer.resize(stream.total_out);
        memcpy(UncompressedBuffer.data(), &UncompressedData[0], stream.total_out);
    } while (false);

    uncompressorTime += UncompressorStartTime.Milliseconds();
    return bOperationSucceeded;
}


bool Compression::CompressFile(const std::string& srcFilePath, const std::string& desFilePath)
{
    bool succeed = false;
    do
    {
        gzFile gFile = gzopen(desFilePath.c_str(), "wb");
        BREAK_IF(gFile == nullptr);
        FILE* srcFp = File::Open(srcFilePath, "rb");
        BREAK_IF(srcFp == nullptr);
        size_t readsize = 0;
		size_t totalReadSize = 0;
        char* buffer = new char[defaultBufferLength];
        do 
        {
            fseek(srcFp, static_cast<long>(totalReadSize), SEEK_SET);
            readsize = fread(buffer, 1, defaultBufferLength, srcFp);
            totalReadSize += readsize;
            gzwrite(gFile, buffer, static_cast<unsigned int>(readsize));
        } while (readsize == static_cast<size_t>(defaultBufferLength));
        delete[] buffer;
        gzflush(gFile, Z_FINISH);
        fclose(srcFp);
        gzclose(gFile);
        succeed = true;
    } while (false);
    return succeed;
}

bool Compression::UncompressFile(const std::string& srcFilePath, const std::string& desFilePath)
{
    bool succeed = false;
    do
    {
        gzFile gFile = gzopen(srcFilePath.c_str(), "rb");
        BREAK_IF(gFile == nullptr);
        FILE* desFp = File::Open(desFilePath, "wb");
        BREAK_IF(desFp == nullptr);
        gzrewind(gFile);
        long readsize = 0;
        char* buffer = new char[defaultBufferLength];
        while (!gzeof(gFile))
        {
            readsize = gzread(gFile, buffer, defaultBufferLength);
            fwrite(buffer, 1, readsize, desFp);
        }
        delete[] buffer;
        fflush(desFp);
        fclose(desFp);
        gzclose(gFile);
        succeed = true;
    } while (false);
    return succeed;
}

NS_FK_END

