#ifndef LOSEMYMIND_COMPRESSION_H
#define LOSEMYMIND_COMPRESSION_H
#pragma once

#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Types.h"

NS_FK_BEGIN

/**
 * Flags controlling [de]compression
 */
enum CompressionFlags
{
    /** No compression																*/
    COMPRESS_NONE = 0x00,
    /** Compress with ZLIB															*/
    COMPRESS_ZLIB = 0x01,
    /** Compress with GZIP															*/
    COMPRESS_GZIP = 0x02,
    /** Prefer compression that compresses smaller (ONLY VALID FOR COMPRESSION)		*/
    COMPRESS_BiasMemory = 0x10,
    /** Prefer compression that compresses faster (ONLY VALID FOR COMPRESSION)		*/
    COMPRESS_BiasSpeed = 0x20,
};


// Define global current platform default to current platform.
#define COMPRESS_DEFAULT			COMPRESS_ZLIB

/** Compression Flag Masks **/
/** mask out compression type flags */
#define COMPRESSION_FLAGS_TYPE_MASK		0x0F

/** mask out compression type */
#define COMPRESSION_FLAGS_OPTIONS_MASK	0xF0


/**
* Chunk size serialization code splits data into. The loading value CANNOT be changed without resaving all
* compressed data which is why they are split into two separate defines.
*/
#define LOADING_COMPRESSION_CHUNK_SIZE_PRE_369  32768
#define LOADING_COMPRESSION_CHUNK_SIZE			131072
#define SAVING_COMPRESSION_CHUNK_SIZE			LOADING_COMPRESSION_CHUNK_SIZE

struct Compression
{
    /** Maximum allowed size of an uncompressed buffer passed to CompressMemory or UncompressMemory. */
    //const static uint32 maxUncompressedSize = 256 * 1024;

    /** Time spent compressing data in seconds. */
    static double compressorTime;
    /** Number of bytes before compression.		*/
    static uint64 compressorSrcBytes;
    /** Number of bytes after compression.		*/
    static uint64 compressorDstBytes;


    /** Time spent uncompressing data in seconds. */
    static double uncompressorTime;
    /** Number of bytes before uncompression.		*/
    static uint64 uncompressorSrcBytes;
    /** Number of bytes after uncompression.		*/
    static uint64 uncompressorDstBytes;

    /**
    * Thread-safe abstract compression routine to query memory requirements for a compression operation.
    *
    * @param	Flags						Flags to control what method to use and optionally control memory vs speed
    * @param	UncompressedSize			Size of uncompressed data in bytes
    * @return The maximum possible bytes needed for compression of data buffer of size UncompressedSize
    */
    static int32 compressMemoryBound(CompressionFlags Flags, int32 UncompressedSize);

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
    static bool compressMemory(CompressionFlags Flags, void* CompressedBuffer, int32& CompressedSize, const void* UncompressedBuffer, int32 UncompressedSize);

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
    static bool uncompressMemory(CompressionFlags Flags, void* UncompressedBuffer, int32 UncompressedSize, const void* CompressedBuffer, int32 CompressedSize);
};

NS_FK_END

#endif // LOSEMYMIND_COMPRESSION_H



