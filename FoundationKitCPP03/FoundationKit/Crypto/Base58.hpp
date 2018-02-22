/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_BASE58_HPP
#define FOUNDATIONKIT_BASE58_HPP


#include <string>
#include <vector>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/std/stdheader.h"
NS_FK_BEGIN
class Base58
{
public:
   /**
    * Encodes the source into a Base58 string
    *
    * @param source the binary payload to stringify
    * @param length the length of the payload that needs encoding
    *
    * @return the stringified form of the binary data
    */
    static std::string encode(const uint8* source, uint32 length);

   /**
    * Encodes a std::string into a Base64 string
    *
    * @param source the string data to convert
    *
    * @return a string that encodes the binary data in a way that can be safely transmitted via various Internet protocols
    */
    static std::string encode(const std::string& source);

   /**
    * Encodes a binary uint8 array into a Base64 string
    *
    * @param source the binary data to convert
    *
    * @return a string that encodes the binary data in a way that can be safely transmitted via various Internet protocols
    */
    static std::string encode(const u8string& source);

   /**
    * Encodes a binary uint8 array into a Base64 string
    *
    * @param source the binary data to convert
    *
    * @return a string that encodes the binary data in a way that can be safely transmitted via various Internet protocols
    */
    static std::string encode(const std::vector<uint8>& source);

   /**
    * Encodes a binary uint8 array into a Base64 string
    *
    * @param source the binary data to convert
    *
    * @return a string that encodes the binary data in a way that can be safely transmitted via various Internet protocols
    */
    static std::string encode_check(const std::string& source);
    static std::string encode_check(const std::vector<uint8>& source);

   /**
    * Decodes a Base64 string into an array of bytes
    *
    * @param source the stringified data to convert
    * @param length the length of the buffer being converted
    * @param dest the out buffer that will be filled with the decoded data
    *
    * @return true if the buffer was decoded, false if it failed to decode
    */
    static bool decode(const char* source, uint32 length, std::vector<uint8>& dest);

   /**
    * Decodes a Base64 string into an array of bytes
    *
    * @param source the stringified data to convert
    * @param dest the out buffer that will be filled with the decoded data
    */
    static bool decode(const std::string& source, u8string& dest);

   /**
    * Decodes a Base64 string into a std::string
    *
    * @param source the stringified data to convert
    * @param dest the out buffer that will be filled with the decoded data
    */
    static bool decode(const std::string& source, std::string& dest);

   /**
    * Decodes a Base64 string into a std::string
    *
    * @param source the stringified data to convert
    * @param dest the out buffer that will be filled with the decoded data
    */
    static bool decode(const std::string& source, std::vector<uint8>& dest);

   /**
    * Decodes a Base64 string into a std::string
    *
    * @param source the stringified data to convert
    * @param dest the out buffer that will be filled with the decoded data
    */
    static bool decode_check(const std::string& source, std::string& dest);
    static bool decode_check(const std::string& source, std::vector<uint8>& dest);
protected:
    static std::string BitcoinAlphabet;
    static std::string IPFSAlphabet;
    static std::string FlickrAlphabet;
    static std::string RippleAlphabet;
};

NS_FK_END

#endif // END OF FOUNDATIONKIT_BASE58_HPP
