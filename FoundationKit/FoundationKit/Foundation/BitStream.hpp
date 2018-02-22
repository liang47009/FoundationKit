

// SEE https://github.com/facebookarchive/RakNet
// SEE https://www.cnblogs.com/ishowfun/articles/4026819.html
#ifndef FOUNDATIONKIT_BITSTREAM_HPP
#define FOUNDATIONKIT_BITSTREAM_HPP

#include <math.h>
#include <float.h>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"

// Threshold at which to do a malloc / free rather than pushing data onto a fixed stack for the BitStream class
// Arbitrary size, just picking something likely to be larger than most packets
#ifndef BITSTREAM_STACK_ALLOCATION_SIZE
#define BITSTREAM_STACK_ALLOCATION_SIZE 256
#endif

/// Given a number of bits, return how many bytes are needed to represent that.
#define BITS_TO_BYTES(x) (((x)+7)>>3)
#define BYTES_TO_BITS(x) ((x)<<3)

#if !PLATFORM_WINDOWS
#define _copysign copysign
#endif

NS_FK_BEGIN

// This class allows you to write and read native types as a string of bits.  
// BitStream is used extensively throughout RakNet and is designed to be used by users as well.
class BitStream
{

public:

    // Default Constructor
    BitStream();

    /**
     * Create the BitStream, with some number of bytes to immediately allocate.
     * @details There is no benefit to calling this, unless you know exactly how 
     *          many bytes you need and it is greater than BITSTREAM_STACK_ALLOCATION_SIZE.
     *          In that case all it does is save you one or more realloc calls.
     * @param[in] InitialBytesToAllocate The number of bytes to pre-allocate.
     */
    BitStream(const uint32 InitialBytesToAllocate);

    /** 
     * Initialize the BitStream, immediately setting the data it contains to a predefined pointer.
     * @details Set bCopyData to true if you want to make an internal copy of the data you are passing. 
     *          Set it to false to just save a pointer to the data. You shouldn't call Write functions 
     *          with bCopyData as false, as this will write to unallocated memory 99% of the time you 
     *          will use this function to cast Packet::data to a bitstream for reading, in which case 
     *          you should write something as follows:
     * @param[in] InData An array of bytes.
     * @param[in] DataSize Size of the  _data.
     * @param[in] bCopyData true or false to make a copy of  _data or not.
     */
    BitStream(uint8* InData, const uint32 DataSize, bool bCopyData);

    // Destructor
    ~BitStream();

    // Resets the BitStream for reuse.
    void Reset(void);

    /**
     * Write any integral type to a BitStream.  
     * @details Undefine BITSTREAM_NATIVE_ENDIAN if you need endian swapping.
     * @param[in] InValue The value to write
     */
    template <class _Ty>
    void Write(const _Ty &InValue);

    /// \brief Write the dereferenced pointer to any integral type to a bitstream.  
    /// \details Undefine BITSTREAM_NATIVE_ENDIAN if you need endian swapping.
    /// \param[in] inTemplateVar The value to write
    template <class _Ty>
    void WritePtr(_Ty *inTemplateVar);

    /// \brief Write any integral type to a bitstream.  
    /// \details Undefine BITSTREAM_NATIVE_ENDIAN if you need endian swapping.
    /// If you are not using BITSTREAM_NATIVE_ENDIAN the opposite is true for types larger than 1 byte
    /// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
    /// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
    /// \param[in] inTemplateVar The value to write
    template <class _Ty>
    void WriteCompressed(const _Ty &inTemplateVar);


    /// \brief Read any integral type from a bitstream.  
    /// \details Define BITSTREAM_NATIVE_ENDIAN if you need endian swapping.
    /// \param[in] outTemplateVar The value to read
    /// \return true on success, false on failure.
    template <class _Ty>
    bool Read(_Ty &outTemplateVar);

    /// \brief Read any integral type from a bitstream.  
    /// \details Undefine BITSTREAM_NATIVE_ENDIAN if you need endian swapping.
    /// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
    /// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
    /// If you are not using BITSTREAM_NATIVE_ENDIAN the opposite is true for types larger than 1 byte
    /// \param[in] outTemplateVar The value to read
    /// \return true on success, false on failure.
    template <class _Ty>
    bool ReadCompressed(_Ty &outTemplateVar);

    /// \brief Read one bitstream to another.
    /// \param[in] numberOfBits bits to read
    /// \param bitStream the bitstream to read into from
    /// \return true on success, false on failure.
    bool Read(BitStream *bitStream, uint32_t numberOfBits);
    bool Read(BitStream *bitStream);
    bool Read(BitStream &bitStream, uint32_t numberOfBits);
    bool Read(BitStream &bitStream);

    /// \brief Write an array or casted stream or raw data.  This does NOT do endian swapping.
    /// \param[in] inputByteArray a byte buffer
    /// \param[in] numberOfBytes the size of \a input in bytes
    void Write(const char* inputByteArray, const uint32 numberOfBytes);

    /// \brief Write one bitstream to another.
    /// \param[in] numberOfBits bits to write
    /// \param bitStream the bitstream to copy from
    void Write(BitStream *bitStream, uint32_t numberOfBits);
    void Write(BitStream *bitStream);
    void Write(BitStream &bitStream, uint32_t numberOfBits);
    void Write(BitStream &bitStream);\

    /// \brief Write a float into 2 bytes, spanning the range between \a floatMin and \a floatMax
    /// \param[in] x The float to write
    /// \param[in] floatMin Predetermined minimum value of f
    /// \param[in] floatMax Predetermined maximum value of f
    void WriteFloat16(float x, float floatMin, float floatMax);

    /// Write one type serialized as another (smaller) type, to save bandwidth
    /// serializationType should be uint8_t, uint16_t, uint24_t, or uint32_t
    /// Example: int num=53; WriteCasted<uint8_t>(num); would use 1 byte to write what would otherwise be an integer (4 or 8 bytes)
    /// \param[in] value The value to write
    template <class serializationType, class sourceType >
    void WriteCasted(const sourceType &value);

    /// \brief Write a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  
    /// \details Will further compress y or z axis aligned vectors.
    /// Accurate to 1/32767.5.
    /// \param[in] x x
    /// \param[in] y y
    /// \param[in] z z
    template <class _Ty> // _Ty for this function must be a float or double
    void WriteNormVector(_Ty x, _Ty y, _Ty z);

    /// \brief Write a vector, using 10 bytes instead of 12.
    /// \details Loses accuracy to about 3/10ths and only saves 2 bytes, 
    /// so only use if accuracy is not important.
    /// \param[in] x x
    /// \param[in] y y
    /// \param[in] z z
    template <class _Ty> // _Ty for this function must be a float or double
    void WriteVector(_Ty x, _Ty y, _Ty z);

    /// \brief Write a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.  Slightly lossy.
    /// \param[in] w w
    /// \param[in] x x
    /// \param[in] y y
    /// \param[in] z z
    template <class _Ty> // _Ty for this function must be a float or double
    void WriteNormQuat(_Ty w, _Ty x, _Ty y, _Ty z);

    /// \brief Write an orthogonal matrix by creating a quaternion, and writing 3 components of the quaternion in 2 bytes each.
    /// \details Use 6 bytes instead of 36
    /// Lossy, although the result is renormalized
    template <class _Ty> // _Ty for this function must be a float or double
    void WriteOrthMatrix(
        _Ty m00, _Ty m01, _Ty m02,
        _Ty m10, _Ty m11, _Ty m12,
        _Ty m20, _Ty m21, _Ty m22);

    /// \brief Read an array or casted stream of byte.
    /// \details The array is raw data. There is no automatic endian conversion with this function
    /// \param[in] output The result byte array. It should be larger than @em numberOfBytes.
    /// \param[in] numberOfBytes The number of byte to read
    /// \return true on success false if there is some missing bytes.
    bool Read(char* output, const uint32 numberOfBytes);

    /// \brief Read a float into 2 bytes, spanning the range between \a floatMin and \a floatMax
    /// \param[in] outFloat The float to read
    /// \param[in] floatMin Predetermined minimum value of f
    /// \param[in] floatMax Predetermined maximum value of f
    bool ReadFloat16(float &outFloat, float floatMin, float floatMax);

    /// Read one type serialized to another (smaller) type, to save bandwidth
    /// serializationType should be uint8_t, uint16_t, uint24_t, or uint32_t
    /// Example: int num; ReadCasted<uint8_t>(num); would read 1 bytefrom the stream, and put the value in an integer
    /// \param[in] value The value to write
    template <class serializationType, class sourceType >
    bool ReadCasted(sourceType &value);

    /// \brief Read a normalized 3D vector, using (at most) 4 bytes + 3 bits instead of 12-24 bytes.  
    /// \details Will further compress y or z axis aligned vectors.
    /// Accurate to 1/32767.5.
    /// \param[in] x x
    /// \param[in] y y
    /// \param[in] z z
    /// \return true on success, false on failure.
    template <class _Ty> // _Ty for this function must be a float or double
    bool ReadNormVector(_Ty &x, _Ty &y, _Ty &z);

    /// \brief Read 3 floats or doubles, using 10 bytes, where those float or doubles comprise a vector.
    /// \details Loses accuracy to about 3/10ths and only saves 2 bytes, 
    /// so only use if accuracy is not important.
    /// \param[in] x x
    /// \param[in] y y
    /// \param[in] z z
    /// \return true on success, false on failure.
    template <class _Ty> // _Ty for this function must be a float or double
    bool ReadVector(_Ty &x, _Ty &y, _Ty &z);

    /// \brief Read a normalized quaternion in 6 bytes + 4 bits instead of 16 bytes.
    /// \param[in] w w
    /// \param[in] x x
    /// \param[in] y y
    /// \param[in] z z
    /// \return true on success, false on failure.
    template <class _Ty> // _Ty for this function must be a float or double
    bool ReadNormQuat(_Ty &w, _Ty &x, _Ty &y, _Ty &z);

    /// \brief Read an orthogonal matrix from a quaternion, reading 3 components of the quaternion in 2 bytes each and extrapolatig the 4th.
    /// \details Use 6 bytes instead of 36
    /// Lossy, although the result is renormalized
    /// \return true on success, false on failure.
    template <class _Ty> // _Ty for this function must be a float or double
    bool ReadOrthMatrix(
        _Ty &m00, _Ty &m01, _Ty &m02,
        _Ty &m10, _Ty &m11, _Ty &m12,
        _Ty &m20, _Ty &m21, _Ty &m22);

    /// \brief Sets the read pointer back to the beginning of your data.
    void ResetReadPointer(void);

    /// \brief Sets the write pointer back to the beginning of your data.
    void ResetWritePointer(void);

    /// \brief This is good to call when you are done with the stream to make
    /// sure you didn't leave any data left over void
    void AssertStreamEmpty(void);

    /// \brief RAKNET_DEBUG_PRINTF the bits in the stream.  Great for debugging.
    void PrintBits(char *out) const;
    void PrintBits(void) const;
    void PrintHex(char *out) const;
    void PrintHex(void) const;

    /// \brief Ignore data we don't intend to read
    /// \param[in] numberOfBits The number of bits to ignore
    void IgnoreBits(const uint32_t numberOfBits);

    /// \brief Ignore data we don't intend to read
    /// \param[in]numberOfBytes The number of bytes to ignore
    void IgnoreBytes(const uint32 numberOfBytes);

    /// \brief Move the write pointer to a position on the array.
    /// \param[in] offset the offset from the start of the array.
    /// \details Dangerous if you don't know what you are doing!
    /// For efficiency reasons you can only write mid-stream if your data is byte aligned.
    void SetWriteOffset(const uint32_t offset);

    /// \brief Returns the length in bits of the stream
    inline uint32_t GetNumberOfBitsUsed(void) const { return GetWriteOffset(); }
    inline uint32_t GetWriteOffset(void) const { return numberOfBitsUsed; }

    /// \brief Returns the length in bytes of the stream
    inline uint32_t GetNumberOfBytesUsed(void) const { return BITS_TO_BYTES(numberOfBitsUsed); }

    /// \brief Returns the number of bits into the stream that we have read
    inline uint32_t GetReadOffset(void) const { return readOffset; }

    /// \brief Sets the read bit index
    void SetReadOffset(const uint32_t newReadOffset) { readOffset = newReadOffset; }

    /// \brief Returns the number of bits left in the stream that haven't been read
    inline uint32_t GetNumberOfUnreadBits(void) const { return numberOfBitsUsed - readOffset; }

    /// \brief Makes a copy of the internal data for you \a _data will point to
    /// the stream. Partial bytes are left aligned.
    /// \param[out] _data The allocated copy of GetData()
    /// \return The length in bits of the stream.
    uint32_t CopyData(uint8** _data) const;

    /// \internal
    /// Set the stream to some initial data.
    void SetData(uint8 *inByteArray);

    /// Gets the data that BitStream is writing to / reading from.
    /// Partial bytes are left aligned.
    /// \return A pointer to the internal state
    inline uint8* GetData(void) const { return data; }

    /// \brief Write numberToWrite bits from the input source.
    /// \details Right aligned data means in the case of a partial byte, the bits are aligned
    /// from the right (bit 0) rather than the left (as in the normal
    /// internal representation) You would set this to true when
    /// writing user data, and false when copying bitstream data, such
    /// as writing one bitstream to another.
    /// \param[in] inByteArray The data
    /// \param[in] numberOfBitsToWrite The number of bits to write
    /// \param[in] rightAlignedBits if true data will be right aligned
    void WriteBits(const uint8* inByteArray, uint32_t numberOfBitsToWrite, const bool rightAlignedBits = true);

    /// \brief Align the bitstream to the byte boundary and then write the
    /// specified number of bits.  
    /// \details This is faster than WriteBits but
    /// wastes the bits to do the alignment and requires you to call
    /// ReadAlignedBits at the corresponding read position.
    /// \param[in] inByteArray The data
    /// \param[in] numberOfBytesToWrite The size of input.
    void WriteAlignedBytes(const uint8 *inByteArray, const uint32 numberOfBytesToWrite);

    // Endian swap bytes already in the bitstream
    void EndianSwapBytes(int byteOffset, int length);

    /// \brief Aligns the bitstream, writes inputLength, and writes input. Won't write beyond maxBytesToWrite
    /// \param[in] inByteArray The data
    /// \param[in] inputLength The size of input.
    /// \param[in] maxBytesToWrite Max bytes to write
    void WriteAlignedBytesSafe(const char *inByteArray, const uint32 inputLength, const uint32 maxBytesToWrite);

    /// \brief Read bits, starting at the next aligned bits. 
    /// \details Note that the modulus 8 starting offset of the sequence must be the same as
    /// was used with WriteBits. This will be a problem with packet
    /// coalescence unless you byte align the coalesced packets.
    /// \param[in] inOutByteArray The byte array larger than @em numberOfBytesToRead
    /// \param[in] numberOfBytesToRead The number of byte to read from the internal state
    /// \return true if there is enough byte.
    bool ReadAlignedBytes(uint8 *inOutByteArray, const uint32 numberOfBytesToRead);

    /// \brief Reads what was written by WriteAlignedBytesSafe.
    /// \param[in] inOutByteArray The data
    /// \param[in] maxBytesToRead Maximum number of bytes to read
    /// \return true on success, false on failure.
    bool ReadAlignedBytesSafe(char *inOutByteArray, int &inputLength, const int maxBytesToRead);
    bool ReadAlignedBytesSafe(char *inOutByteArray, uint32 &inputLength, const uint32 maxBytesToRead);

    /// \brief Same as ReadAlignedBytesSafe() but allocates the memory for you using new, rather than assuming it is safe to write to
    /// \param[in] outByteArray outByteArray will be deleted if it is not a pointer to 0
    /// \return true on success, false on failure.
    bool ReadAlignedBytesSafeAlloc(char **outByteArray, int &inputLength, const uint32 maxBytesToRead);
    bool ReadAlignedBytesSafeAlloc(char **outByteArray, uint32 &inputLength, const uint32 maxBytesToRead);

    /// \brief Align the next write and/or read to a byte boundary.  
    /// \details This can be used to 'waste' bits to byte align for efficiency reasons It
    /// can also be used to force coalesced bitstreams to start on byte
    /// boundaries so so WriteAlignedBits and ReadAlignedBits both
    /// calculate the same offset when aligning.
    inline void AlignWriteToByteBoundary(void) { numberOfBitsUsed += 8 - (((numberOfBitsUsed - 1) & 7) + 1); }

    /// \brief Align the next write and/or read to a byte boundary.  
    /// \details This can be used to 'waste' bits to byte align for efficiency reasons It
    /// can also be used to force coalesced bitstreams to start on byte
    /// boundaries so so WriteAlignedBits and ReadAlignedBits both
    /// calculate the same offset when aligning.
    inline void AlignReadToByteBoundary(void) { readOffset += 8 - (((readOffset - 1) & 7) + 1); }

    /// \brief Read \a numberOfBitsToRead bits to the output source.
    /// \details alignBitsToRight should be set to true to convert internal
    /// bitstream data to userdata. It should be false if you used
    /// WriteBits with rightAlignedBits false
    /// \param[in] inOutByteArray The resulting bits array
    /// \param[in] numberOfBitsToRead The number of bits to read
    /// \param[in] alignBitsToRight if true bits will be right aligned.
    /// \return true if there is enough bits to read
    bool ReadBits(uint8 *inOutByteArray, uint32_t numberOfBitsToRead, const bool alignBitsToRight = true);

    /// \brief Write a 0
    void Write0(void);

    /// \brief Write a 1
    void Write1(void);

    /// \brief Reads 1 bit and returns true if that bit is 1 and false if it is 0.
    bool ReadBit(void);

    /// \brief If we used the constructor version with copy data off, this
    /// *makes sure it is set to on and the data pointed to is copied.
    void AssertCopyData(void);

    /// \brief Use this if you pass a pointer copy to the constructor
    /// *(_copyData==false) and want to overallocate to prevent
    /// reallocation.
    void SetNumberOfBitsAllocated(const uint32_t lengthInBits);

    /// \brief Reallocates (if necessary) in preparation of writing numberOfBitsToWrite
    void AddBitsAndReallocate(const uint32_t numberOfBitsToWrite);

    /// \internal
    /// \return How many bits have been allocated internally
    uint32_t GetNumberOfBitsAllocated(void) const;

    /// \brief Read strings, non reference.
    bool Read(char *varString);
    bool Read(uint8 *varString);

    /// Write zeros until the bitstream is filled up to \a bytes
    void PadWithZeroToByteLength(uint32 bytes);

    /// \internal Unrolled inner loop, for when performance is critical
    void WriteAlignedVar8(const char *inByteArray);
    /// \internal Unrolled inner loop, for when performance is critical
    bool ReadAlignedVar8(char *inOutByteArray);
    /// \internal Unrolled inner loop, for when performance is critical
    void WriteAlignedVar16(const char *inByteArray);
    /// \internal Unrolled inner loop, for when performance is critical
    bool ReadAlignedVar16(char *inOutByteArray);
    /// \internal Unrolled inner loop, for when performance is critical
    void WriteAlignedVar32(const char *inByteArray);
    /// \internal Unrolled inner loop, for when performance is critical
    bool ReadAlignedVar32(char *inOutByteArray);

    inline void Write(const char * const inStringVar)
    {
        unsigned short l = static_cast<unsigned short>(strlen(inStringVar));
        this->Write(l);
        this->WriteAlignedBytes((const uint8*)inStringVar, (const uint32)l);
    }

    inline void Write(const wchar_t * const inStringVar)
    {
        size_t mbByteLength = wcslen(inStringVar);
        this->WriteCasted<unsigned short>(mbByteLength);
        for (uint32 i = 0; i < mbByteLength; i++)
        {
            uint16_t t;
            t = (uint16_t)inStringVar[i];
            // Force endian swapping, and write to 16 bits
            this->Write(t);
        }
    }

    inline void Write(const uint8 * const inTemplateVar)
    {
        Write((const char*)inTemplateVar);
    }
    inline void Write(char * const inTemplateVar)
    {
        Write((const char*)inTemplateVar);
    }
    inline void Write(uint8 * const inTemplateVar)
    {
        Write((const char*)inTemplateVar);
    }
    //inline void WriteCompressed(const char * const inStringVar)
    //{
    //    RakString::SerializeCompressed(inStringVar, this, 0, false);
    //}
    //inline void WriteCompressed(const wchar_t * const inStringVar)
    //{
    //    RakWString::Serialize(inStringVar, this);
    //}
    inline void WriteCompressed(const uint8 * const inTemplateVar)
    {
        WriteCompressed((const char*)inTemplateVar);
    }
    inline void WriteCompressed(char * const inTemplateVar)
    {
        WriteCompressed((const char*)inTemplateVar);
    }
    inline void WriteCompressed(uint8 * const inTemplateVar)
    {
        WriteCompressed((const char*)inTemplateVar);
    }

    inline static bool DoEndianSwap(void) {
#ifndef BITSTREAM_NATIVE_ENDIAN
        return IsNetworkOrder() == false;
#else
        return false;
#endif
    }
    inline static bool IsBigEndian(void)
    {
        return IsNetworkOrder();
    }
    inline static bool IsNetworkOrder(void) { bool r = IsNetworkOrderInternal(); return r; }
    // Not inline, won't compile on PC due to winsock include errors
    static bool IsNetworkOrderInternal(void);
    static void ReverseBytes(uint8 *inByteArray, uint8 *inOutByteArray, const uint32 length);
    static void ReverseBytesInPlace(uint8 *inOutData, const uint32 length);

private:
    BitStream(const BitStream &invalid) = delete;
    BitStream& operator = (const BitStream& invalid) = delete;

    /// \brief Assume the input source points to a native type, compress and write it.
    void WriteCompressed(const uint8* inByteArray, const uint32 size, const bool unsignedData);

    /// \brief Assume the input source points to a compressed native type. Decompress and read it.
    bool ReadCompressed(uint8* inOutByteArray, const uint32 size, const bool unsignedData);


    uint32_t numberOfBitsUsed;

    uint32_t numberOfBitsAllocated;

    uint32_t readOffset;

    uint8 *data;

    /// true if the internal buffer is copy of the data passed to the constructor
    bool copyData;

    /// BitStreams that use less than BITSTREAM_STACK_ALLOCATION_SIZE use the stack, rather than the heap to store data.  It switches over if BITSTREAM_STACK_ALLOCATION_SIZE is exceeded
    uint8 stackData[BITSTREAM_STACK_ALLOCATION_SIZE];
};

template <class _Ty>
inline void BitStream::Write(const _Ty &inTemplateVar)
{
    if (sizeof(inTemplateVar) == 1)
        WriteBits((uint8*)& inTemplateVar, sizeof(_Ty) * 8, true);
    else
    {
#ifndef BITSTREAM_NATIVE_ENDIAN
        if (DoEndianSwap())
        {
            uint8 output[sizeof(_Ty)];
            ReverseBytes((uint8*)&inTemplateVar, output, sizeof(_Ty));
            WriteBits((uint8*)output, sizeof(_Ty) * 8, true);
        }
        else
#endif
            WriteBits((uint8*)& inTemplateVar, sizeof(_Ty) * 8, true);
    }
}

template <class _Ty>
inline void BitStream::WritePtr(_Ty *inTemplateVar)
{
    if (sizeof(_Ty) == 1)
        WriteBits((uint8*)inTemplateVar, sizeof(_Ty) * 8, true);
    else
    {
#ifndef BITSTREAM_NATIVE_ENDIAN
        if (DoEndianSwap())
        {
            uint8 output[sizeof(_Ty)];
            ReverseBytes((uint8*)inTemplateVar, output, sizeof(_Ty));
            WriteBits((uint8*)output, sizeof(_Ty) * 8, true);
        }
        else
#endif
            WriteBits((uint8*)inTemplateVar, sizeof(_Ty) * 8, true);
    }
}

/// \brief Write a bool to a bitstream.
/// \param[in] inTemplateVar The value to write
template <>
inline void BitStream::Write(const bool &inTemplateVar)
{
    if (inTemplateVar)
        Write1();
    else
        Write0();
}

template <>
inline void BitStream::Write(const uint24_t &inTemplateVar)
{
    AlignWriteToByteBoundary();
    AddBitsAndReallocate(3 * 8);
    if (IsBigEndian() == false)
    {
        data[(numberOfBitsUsed >> 3) + 0] = ((uint8 *)&inTemplateVar.val)[0];
        data[(numberOfBitsUsed >> 3) + 1] = ((uint8 *)&inTemplateVar.val)[1];
        data[(numberOfBitsUsed >> 3) + 2] = ((uint8 *)&inTemplateVar.val)[2];
    }
    else
    {
        data[(numberOfBitsUsed >> 3) + 0] = ((uint8 *)&inTemplateVar.val)[3];
        data[(numberOfBitsUsed >> 3) + 1] = ((uint8 *)&inTemplateVar.val)[2];
        data[(numberOfBitsUsed >> 3) + 2] = ((uint8 *)&inTemplateVar.val)[1];
    }

    numberOfBitsUsed += 3 * 8;
}


/// \brief Write a string to a bitstream.
/// \param[in]inTemplateVar The value to write
template <>
inline void BitStream::Write(const std::string &inTemplateVar)
{
    unsigned short l = static_cast<unsigned short>(inTemplateVar.size());
    this->Write(l);
    this->WriteAlignedBytes((const uint8*)inTemplateVar.c_str(), (const uint32)l);
}
template <>
inline void BitStream::Write(const std::wstring &inTemplateVar)
{
    size_t mbByteLength = inTemplateVar.size();
    this->WriteCasted<unsigned short>(mbByteLength);
    for (uint32 i = 0; i < mbByteLength; i++)
    {
        uint16_t t;
        t = (uint16_t)inTemplateVar[i];
        // Force endian swapping, and write to 16 bits
        this->Write(t);
    }
}

template <>
inline void BitStream::Write(const char * const &inStringVar)
{
    Write(std::string(inStringVar));
}

template <>
inline void BitStream::Write(const wchar_t * const &inStringVar)
{
    Write(std::wstring(inStringVar));
}

template <>
inline void BitStream::Write(const uint8 * const &inTemplateVar)
{
    Write((const char*)inTemplateVar);
}
template <>
inline void BitStream::Write(char * const &inTemplateVar)
{
    Write((const char*)inTemplateVar);
}

template <>
inline void BitStream::Write(uint8 * const &inTemplateVar)
{
    Write((const char*)inTemplateVar);
}

/// \brief Write any integral type to a bitstream.  
/// \details Undefine BITSTREAM_NATIVE_ENDIAN if you need endian swapping.
/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
/// If you are not using BITSTREAM_NATIVE_ENDIAN the opposite is true for types larger than 1 byte
/// \param[in] inTemplateVar The value to write
template <class _Ty>
inline void BitStream::WriteCompressed(const _Ty &inTemplateVar)
{
    if (sizeof(inTemplateVar) == 1)
        WriteCompressed((uint8*)& inTemplateVar, sizeof(_Ty) * 8, true);
    else
    {
#ifndef BITSTREAM_NATIVE_ENDIAN
        if (DoEndianSwap())
        {
            uint8 output[sizeof(_Ty)];
            ReverseBytes((uint8*)&inTemplateVar, output, sizeof(_Ty));
            WriteCompressed((uint8*)output, sizeof(_Ty) * 8, true);
        }
        else
#endif
            WriteCompressed((uint8*)& inTemplateVar, sizeof(_Ty) * 8, true);
    }
}

template <>
inline void BitStream::WriteCompressed(const uint24_t &var)
{
    Write(var);
}

template <>
inline void BitStream::WriteCompressed(const bool &inTemplateVar)
{
    Write(inTemplateVar);
}

/// For values between -1 and 1
template <>
inline void BitStream::WriteCompressed(const float &inTemplateVar)
{
    ASSERTED(inTemplateVar > -1.01f && inTemplateVar < 1.01f, _FILE_AND_LINE_);
    float varCopy = inTemplateVar;
    if (varCopy < -1.0f)
        varCopy = -1.0f;
    if (varCopy > 1.0f)
        varCopy = 1.0f;
    Write((unsigned short)((varCopy + 1.0f)*32767.5f));
}

/// For values between -1 and 1
template <>
inline void BitStream::WriteCompressed(const double &inTemplateVar)
{
    ASSERTED(inTemplateVar > -1.01 && inTemplateVar < 1.01, _FILE_AND_LINE_);
    double varCopy = inTemplateVar;
    if (varCopy < -1.0f)
        varCopy = -1.0f;
    if (varCopy > 1.0f)
        varCopy = 1.0f;
    Write((uint32_t)((varCopy + 1.0)*2147483648.0));
}

/// Compress the string
//template <>
//inline void BitStream::WriteCompressed(const RakString &inTemplateVar)
//{
//    inTemplateVar.SerializeCompressed(this, 0, false);
//}
//template <>
//inline void BitStream::WriteCompressed(const RakWString &inTemplateVar)
//{
//    inTemplateVar.Serialize(this);
//}
//template <>
//inline void BitStream::WriteCompressed(const char * const &inStringVar)
//{
//    RakString::SerializeCompressed(inStringVar, this, 0, false);
//}
//template <>
//inline void BitStream::WriteCompressed(const wchar_t * const &inStringVar)
//{
//    RakWString::Serialize(inStringVar, this);
//}
//template <>
//inline void BitStream::WriteCompressed(const uint8 * const &inTemplateVar)
//{
//    WriteCompressed((const char*)inTemplateVar);
//}
//
//template <>
//inline void BitStream::WriteCompressed(char * const &inTemplateVar)
//{
//    WriteCompressed((const char*)inTemplateVar);
//}
//template <>
//inline void BitStream::WriteCompressed(uint8 * const &inTemplateVar)
//{
//    WriteCompressed((const char*)inTemplateVar);
//}

/// \brief Read any integral type from a bitstream.  Define BITSTREAM_NATIVE_ENDIAN if you need endian swapping.
/// \param[in] outTemplateVar The value to read
template <class _Ty>
inline bool BitStream::Read(_Ty &outTemplateVar)
{
    if (sizeof(outTemplateVar) == 1)
        return ReadBits((uint8*)&outTemplateVar, sizeof(_Ty) * 8, true);
    else
    {
#ifndef BITSTREAM_NATIVE_ENDIAN
        if (DoEndianSwap())
        {
            uint8 output[sizeof(_Ty)];
            if (ReadBits((uint8*)output, sizeof(_Ty) * 8, true))
            {
                ReverseBytes(output, (uint8*)&outTemplateVar, sizeof(_Ty));
                return true;
            }
            return false;
        }
        else
#endif
            return ReadBits((uint8*)& outTemplateVar, sizeof(_Ty) * 8, true);
    }
}

/// \brief Read a bool from a bitstream.
/// \param[in] outTemplateVar The value to read
template <>
inline bool BitStream::Read(bool &outTemplateVar)
{
    if (readOffset + 1 > numberOfBitsUsed)
        return false;

    if (data[readOffset >> 3] & (0x80 >> (readOffset & 7)))   // Is it faster to just write it out here?
        outTemplateVar = true;
    else
        outTemplateVar = false;

    // Has to be on a different line for Mac
    readOffset++;

    return true;
}

template <>
inline bool BitStream::Read(uint24_t &outTemplateVar)
{
    AlignReadToByteBoundary();
    if (readOffset + 3 * 8 > numberOfBitsUsed)
        return false;

    if (IsBigEndian() == false)
    {
        ((uint8 *)&outTemplateVar.val)[0] = data[(readOffset >> 3) + 0];
        ((uint8 *)&outTemplateVar.val)[1] = data[(readOffset >> 3) + 1];
        ((uint8 *)&outTemplateVar.val)[2] = data[(readOffset >> 3) + 2];
        ((uint8 *)&outTemplateVar.val)[3] = 0;
    }
    else
    {

        ((uint8 *)&outTemplateVar.val)[3] = data[(readOffset >> 3) + 0];
        ((uint8 *)&outTemplateVar.val)[2] = data[(readOffset >> 3) + 1];
        ((uint8 *)&outTemplateVar.val)[1] = data[(readOffset >> 3) + 2];
        ((uint8 *)&outTemplateVar.val)[0] = 0;
    }

    readOffset += 3 * 8;
    return true;
}

template <>
inline bool BitStream::Read(std::string &outTemplateVar)
{
    outTemplateVar.clear();
    bool b;
    unsigned short l;
    b = this->Read(l);
    if (l > 0)
    {
        outTemplateVar.resize(l);
        b = this->ReadAlignedBytes((uint8*)outTemplateVar.c_str(), l);
        if (!b)
            outTemplateVar.clear();
    }
    else
        this->AlignReadToByteBoundary();
    return b;
}

template <>
inline bool BitStream::Read(std::wstring &outTemplateVar)
{
    outTemplateVar.clear();
    size_t mbByteLength;
    this->ReadCasted<unsigned short>(mbByteLength);
    if (mbByteLength > 0)
    {
        outTemplateVar.resize(mbByteLength);
        for (uint32 i = 0; i < mbByteLength; i++)
        {
            uint16_t t;
            // Force endian swapping, and read 16 bits
            this->Read(t);
            outTemplateVar[i] = t;
        }
        return true;
    }
    return false;
}


template <>
inline bool BitStream::Read(char *&varString)
{
    return Read((uint8 *&)varString);
}

template <>
inline bool BitStream::Read(uint8 *&varString)
{
    bool b;
    unsigned short l;
    b = this->Read(l);
    if (b && l > 0)
        b = this->ReadAlignedBytes(varString, l);

    if (b == false)
        varString[0] = 0;

    varString[l] = 0;
    return b;
}

template <>
inline bool BitStream::Read(wchar_t *&varString)
{
    size_t mbByteLength;
    this->ReadCasted<unsigned short>(mbByteLength);
    if (mbByteLength > 0)
    {
        for (uint32 i = 0; i < mbByteLength; i++)
        {
            uint16_t t;
            // Force endian swapping, and read 16 bits
            this->Read(t);
            varString[i] = t;
        }
        varString[mbByteLength] = 0;
        return true;
    }
    else
    {
        wcscpy(varString, L"");
    }
    return true;
}

/// \brief Read any integral type from a bitstream.  
/// \details Undefine BITSTREAM_NATIVE_ENDIAN if you need endian swapping.
/// For floating point, this is lossy, using 2 bytes for a float and 4 for a double.  The range must be between -1 and +1.
/// For non-floating point, this is lossless, but only has benefit if you use less than half the bits of the type
/// If you are not using BITSTREAM_NATIVE_ENDIAN the opposite is true for types larger than 1 byte
/// \param[in] outTemplateVar The value to read
template <class _Ty>
inline bool BitStream::ReadCompressed(_Ty &outTemplateVar)
{
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
    if (sizeof(outTemplateVar) == 1)
        return ReadCompressed((uint8*)&outTemplateVar, sizeof(_Ty) * 8, true);
    else
    {
#ifndef BITSTREAM_NATIVE_ENDIAN
        if (DoEndianSwap())
        {
            uint8 output[sizeof(_Ty)];
            if (ReadCompressed((uint8*)output, sizeof(_Ty) * 8, true))
            {
                ReverseBytes(output, (uint8*)&outTemplateVar, sizeof(_Ty));
                return true;
            }
            return false;
        }
        else
#endif
            return ReadCompressed((uint8*)& outTemplateVar, sizeof(_Ty) * 8, true);
    }
}


template <>
inline bool BitStream::ReadCompressed(uint24_t &outTemplateVar)
{
    return Read(outTemplateVar);
}

template <>
inline bool BitStream::ReadCompressed(bool &outTemplateVar)
{
    return Read(outTemplateVar);
}

/// For values between -1 and 1
template <>
inline bool BitStream::ReadCompressed(float &outTemplateVar)
{
    unsigned short compressedFloat;
    if (Read(compressedFloat))
    {
        outTemplateVar = ((float)compressedFloat / 32767.5f - 1.0f);
        return true;
    }
    return false;
}

/// For values between -1 and 1
template <>
inline bool BitStream::ReadCompressed(double &outTemplateVar)
{
    uint32_t compressedFloat;
    if (Read(compressedFloat))
    {
        outTemplateVar = ((double)compressedFloat / 2147483648.0 - 1.0);
        return true;
    }
    return false;
}

/// For strings
//template <>
//inline bool BitStream::ReadCompressed(RakString &outTemplateVar)
//{
//    return outTemplateVar.DeserializeCompressed(this, false);
//}
//template <>
//inline bool BitStream::ReadCompressed(RakWString &outTemplateVar)
//{
//    return outTemplateVar.Deserialize(this);
//}
//template <>
//inline bool BitStream::ReadCompressed(char *&outTemplateVar)
//{
//    return RakString::DeserializeCompressed(outTemplateVar, this, false);
//}
//template <>
//inline bool BitStream::ReadCompressed(wchar_t *&outTemplateVar)
//{
//    return RakWString::Deserialize(outTemplateVar, this);
//}
//template <>
//inline bool BitStream::ReadCompressed(uint8 *&outTemplateVar)
//{
//    return RakString::DeserializeCompressed((char*)outTemplateVar, this, false);
//}

template <class destinationType, class sourceType >
void BitStream::WriteCasted(const sourceType &value)
{
    destinationType val = (destinationType)value;
    Write(val);
}

template <class _Ty> // _Ty for this function must be a float or double
void BitStream::WriteNormVector(_Ty x, _Ty y, _Ty z)
{
    ASSERTED(x <= 1.01 && y <= 1.01 && z <= 1.01 && x >= -1.01 && y >= -1.01 && z >= -1.01, _FILE_AND_LINE_);
    WriteFloat16((float)x, -1.0f, 1.0f);
    WriteFloat16((float)y, -1.0f, 1.0f);
    WriteFloat16((float)z, -1.0f, 1.0f);
}

template <class _Ty> // _Ty for this function must be a float or double
void BitStream::WriteVector(_Ty x, _Ty y, _Ty z)
{
    _Ty magnitude = sqrt(x * x + y * y + z * z);
    Write((float)magnitude);
    if (magnitude > 0.00001f)
    {
        WriteCompressed((float)(x / magnitude));
        WriteCompressed((float)(y / magnitude));
        WriteCompressed((float)(z / magnitude));
    }
}

template <class _Ty> // _Ty for this function must be a float or double
void BitStream::WriteNormQuat(_Ty w, _Ty x, _Ty y, _Ty z)
{
    Write((bool)(w < 0.0));
    Write((bool)(x < 0.0));
    Write((bool)(y < 0.0));
    Write((bool)(z < 0.0));
    Write((unsigned short)(fabs(x)*65535.0));
    Write((unsigned short)(fabs(y)*65535.0));
    Write((unsigned short)(fabs(z)*65535.0));
    // Leave out w and calculate it on the target
}

template <class _Ty> // _Ty for this function must be a float or double
void BitStream::WriteOrthMatrix(
    _Ty m00, _Ty m01, _Ty m02,
    _Ty m10, _Ty m11, _Ty m12,
    _Ty m20, _Ty m21, _Ty m22)
{

    double qw;
    double qx;
    double qy;
    double qz;

    // Convert matrix to quat
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
    float sum;
    sum = 1 + m00 + m11 + m22;
    if (sum < 0.0f) sum = 0.0f;
    qw = sqrt(sum) / 2;
    sum = 1 + m00 - m11 - m22;
    if (sum < 0.0f) sum = 0.0f;
    qx = sqrt(sum) / 2;
    sum = 1 - m00 + m11 - m22;
    if (sum < 0.0f) sum = 0.0f;
    qy = sqrt(sum) / 2;
    sum = 1 - m00 - m11 + m22;
    if (sum < 0.0f) sum = 0.0f;
    qz = sqrt(sum) / 2;
    if (qw < 0.0) qw = 0.0;
    if (qx < 0.0) qx = 0.0;
    if (qy < 0.0) qy = 0.0;
    if (qz < 0.0) qz = 0.0;
    qx = _copysign((double)qx, (double)(m21 - m12));
    qy = _copysign((double)qy, (double)(m02 - m20));
    qz = _copysign((double)qz, (double)(m10 - m01));

    WriteNormQuat(qw, qx, qy, qz);
}

template <class serializationType, class sourceType >
bool BitStream::ReadCasted(sourceType &value)
{
    serializationType val;
    bool success = Read(val);
    value = (sourceType)val;
    return success;
}

template <class _Ty> // _Ty for this function must be a float or double
bool BitStream::ReadNormVector(_Ty &x, _Ty &y, _Ty &z)
{
    float xIn, yIn, zIn;
    ReadFloat16(xIn, -1.0f, 1.0f);
    ReadFloat16(yIn, -1.0f, 1.0f);
    ReadFloat16(zIn, -1.0f, 1.0f);
    x = xIn;
    y = yIn;
    z = zIn;
    return true;
}

template <class _Ty> // _Ty for this function must be a float or double
bool BitStream::ReadVector(_Ty &x, _Ty &y, _Ty &z)
{
    float magnitude;
    //unsigned short sx,sy,sz;
    if (!Read(magnitude))
        return false;
    if (magnitude > 0.00001f)
    {
        float cx = 0.0f, cy = 0.0f, cz = 0.0f;
        ReadCompressed(cx);
        ReadCompressed(cy);
        if (!ReadCompressed(cz))
            return false;
        x = cx;
        y = cy;
        z = cz;
        x *= magnitude;
        y *= magnitude;
        z *= magnitude;
    }
    else
    {
        x = 0.0;
        y = 0.0;
        z = 0.0;
    }
    return true;
}

template <class _Ty> // _Ty for this function must be a float or double
bool BitStream::ReadNormQuat(_Ty &w, _Ty &x, _Ty &y, _Ty &z)
{
    bool cwNeg = false, cxNeg = false, cyNeg = false, czNeg = false;
    unsigned short cx, cy, cz;
    Read(cwNeg);
    Read(cxNeg);
    Read(cyNeg);
    Read(czNeg);
    Read(cx);
    Read(cy);
    if (!Read(cz))
        return false;

    // Calculate w from x,y,z
    x = (_Ty)(cx / 65535.0);
    y = (_Ty)(cy / 65535.0);
    z = (_Ty)(cz / 65535.0);
    if (cxNeg) x = -x;
    if (cyNeg) y = -y;
    if (czNeg) z = -z;
    float difference = 1.0f - x*x - y*y - z*z;
    if (difference < 0.0f)
        difference = 0.0f;
    w = (_Ty)(sqrt(difference));
    if (cwNeg)
        w = -w;

    return true;
}

template <class _Ty> // _Ty for this function must be a float or double
bool BitStream::ReadOrthMatrix(
    _Ty &m00, _Ty &m01, _Ty &m02,
    _Ty &m10, _Ty &m11, _Ty &m12,
    _Ty &m20, _Ty &m21, _Ty &m22)
{
    float qw, qx, qy, qz;
    if (!ReadNormQuat(qw, qx, qy, qz))
        return false;

    // Quat to orthogonal rotation matrix
    // http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToMatrix/index.htm
    double sqw = (double)qw*(double)qw;
    double sqx = (double)qx*(double)qx;
    double sqy = (double)qy*(double)qy;
    double sqz = (double)qz*(double)qz;
    m00 = (_Ty)(sqx - sqy - sqz + sqw); // since sqw + sqx + sqy + sqz =1
    m11 = (_Ty)(-sqx + sqy - sqz + sqw);
    m22 = (_Ty)(-sqx - sqy + sqz + sqw);

    double tmp1 = (double)qx*(double)qy;
    double tmp2 = (double)qz*(double)qw;
    m10 = (_Ty)(2.0 * (tmp1 + tmp2));
    m01 = (_Ty)(2.0 * (tmp1 - tmp2));

    tmp1 = (double)qx*(double)qz;
    tmp2 = (double)qy*(double)qw;
    m20 = (_Ty)(2.0 * (tmp1 - tmp2));
    m02 = (_Ty)(2.0 * (tmp1 + tmp2));
    tmp1 = (double)qy*(double)qz;
    tmp2 = (double)qx*(double)qw;
    m21 = (_Ty)(2.0 * (tmp1 + tmp2));
    m12 = (_Ty)(2.0 * (tmp1 - tmp2));

    return true;
}

template <class _Ty>
BitStream& operator<<(BitStream& out, _Ty& c)
{
    out.Write(c);
    return out;
}

template <class _Ty>
BitStream& operator >> (BitStream& in, _Ty& c)
{
    bool success = in.Read(c);
    (void)success;

    ASSERTED(success, _FILE_AND_LINE_);
    return in;
}
NS_FK_END

#endif // END OF FOUNDATIONKIT_BITSTREAM_HPP
