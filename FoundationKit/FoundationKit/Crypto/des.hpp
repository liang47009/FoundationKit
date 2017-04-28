/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_DES_HPP
#define FOUNDATIONKIT_DES_HPP

#pragma once

#include "FoundationKit/GenericPlatformMacros.hpp"  // for NS_FK_BEGIN NS_FK_END
NS_FK_BEGIN

namespace DES{

   /**
    * Encrypt a block of data using DES.
    *
    * @param[out] output A pointer to the buffer into which the encrypted data will be written.
    * @param[out] outlen The maximum number of bytes to write into the output buffer.
    * @param[in]  input A pointer to the data to be encrypted.
    * @param[in]  inlen The number of bytes of data to encrypt.
    * @return The number of bytes encrypted, or -1 on failure.
    *
    * @code
    * #define BUFF_SIZE 256
    * unsigned char cipherbuf[BUFF_SIZE]; // the buffer that will be written into
    * unsigned char plaintext[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // the text to encrypt
    * int written = desEncrypt(cipherbuf, BUFF_SIZE, plaintext, 26);
    * @endcode
    */
    int desEncrypt(unsigned char* output, int outlen, unsigned char* input, int inlen);

   /**
    * Decrypt a block of AES encrypted data.
    *
    * @param[out] output A pointer to the buffer into which the decrypted data will be written.
    * @param[out] outlen The maximum number of bytes of data to write into the output buffer.
    * @param[in]  input A pointer to the encrypted block to be decrytped.
    * @param[in]  inlen The number of bytes to be decrypted.
    * @return The number of bytes written, or -1 on failure.
    *
    * @code
    * //first encrypt some data, then decrypt it and make sure we get out what we put in
    * unsigned char cipherbuf[256];
    * unsigned char plainbuf[256];
    * unsigned char plaintext[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // our plaintext to encrypt
    *
    * int written = desEncrypt(cipherbuf, 256, plaintext, 26);
    * written = desDecrypt(plainbuf, 256, cipherbuf, written);
    * //we now have our original plaintext in plainbuf
    * @endcode
    */
    int desDecrypt(unsigned char* output, int outlen, unsigned char* input, int inlen);

} //namespace DES

NS_FK_END

#endif // FOUNDATIONKIT_DES_HPP
