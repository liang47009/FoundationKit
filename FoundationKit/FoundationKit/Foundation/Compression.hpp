/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_COMPRESSION_HPP
#define FOUNDATIONKIT_COMPRESSION_HPP

#pragma once
#include <string>
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Base/mutablebuf.hpp"
#include "FoundationKit/Base/types.hpp"

NS_FK_BEGIN

/**
 * Flags controlling [de]compression
 */
enum CompressionFlags
{
    /** No compression */
    COMPRESSION_NONE    = 0x00,
    /** Compress with ZLIB */
    COMPRESSION_ZLIB    = 0x01,
    /** Compress with GZIP */
    COMPRESSION_GZIP    = 0x02,
    // default compression
    COMPRESSION_DEFAULT = COMPRESSION_GZIP
};

/** Default compressor bit window for Zlib */
static constexpr uint32_t DEFAULT_ZLIB_BIT_WINDOW = 15;

struct Compression
{
    /** Time spent compressing data in milliseconds. */
    static long long compressorTime;

    /** Time spent uncompressing data in milliseconds. */
    static long long uncompressorTime;

    static int32_t defaultBufferLength;

    /**
     * Thread-safe abstract compression routine. Compresses memory from uncompressed buffer and writes it to compressed
     * buffer. Updates CompressedSize with size of compressed data. Compression controlled by the passed in flags.
     *
     * @param	Flags						Flags to control what method to use and optionally control memory vs speed
     * @param	CompressedBuffer			Buffer compressed data is going to be written to
     * @param	UncompressedBuffer			Buffer containing uncompressed data
     * @return true if compression succeeds, false if it fails because CompressedBuffer was too small or other reasons
     */
    static bool CompressMemory(CompressionFlags Flags, std::vector<uint8>& CompressedBuffer, const mutablebuf& UncompressedBuffer);

    /**
     * Thread-safe abstract decompression routine. Uncompresses memory from compressed buffer and writes it to uncompressed
     * buffer. UncompressedSize is expected to be the exact size of the data after decompression.
     *
     * @param	Flags						Flags to control what method to use to decompress
     * @param	UncompressedBuffer			Buffer containing uncompressed data
     * @param	CompressedBuffer			Buffer compressed data is going to be read from
     * @return true if compression succeeds, false if it fails because CompressedBuffer was too small or other reasons
     */
    static bool UncompressMemory(CompressionFlags Flags, std::vector<uint8>& UncompressedBuffer, const mutablebuf& CompressedBuffer);

    // compress file to a .gz file
    static bool CompressFile(const std::string& srcFilePath, const std::string& desFilePath);

    // uncompress .gz file
    static bool UncompressFile(const std::string& srcFilePath, const std::string& desFilePath);
};

NS_FK_END
#endif // FOUNDATIONKIT_COMPRESSION_HPP
