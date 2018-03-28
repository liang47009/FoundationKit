/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef BLOCKCHAIN_KEYPAIR_HPP
#define BLOCKCHAIN_KEYPAIR_HPP
#include <string>
#include <vector>
#include "FoundationKit/Foundation/BitStream.hpp"

NS_FK_BEGIN

template<typename PublicKeyType, typename PrivateKeyType>
class KeyPair
{
public:
    void SetPublicKey(const PublicKeyType& InPublicKey)
    {
        PublicKey = InPublicKey;
    }

    void SetPrivateKey(const PrivateKeyType& InPrivateKey)
    {
        PrivateKey = InPrivateKey;
    }

    PublicKeyType GetPublicKey()
    {
        return PublicKey;
    }

    PrivateKeyType GetPrivateKey()
    {
        return PrivateKey;
    }

    virtual std::string Encrypt(const std::string& message) = 0;
    virtual std::string Decrypt(const std::string& cipher) = 0;
    virtual bool Verify(const std::vector<uint8>& InData, const std::vector<uint8>& InSignature) = 0;
    virtual std::vector<uint8> Sign(const std::vector<uint8>& InData) = 0;
    virtual bool SavePublicKey(const std::string& InFileName) = 0;
    virtual bool SavePrivateKey(const std::string& InFileName) = 0;
    virtual bool LoadPublicKey(const std::string& InFileName) = 0;
    virtual bool LoadPrivateKey(const std::string& InFileName) = 0;
    virtual void Save(BitStream& bs) const = 0;
    virtual void Load(BitStream& bs) = 0;

protected:
    PublicKeyType  PublicKey;
    PrivateKeyType PrivateKey;
};
NS_FK_END
#endif // END OF BLOCKCHAIN_KEYPAIR_HPP
