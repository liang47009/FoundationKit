#include <mutex>
#include "Compression.h"
#include "zlib.h"
#include "FoundationKit/Base/Time.h"
NS_FK_BEGIN

/**
 * Thread-safe abstract compression routine. Compresses memory from uncompressed buffer and writes it to compressed
 * buffer. Updates CompressedSize with size of compressed data.
 *
 * @param	CompressedBuffer			Buffer compressed data is going to be written to
 * @param	CompressedSize	[in/out]	Size of CompressedBuffer, at exit will be size of compressed data
 * @param	UncompressedBuffer			Buffer containing uncompressed data
 * @param	UncompressedSize			Size of uncompressed data in bytes
 * @return true if compression succeeds, false if it fails because CompressedBuffer was too small or other reasons
 */
static bool compressMemoryZLIB(void* CompressedBuffer, int32& CompressedSize, const void* UncompressedBuffer, int32 UncompressedSize)
{
    static std::mutex zlibScopeMutex;
    std::lock_guard<std::mutex> scopeLock(zlibScopeMutex);

    // Zlib wants to use unsigned long.
    unsigned long ZCompressedSize = CompressedSize;
    unsigned long ZUncompressedSize = UncompressedSize;
    // Compress data
    bool bOperationSucceeded = compress((uint8*)CompressedBuffer, &ZCompressedSize, (const uint8*)UncompressedBuffer, ZUncompressedSize) == Z_OK ? true : false;
    // Propagate compressed size from intermediate variable back into out variable.
    CompressedSize = static_cast<int32>(ZCompressedSize);
    return bOperationSucceeded;
}

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

static bool compressMemoryGZIP(void* CompressedBuffer, int32& CompressedSize, const void* UncompressedBuffer, int32 UncompressedSize)
{
    static std::mutex gzipScopeMutex;
    std::lock_guard<std::mutex> scopeLock(gzipScopeMutex);

    z_stream gzipstream;
    gzipstream.zalloc = &zipAlloc;
    gzipstream.zfree = &zipFree;
    gzipstream.opaque = Z_NULL;

    // Setup input buffer
    gzipstream.next_in = (uint8*)UncompressedBuffer;
    gzipstream.avail_in = UncompressedSize;

    // Init deflate settings to use GZIP
    int windowsBits = 15;
    int GZIP_ENCODING = 16;
    deflateInit2(
        &gzipstream,
        Z_DEFAULT_COMPRESSION,
        Z_DEFLATED,
        windowsBits | GZIP_ENCODING,
        MAX_MEM_LEVEL,
        Z_DEFAULT_STRATEGY);

    // Setup output buffer
    //const unsigned long GzipHeaderLength = 12;
    // This is how much memory we may need, however the consumer is allocating memory for us without knowing the required length.
    //unsigned long CompressedMaxSize = deflateBound(&gzipstream, gzipstream.avail_in) + GzipHeaderLength;
    gzipstream.next_out = (uint8*)CompressedBuffer;
    gzipstream.avail_out = UncompressedSize;

    int status = 0;
    bool bOperationSucceeded = false;
    while ((status = deflate(&gzipstream, Z_FINISH)) == Z_OK);
    if (status == Z_STREAM_END)
    {
        bOperationSucceeded = true;
        deflateEnd(&gzipstream);
    }

    // Propagate compressed size from intermediate variable back into out variable.
    CompressedSize = static_cast<int32>(gzipstream.total_out);
    return bOperationSucceeded;
}

/**
* Thread-safe abstract compression routine. Compresses memory from uncompressed buffer and writes it to compressed
* buffer. Updates CompressedSize with size of compressed data.
*
* @param	UncompressedBuffer			Buffer containing uncompressed data
* @param	UncompressedSize			Size of uncompressed data in bytes
* @param	CompressedBuffer			Buffer compressed data is going to be read from
* @param	CompressedSize				Size of CompressedBuffer data in bytes
* @return true if compression succeeds, false if it fails because CompressedBuffer was too small or other reasons
*/
bool uncompressMemoryZLIB(void* UncompressedBuffer, int32 UncompressedSize, const void* CompressedBuffer, int32 CompressedSize)
{
    static std::mutex zlibScopeMutex;
    std::lock_guard<std::mutex> scopeLock(zlibScopeMutex);

    // Zlib wants to use unsigned long.
    unsigned int ZCompressedSize = CompressedSize;
    unsigned int ZUncompressedSize = UncompressedSize;

    z_stream stream;
    stream.zalloc = &zipAlloc;
    stream.zfree = &zipFree;
    stream.opaque = Z_NULL;
    stream.next_in = (uint8*)CompressedBuffer;
    stream.avail_in = ZCompressedSize;
    stream.next_out = (uint8*)UncompressedBuffer;
    stream.avail_out = ZUncompressedSize;

    int32 Result = inflateInit(&stream);

    if (Result != Z_OK)
        return false;

    // Uncompress data.
    Result = inflate(&stream, Z_FINISH);

    if (Result == Z_STREAM_END)
    {
        ZUncompressedSize = static_cast<int32>(stream.total_out);
    }

    Result = inflateEnd(&stream);

    // These warnings will be compiled out in shipping.
    LOG_CONDITION(Result == Z_MEM_ERROR,  "uncompressMemoryZLIB failed: Error: Z_MEM_ERROR, not enough memory!");
    LOG_CONDITION(Result == Z_BUF_ERROR,  "uncompressMemoryZLIB failed: Error: Z_BUF_ERROR, not enough room in the output buffer!");
    LOG_CONDITION(Result == Z_DATA_ERROR, "uncompressMemoryZLIB failed: Error: Z_DATA_ERROR, input data was corrupted or incomplete!");

    const bool bOperationSucceeded = (Result == Z_OK);

    // Sanity check to make sure we uncompressed as much data as we expected to.
    LOG_ASSERT(static_cast<unsigned long>(UncompressedSize) == ZUncompressedSize, " uncompressMemoryZLIB - Failed to uncompress memory (%d/%d)", UncompressedSize, ZUncompressedSize);
    return bOperationSucceeded;
}

/** Time spent compressing data in seconds. */
double Compression::compressorTime = 0;
/** Number of bytes before compression.		*/
uint64 Compression::compressorSrcBytes = 0;
/** Nubmer of bytes after compression.		*/
uint64 Compression::compressorDstBytes = 0;

/** Time spent uncompressing data in seconds. */
double Compression::uncompressorTime = 0;
/** Number of bytes before uncompression.		*/
uint64 Compression::uncompressorSrcBytes = 0;
/** Number of bytes after uncompression.		*/
uint64 Compression::uncompressorDstBytes = 0;

/**
* Thread-safe abstract compression routine to query memory requirements for a compression operation.
*
* @param	Flags						Flags to control what method to use and optionally control memory vs speed
* @param	UncompressedSize			Size of uncompressed data in bytes
* @return The maximum possible bytes needed for compression of data buffer of size UncompressedSize
*/
int32 Compression::compressMemoryBound(CompressionFlags Flags, int32 UncompressedSize)
{
    int32 CompressionBound = UncompressedSize;
    // make sure a valid compression scheme was provided
    LOG_ASSERT(Flags & COMPRESS_ZLIB, "Compression::compressMemoryBound just support zlib.");

    switch (Flags & COMPRESSION_FLAGS_TYPE_MASK)
    {
    case COMPRESS_ZLIB:
        CompressionBound = static_cast<int32>(compressBound(UncompressedSize));
        break;
    default:
        break;
    }
    return CompressionBound;
}

/**
* Thread-safe abstract compression routine. Compresses memory from uncompressed buffer and writes it to compressed
* buffer. Updates CompressedSize with size of compressed data. Compression controlled by the passed in flags.
*
* @param	Flags						Flags to control what method to use and optionally control memory vs speed
* @param	CompressedBuffer			Buffer compressed data is going to be written to
* @param	CompressedSize	[in/out]	Size of CompressedBuffer, at exit will be size of compressed data
* @param	UncompressedBuffer			Buffer containing uncompressed data
* @param	UncompressedSize			Size of uncompressed data in bytes
* @return true if compression succeeds, false if it fails because CompressedBuffer was too small or other reasons
*/
bool Compression::compressMemory(CompressionFlags Flags, void* CompressedBuffer, int32& CompressedSize, const void* UncompressedBuffer, int32 UncompressedSize)
{
    Time CompressorStartTime = Time::now();

    // make sure a valid compression scheme was provided
    LOG_ASSERT(Flags & COMPRESS_ZLIB || Flags & COMPRESS_GZIP
        , "Compression::compressMemory just support COMPRESS_ZLIB and COMPRESS_GZIP, Please check the Flags param. ");

    bool bCompressSucceeded = false;

    switch (Flags & COMPRESSION_FLAGS_TYPE_MASK)
    {
    case COMPRESS_ZLIB:
        bCompressSucceeded = compressMemoryZLIB(CompressedBuffer, CompressedSize, UncompressedBuffer, UncompressedSize);
        break;
    case COMPRESS_GZIP:
        bCompressSucceeded = compressMemoryGZIP(CompressedBuffer, CompressedSize, UncompressedBuffer, UncompressedSize);
        break;
    default:
        LOG_CONDITION(true, "Compression::compressMemory - This compression type not supported");
        bCompressSucceeded = false;
    }

    // Keep track of compression time and stats.
    compressorTime += (Time::now() - CompressorStartTime).getSeconds();
    if (bCompressSucceeded)
    {
        compressorSrcBytes += UncompressedSize;
        compressorDstBytes += CompressedSize;
    }

    return bCompressSucceeded;
}

/**
* Thread-safe abstract decompression routine. Uncompresses memory from compressed buffer and writes it to uncompressed
* buffer. UncompressedSize is expected to be the exact size of the data after decompression.
*
* @param	Flags						Flags to control what method to use to decompress
* @param	UncompressedBuffer			Buffer containing uncompressed data
* @param	UncompressedSize			Size of uncompressed data in bytes
* @param	CompressedBuffer			Buffer compressed data is going to be read from
* @param	CompressedSize				Size of CompressedBuffer data in bytes
* @return true if compression succeeds, false if it fails because CompressedBuffer was too small or other reasons
*/
bool Compression::uncompressMemory(CompressionFlags Flags, void* UncompressedBuffer, int32 UncompressedSize, const void* CompressedBuffer, int32 CompressedSize)
{
    // Keep track of time spent uncompressing memory.
    Time UncompressorStartTime = Time::now();

    // make sure a valid compression scheme was provided
    LOG_ASSERT(Flags & COMPRESS_ZLIB, "Compression::uncompressMemory just support COMPRESS_ZLIB");

    bool bUncompressSucceeded = false;

    switch (Flags & COMPRESSION_FLAGS_TYPE_MASK)
    {
    case COMPRESS_ZLIB:
        bUncompressSucceeded = uncompressMemoryZLIB(UncompressedBuffer, UncompressedSize, CompressedBuffer, CompressedSize);
        if (!bUncompressSucceeded)
        {
            // Always log an error
            LOG_CONDITION(true,"Compression::uncompressMemory - Failed to uncompress memory (%d/%d), this may indicate the asset is corrupt!", CompressedSize, UncompressedSize);
        }
        break;
    default:
        LOG_CONDITION(true,"Compression::uncompressMemory - This compression type not supported");
        bUncompressSucceeded = false;
    }
    uncompressorTime += (Time::now() - UncompressorStartTime).getSeconds();
    if (bUncompressSucceeded)
    {
        uncompressorSrcBytes += CompressedSize;
        uncompressorDstBytes += UncompressedSize;
    }
    return bUncompressSucceeded;
}


NS_FK_END

