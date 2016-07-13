#ifndef LOSEMYMIND_COMPRESSION_H
#define LOSEMYMIND_COMPRESSION_H
#pragma once

#include "FoundationKit/GenericPlatformMacros.h"
#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Base/mutable_data.hpp"

NS_FK_BEGIN

/**
 * Flags controlling [de]compression
 */
enum CompressionFlags
{
    /** No compression																*/
    COMPRESSION_NONE = 0x00,
    /** Compress with ZLIB															*/
    COMPRESSION_ZLIB = 0x01,
    /** Compress with GZIP															*/
    COMPRESSION_GZIP = 0x02,
    // default compression
    COMPRESSION_DEFAULT = COMPRESSION_GZIP
};


enum CompressionLevel
{
    COMPRESSION_LEVEL_FASTEST = -1,
    COMPRESSION_LEVEL_DEFAULT = -2,
    COMPRESSION_LEVEL_BEST = -3
};

struct Compression
{
    /** Time spent compressing data in seconds. */
    static double compressorTime;

    /** Time spent uncompressing data in seconds. */
    static double uncompressorTime;

    /**
     * Thread-safe abstract compression routine. Compresses memory from uncompressed buffer and writes it to compressed
     * buffer. Updates CompressedSize with size of compressed data. Compression controlled by the passed in flags.
     *
     * @param	Flags						Flags to control what method to use and optionally control memory vs speed
     * @param	CompressedBuffer			Buffer compressed data is going to be written to
     * @param	UncompressedBuffer			Buffer containing uncompressed data
     * @return true if compression succeeds, false if it fails because CompressedBuffer was too small or other reasons
     */
    static bool compressMemory(CompressionFlags Flags, mutable_data& CompressedBuffer, const mutable_data& UncompressedBuffer, CompressionLevel level = COMPRESSION_LEVEL_FASTEST);

    /**
     * Thread-safe abstract decompression routine. Uncompresses memory from compressed buffer and writes it to uncompressed
     * buffer. UncompressedSize is expected to be the exact size of the data after decompression.
     *
     * @param	Flags						Flags to control what method to use to decompress
     * @param	UncompressedBuffer			Buffer containing uncompressed data
     * @param	CompressedBuffer			Buffer compressed data is going to be read from
     * @return true if compression succeeds, false if it fails because CompressedBuffer was too small or other reasons
     */
    static bool uncompressMemory(CompressionFlags Flags, mutable_data& UncompressedBuffer, const mutable_data& CompressedBuffer);
};

NS_FK_END

#endif // LOSEMYMIND_COMPRESSION_H



