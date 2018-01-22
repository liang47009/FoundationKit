/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKITUNITTEST_SHA256_HPP
#define FOUNDATIONKITUNITTEST_SHA256_HPP

#include <string>
#include "cryptopp/sha.h"
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/hex.hpp"
NS_FK_BEGIN

static std::string SHA256Ex(const std::string& HashPlainText)
{
    static CryptoPP::SHA256 sha256;
    static int DigestSize = sha256.DigestSize();
    static std::string DigestData(DigestSize, '\0');
    sha256.CalculateDigest((CryptoPP::byte*)&DigestData[0], (const CryptoPP::byte *)HashPlainText.c_str(), HashPlainText.size());
    return hex(DigestData);
}

NS_FK_END

#endif // END OF FOUNDATIONKITUNITTEST_SHA256_HPP
