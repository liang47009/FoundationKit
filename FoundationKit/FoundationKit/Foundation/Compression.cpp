#include <mutex>
#include <vector>
#include <stdio.h>
#include "Compression.h"
#include "zlib.h"
#include "FoundationKit/Base/TimeEx.h"
#include "FoundationKit/Foundation/Logger.h"

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

static int transformLevel(CompressionLevel level)
{
    int retLevel = Z_DEFAULT_COMPRESSION;
    switch (level) {
    case COMPRESSION_LEVEL_FASTEST:
        retLevel = 1;
        break;
    case COMPRESSION_LEVEL_DEFAULT:
        retLevel = Z_DEFAULT_COMPRESSION;
        break;
    case COMPRESSION_LEVEL_BEST:
        retLevel = 9;
        break;
    }
    return retLevel;
}

static int doInflate(z_stream* stream, std::vector<uint8>& head, size_t blockSize) {
    if (stream->avail_out == 0) 
    {
        int index = head.size();
        head.resize(head.size() + blockSize);
        stream->next_out = &head[index];
        stream->avail_out = blockSize;
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
        LOG_ERROR("doInflate inflate error:%d, msg:%s", status, stream->msg);
    default:
        LOG_ERROR("uncompressMemory inflate error:%d, msg:%s", status, stream->msg);
    }
    return status;
}

struct scope_exit
{
    typedef std::function<void()> _Fun;
    scope_exit(const _Fun& fun)
        :_fun(fun)
    {

    }

    ~scope_exit()
    {
        if (_fun)
        {
            _fun();
        }
    }
    _Fun   _fun;
};


/** Time spent compressing data in seconds. */
double Compression::compressorTime = 0;

/** Time spent uncompressing data in seconds. */
double Compression::uncompressorTime = 0;


bool Compression::compressMemory(CompressionFlags Flags, mutable_data& CompressedBuffer, const mutable_data& UncompressedBuffer, CompressionLevel level/* = COMPRESSION_LEVEL_FASTEST*/)
{
    static std::mutex zlibScopeMutex;
    std::lock_guard<std::mutex> scopeLock(zlibScopeMutex);
    // Keep track of time spent uncompressing memory.
    Time CompressorStartTime = Time::now();
    bool bCompressSucceeded = false;
    do 
    {
        z_stream stream;
        stream.zalloc = &zipAlloc;
        stream.zfree = &zipFree;
        stream.opaque = nullptr;

        // Using deflateInit2() to support gzip.  "The windowBits parameter is the
        // base two logarithm of the maximum window size (...) The default value is
        // 15 (...) Add 16 to windowBits to write a simple gzip header and trailer
        // around the compressed data instead of a zlib wrapper. The gzip header
        // will have no file name, no extra data, no comment, no modification time
        // (set to zero), no header crc, and the operating system will be set to 255
        // (unknown)."
        int windowBits = 15 + (Flags == COMPRESSION_GZIP ? 16 : 0);

        // All other parameters (method, memLevel, strategy) get default values from
        // the zlib manual.
        int status = deflateInit2(&stream,
            transformLevel(level),
            Z_DEFLATED,
            windowBits,
            MAX_MEM_LEVEL,
            Z_DEFAULT_STRATEGY);

        if (status != Z_OK)
        {
            LOG_ERROR("compressMemory deflateInit2 error:%d, msg:%s", status, stream.msg);
            break;
        }

        scope_exit se([&]()
        {
            status = deflateEnd(&stream);
            if (status != Z_OK)
            {
                LOG_ERROR("compressMemory deflateEnd error:%d, msg:%s", status, stream.msg);
            }
        });

        uLong uncompressedLength = UncompressedBuffer.size();

        stream.next_in  = static_cast<uint8*>(UncompressedBuffer.data());
        stream.avail_in = uncompressedLength;
        mutable_data tempData;
        tempData.reserve(uncompressedLength);
        stream.next_out = static_cast<uint8*>(tempData.data());;
        stream.avail_out = uncompressedLength;
        while ((status = deflate(&stream, Z_FINISH)) == Z_OK);
        if (status != Z_STREAM_END)
        {
            LOG_ERROR("compressMemory deflate error:%d, msg:%s", status, stream.msg);
            break;
        }
        CompressedBuffer.reserve(stream.total_out);
        memcpy(CompressedBuffer.data(), tempData.data(), stream.total_out);
        bCompressSucceeded = true;
    } while (false);
    // Keep track of compression time and stats.
    compressorTime += (Time::now() - CompressorStartTime).seconds();
    return bCompressSucceeded;
}

bool Compression::uncompressMemory(CompressionFlags Flags, mutable_data& UncompressedBuffer, const mutable_data& CompressedBuffer)
{
    static std::mutex zlibScopeMutex;
    std::lock_guard<std::mutex> scopeLock(zlibScopeMutex);
    // Keep track of time spent uncompressing memory.
    Time UncompressorStartTime = Time::now();

    bool bUncompressSucceeded = false;
    do 
    {
        z_stream stream;
        stream.zalloc = &zipAlloc;
        stream.zfree = &zipFree;
        stream.opaque = nullptr;
        // "The windowBits parameter is the base two logarithm of the maximum window
        // size (...) The default value is 15 (...) add 16 to decode only the gzip
        // format (the zlib format will return a Z_DATA_ERROR)."
        int windowBits = 15 + (Flags == COMPRESSION_GZIP  ? 16 : 0);

        int status = inflateInit2(&stream, windowBits);
        if (status != Z_OK)
        {
            LOG_ERROR("Compression inflateInit2 error:%d, msg:%s", status, stream.msg);
            break;
        }

        scope_exit se([&]()
        {
            status = inflateEnd(&stream);
            if (status != Z_OK)
            {
                LOG_ERROR("uncompressMemory inflateEnd error:%d, msg:%s", status, stream.msg);
            }
        });

        //constexpr uint32_t defaultBufferLength = uint32_t(4) << 20;     // 4MiB
        constexpr uint32_t defaultBufferLength = uint32_t(1) << 20;       // 1MiB
        stream.next_in = static_cast<uint8*>(CompressedBuffer.data());
        stream.avail_in = CompressedBuffer.size();
        stream.next_out = nullptr;
        stream.avail_out = 0;
        stream.total_in = 0;
        stream.total_out = 0;

        std::vector<uint8>  UncompressedData;
        do 
        {
            status = doInflate(&stream, UncompressedData, defaultBufferLength);
        } while (status == Z_OK);

        UncompressedBuffer.reserve(stream.total_out);
        memcpy(UncompressedBuffer.data(), &UncompressedData[0], stream.total_out);
        bUncompressSucceeded = true;
    } while (false);

    uncompressorTime += (Time::now() - UncompressorStartTime).seconds();
    return bUncompressSucceeded;
}


bool Compression::compressFile(const std::string& srcFilePath, const std::string& desFilePath)
{
    LOG_ASSERT(false, "***** Not Implementation.");
    //gzFile gFile = gzopen(desFilePath.c_str(), "wb");
    //FILE* srcFp = fopen(srcFilePath.c_str(), "rb");
    //size_t size = 0;

    //fseek(srcFp, 0, SEEK_END);
    //size = ftell(srcFp);
    //fseek(srcFp, 0, SEEK_SET);

    //const uint32_t defaultBufferLength = uint32_t(1) << 20;       // 1MiB
    //size_t leftSize = size;
    //size_t readsize = 0;

    //char* buffer = new char[defaultBufferLength];
    //while (leftSize > 0)
    //{
    //    if (leftSize > defaultBufferLength)
    //    {
    //        readsize = fread(buffer, 1, defaultBufferLength, srcFp);
    //    }
    //    else
    //    {
    //        readsize = fread(buffer, 1, leftSize, srcFp);
    //    }
    //    leftSize -= readsize;
    //    fseek(srcFp, size - leftSize, SEEK_SET);
    //    gzwrite(gFile, buffer, readsize);
    //}
    //delete[] buffer;
    //fclose(srcFp);
    //gzclose(gFile);
    return false;
}

bool Compression::uncompressFile(const std::string& srcFilePath, const std::string& desFilePath)
{
    LOG_ASSERT(false, "***** Not Implementation.");
    return false;
}

NS_FK_END














