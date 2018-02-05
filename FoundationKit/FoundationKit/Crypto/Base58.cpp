#include <algorithm>
#include <string.h>
#include "FoundationKit/Crypto/Base58.hpp"
NS_FK_BEGIN

/** All alphanumeric characters except for "0", "I", "O", and "l" */
std::string Base58::BitcoinAlphabet = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
std::string Base58::IPFSAlphabet    = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
std::string Base58::FlickrAlphabet  = "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";
std::string Base58::RippleAlphabet  = "rpshnaf39wBUDNEGHJKLM4PQRST7VWXYZ2bcdeCg65jkm8oFqi1tuvAxyz";

std::string Base58::encode(const uint8* source, uint32 length)
{
    // Skip & count leading zeroes.
    int zeroes = 0;
    const uint8* sourceEnd = source + length;
    while (source != sourceEnd && *source == 0)
    {
        source++;
        zeroes++;
    }

    // Allocate enough space in big-endian base58 representation
    int b58Size = length * 138 / 100 + 1; // log(256) / log(58), rounded up.
    std::vector<uint8> Base58Data(b58Size);
    int index = 0;
    while (source != sourceEnd)
    {
        int carry = *source;
        int i = 0;
        std::vector<uint8>::reverse_iterator it = Base58Data.rbegin();
        for (it;(carry != 0 || i < index)&&(it != Base58Data.rend()); it++, i++)
        {
            carry += 256 * (*it);
            *it = carry % 58;
            carry /= 58;
        }
        ASSERTED(carry == 0, _FILE_AND_LINE_);
        index = i;
        source++;
    }
    std::vector<uint8>::iterator it = Base58Data.begin() + (b58Size - index);
    while (it != Base58Data.end() && *it == 0)
        it++;
    // Translate the result into a string;
    std::string Base58String;
    Base58String.reserve(zeroes + (Base58Data.end() - it));
    Base58String.assign(zeroes, '1');
    while (it != Base58Data.end())
        Base58String += BitcoinAlphabet[*(it++)];
    return Base58String;
}

std::string Base58::encode(const std::string& source)
{
    return encode(reinterpret_cast<const uint8*>(source.c_str()), static_cast<uint32>(source.size()));
}

std::string Base58::encode(const u8string& source)
{
    return encode(source.c_str(), static_cast<uint32>(source.size()));
}

std::string Base58::encode(const std::vector<uint8>& source)
{
    return encode(source.data(), source.size());
}

std::string Base58::encode_check(const std::string& source)
{
    // add 4-byte hash check to the end
    std::string source_check(source);
    int hash = std::hash<std::string>{}(source_check);
    source_check.insert(source_check.end(), (unsigned char*)&hash, (unsigned char*)&hash + 4);
    return encode(source_check);
}

std::string Base58::encode_check(const std::vector<uint8>& source)
{
    return encode_check(std::string(source.begin(), source.end()));
}

bool Base58::decode(const char* source, uint32 length, std::vector<uint8>& dest)
{
    bool bDecodeSuccess = false;
    // Skip leading spaces.
    while (*source && isspace(*source))
        source++;

    // Skip and count leading '1's
    int zeroes = 0;
    while (*source == '1')
    {
        zeroes++;
        source++;
    }

    // Allocate enough space in big-endian base256 representation.
    int Base256Size = length * 733 / 1000 + 1; // log(58) / log(256), rounded up.
    std::vector<uint8> Base256Data(Base256Size);
    int index = 0;
    while (*source && !isspace(*source))
    {
        //Decode base58 character
        const char* ch = ::strchr(BitcoinAlphabet.c_str(), *source);
        if (ch == nullptr)
            return false;
        // Apply "b256 = b256*58+ch"
        int carry = ch - BitcoinAlphabet.c_str();
        int i = 0;
        std::vector<uint8>::reverse_iterator it = Base256Data.rbegin();
        for (it; (carry != 0 || i < index) && (it != Base256Data.rend()); ++it, ++i)
        {
            carry += 58 * (*it);
            *it = carry % 256;
            carry /= 256;
        }
        ASSERTED(carry == 0, _FILE_AND_LINE_);
        index = i;
        source++;
    }

    // Skip trailing spaces.
    while (isspace(*source))
        source++;
    if (*source != 0)
        return false;
    // Skip leading zeroes in Base256Data.
    std::vector<unsigned char>::iterator it = Base256Data.begin() + (Base256Size - index);
    while (it != Base256Data.end() && *it == 0)
        it++;
    // Copy result into output vector.
    dest.reserve(zeroes + (Base256Data.end() - it));
    dest.assign(zeroes, 0x00);
    while (it != Base256Data.end())
        dest.push_back(*(it++));
    bDecodeSuccess  = true;
    return bDecodeSuccess;
}

bool Base58::decode(const std::string& source, u8string& dest)
{
    std::vector<uint8> decodeData;
    bool bWasSuccessful = decode(source.c_str(), source.size(), decodeData);
    dest.assign(&decodeData[0], decodeData.size());
    return bWasSuccessful;
}

bool Base58::decode(const std::string& source, std::string& dest)
{
    std::vector<uint8> Result;
    bool bWasSuccessful = decode(source.c_str(), source.size(), Result);
    dest.assign((const char*)&Result[0], Result.size());
    return bWasSuccessful;
}

bool Base58::decode(const std::string& source, std::vector<uint8>& dest)
{
    return decode(source.c_str(), source.size(), dest);
}


bool Base58::decode_check(const std::string& source, std::string& dest)
{
    std::vector<uint8> Result;
    bool bWasSuccessful = decode_check(source, Result);
    dest.assign((const char*)&Result[0], Result.size());
    return bWasSuccessful;
}

bool Base58::decode_check(const std::string& source, std::vector<uint8>& dest)
{
    if (!decode(source.c_str(), source.size(), dest) || (dest.size() < 4))
    {
        dest.clear();
        return false;
    }
    // re-calculate the checksum, ensure it matches the included 4-byte checksum
    int hash = std::hash<std::string >{}(std::string(dest.begin(), dest.end() - 4));
    if (memcmp(&hash, &dest[dest.size() - 4], 4) != 0)
    {
        dest.clear();
        return false;
    }
    dest.resize(dest.size() - 4);
    return true;
}



NS_FK_END
