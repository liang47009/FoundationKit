#include <mutex>
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

        uLong uncompressedLength = UncompressedBuffer.size();
        uLong compressedLength = deflateBound(&stream, uncompressedLength);
        // Max 64MiB in one go
        //constexpr uint32_t maxSingleStepLength = uint32_t(64) << 20;    // 64MiB
        //constexpr uint32_t defaultBufferLength = uint32_t(4) << 20;     // 4MiB
        stream.next_in  = static_cast<uint8*>(UncompressedBuffer.data());
        stream.avail_in = uncompressedLength;
        CompressedBuffer.reserve(compressedLength);
        stream.next_out = static_cast<uint8*>(CompressedBuffer.data());;
        stream.avail_out = compressedLength;

        while ((status = deflate(&stream, Z_FINISH)) == Z_OK);
        if (status == Z_STREAM_END)
        {
            bCompressSucceeded = true;
        }

        status = deflateEnd(&stream);
        if (status != Z_OK)
        {
            LOG_ERROR("compressMemory deflateEnd error:%d, msg:%s", status, stream.msg);
            break;
        }
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

        constexpr uint32_t defaultBufferLength = uint32_t(4) << 20;     // 4MiB

        stream.next_in = static_cast<uint8*>(CompressedBuffer.data());
        stream.avail_in = CompressedBuffer.size();
        if (UncompressedBuffer.size() == 0)
        {
            UncompressedBuffer.reserve(defaultBufferLength);
        }
        stream.next_out = static_cast<uint8*>(UncompressedBuffer.data());;
        stream.avail_out = UncompressedBuffer.size();

        size_t uncompressedSize = UncompressedBuffer.size();
        // Uncompress data.
        status = inflate(&stream, Z_FINISH);

        if (status == Z_STREAM_END)
        {
            uncompressedSize = stream.total_out;
        }

        status = inflateEnd(&stream);
        if (status != Z_OK)
        {
            LOG_ERROR("uncompressMemory inflateEnd error:%d, msg:%s", status, stream.msg);
            break;
        }
        // These warnings will be compiled out in shipping.
        LOG_CONDITION(status == Z_MEM_ERROR, "uncompressMemoryZLIB failed: Error: Z_MEM_ERROR, not enough memory!");
        LOG_CONDITION(status == Z_BUF_ERROR, "uncompressMemoryZLIB failed: Error: Z_BUF_ERROR, not enough room in the output buffer!");
        LOG_CONDITION(status == Z_DATA_ERROR, "uncompressMemoryZLIB failed: Error: Z_DATA_ERROR, input data was corrupted or incomplete!");
        // Sanity check to make sure we uncompressed as much data as we expected to.
        LOG_ASSERT(UncompressedBuffer.size() == uncompressedSize, " uncompressMemory - Failed to uncompress memory (%d/%d)", UncompressedBuffer.size(), uncompressedSize);
        bUncompressSucceeded = true;
    } while (false);

    uncompressorTime += (Time::now() - UncompressorStartTime).seconds();
    return bUncompressSucceeded;
}

NS_FK_END

