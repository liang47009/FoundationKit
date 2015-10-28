/****************************************************************************
  Copyright (c) 2013-2014 libo.
 
  losemymind.libo@gmail.com

****************************************************************************/
#pragma once

#include <string>
#include <stdio.h>
#include "FoundationKit/Foundation/noncopyable.hpp"

NS_FK_BEGIN

/** 
 * Encode/Decode data using Base64.
 */
class Base64 : private noncopyable
{
public:

	/**
	 * Encode a block of data using Base64.
	 * 
	 * @param[in] str_in  A string to the data to be Encoded.
	 * @return The cipher string of Encoded.
	 * 
	 *   @code
	 * 	  std::string plaintext("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890+-");
	 * 	  std::string cipherbuf = Foundation::Base64::base64Encode(plaintext);
	 * 	 @endcode
	 */
	static std::string base64Encode(const std::string& str_in);

	/**
	 * Decode a block of Base64 encrypted data.
	 * 
	 * @param[in] str_in  A string to the encrypted block to be decoded.
	 * @return The plain string of decoded.
	 * 
	 *   @code
	 * 	  std::string plaintext("ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890+-");
	 * 	  std::string cipherbuf = Foundation::Base64::base64Encode(plaintext);
	 * 	  std::string plainbuf = Foundation::Base64::base64Decode(cipherbuf);
	 * 	 @endcode
	 */
	static std::string base64Decode(const std::string& str_in);


	/** 
	 * Encode data of file using Base64.
	 * @param[in]  fileHander     The file handler.
	 * @param[out] output         The cipher string of Encoded.
	 * @param[in]  add_crlf       Add cr/lf?
	 */
	static void encode(FILE* fileHander, std::string& output , bool add_crlf = true);

	/** 
	 * Encode data of string using Base64.
	 * @param[in]  input          A string to the data to be Encoded.
	 * @param[out] output         The cipher string of Encoded.
	 * @param[in]  add_crlf       Add cr/lf?
	 */
	static void encode(const std::string& input, std::string& output, bool add_crlf = true);

	/** 
	 * Encode data of char* using Base64.
	 * @param[in]  input          A pointer to the data to be encrypted.
	 * @param[in]  len            The data len.
	 * @param[out] output         The cipher string of Encoded.
	 * @param[in]  add_crlf       Add cr/lf?
	 */
	static void encode(const char* input, size_t len, std::string& output, bool add_crlf = true);

	static void encode(const unsigned char* input, size_t len, std::string& output, bool add_crlf = true);

	static void decode(const std::string& input, std::string& output);

	static void decode(const std::string& input, unsigned char* output, size_t& sz);

	/**
	 * Get size of base64 string, if the string is not Encoded using base64,
	 * it will return 0.
	 */
	static size_t      decode_length(const std::string& str64);

private:
	static	const char *bstr;
	static	const char rstr[128];
};

NS_FK_END



