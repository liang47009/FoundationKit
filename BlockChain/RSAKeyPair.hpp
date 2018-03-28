/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef BLOCKCHAIN_RSAKEYPAIR_HPP
#define BLOCKCHAIN_RSAKEYPAIR_HPP

#include <string>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/lexical_cast.hpp"
#include "FoundationKit/Foundation/BitStream.hpp"
#include "cryptopp/sha.h"
#include "cryptopp/rsa.h"
#include "cryptopp/osrng.h"
#include "cryptopp/files.h"
#include "KeyPair.hpp"
NS_FK_BEGIN

class RSAKeyPair : public KeyPair<CryptoPP::RSA::PublicKey, CryptoPP::RSA::PrivateKey>
{
public:

    RSAKeyPair()
    {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::InvertibleRSAFunction parameters;
        parameters.GenerateRandomWithKeySize(rng, 1024);
        PublicKey = CryptoPP::RSA::PublicKey(parameters);
        PrivateKey = CryptoPP::RSA::PrivateKey(parameters);
    }

    RSAKeyPair(const CryptoPP::RSA::PublicKey& pbrkey, const CryptoPP::RSA::PrivateKey& prkey)
    {
        PublicKey = pbrkey;
        PrivateKey = prkey;
    }

    RSAKeyPair(const RSAKeyPair&rhs)
    {
        PublicKey = rhs.PublicKey;
        PrivateKey = rhs.PrivateKey;
    }

    virtual std::string Encrypt(const std::string& message) override
    {
        CryptoPP::AutoSeededRandomPool rng;
        std::string cipher;
        CryptoPP::RSAES_OAEP_SHA_Encryptor Encrypter(PublicKey);
        try
        {
            CryptoPP::StringSource ss(message, true, new CryptoPP::PK_EncryptorFilter(rng, Encrypter, new CryptoPP::StringSink(cipher)));
        }
        catch (std::exception e)
        {
            FKLog("Erreur while encrypting a string");
        }
        return cipher;
    }

    virtual std::string Decrypt(const std::string& cipher)override
    {
        CryptoPP::AutoSeededRandomPool rng;
        std::string pl_text;
        CryptoPP::RSAES_OAEP_SHA_Decryptor Decrypter(PrivateKey);
        try
        {
            CryptoPP::StringSource(cipher, true, new CryptoPP::PK_DecryptorFilter(rng, Decrypter, new CryptoPP::StringSink(pl_text)));
        }
        catch (...)
        {
            FKLog("erreur while decrypting ");
        }
        return pl_text;
    }

    virtual bool Verify(const std::vector<uint8>& InData, const std::vector<uint8>& InSignature)override
    {
        CryptoPP::RSASSA_PKCS1v15_SHA_Verifier verifier(PublicKey);
        CryptoPP::SecByteBlock   signature(InSignature.data(), InSignature.size());
        return verifier.VerifyMessage(InData.data(), InData.size(), signature, signature.size());
    }

    virtual std::vector<uint8> Sign(const std::vector<uint8>& InData) override
    {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::RSASSA_PKCS1v15_SHA_Signer signer(PrivateKey);
        CryptoPP::SecByteBlock localSignature(signer.MaxSignatureLength());
        size_t length = signer.SignMessage(rng, InData.data(), InData.size(), localSignature);
        std::vector<uint8> ba;
        ba.resize(localSignature.size());
        memcpy(&ba[0], localSignature.data(), localSignature.size());
        return ba;
    }

    virtual bool SavePublicKey(const std::string& InFileName) override
    {
        CryptoPP::ByteQueue queue;
        PublicKey.Save(queue);
        return Save(InFileName, queue);
    }

    virtual bool SavePrivateKey(const std::string& InFileName) override
    {
        CryptoPP::ByteQueue queue;
        PrivateKey.Save(queue);
        return Save(InFileName, queue);
    }

    virtual bool LoadPublicKey(const std::string& InFileName)override
    {
        try
        {
            CryptoPP::ByteQueue queue;
            Load(InFileName, queue);
            PublicKey.Load(queue);
            return true; // TODO
        }
        catch (std::exception e)
        {
            return false;
        }
        catch (...)
        {
            return false;
        }
    }

    virtual bool LoadPrivateKey(const std::string& InFileName) override
    {
        try
        {
            CryptoPP::ByteQueue queue;
            Load(InFileName, queue);
            PrivateKey.Load(queue);
            return true; // TODO
        }
        catch (std::exception e)
        {
            return false;
        }
        catch (...)
        {
            return false;
        }
    }

    virtual void Save(BitStream& bs) const override
    {
        std::string aa, bb, cc, dd;
        aa = lexical_cast<std::string>(PrivateKey.GetPrivateExponent());
        bb = lexical_cast<std::string>(PrivateKey.GetModulus());
        cc = lexical_cast<std::string>(PublicKey.GetPublicExponent());
        aa = lexical_cast<std::string>(PublicKey.GetModulus());
        bs << aa << bb << cc << dd;
    }

    virtual void Load(BitStream& bs) override
    {
        std::string aa, bb, cc, dd;
        bs >> aa >> bb >> cc >> dd;
        PrivateKey.SetPrivateExponent(CryptoPP::Integer(aa.c_str()));
        PrivateKey.SetModulus(CryptoPP::Integer(bb.c_str()));
        PublicKey.SetPublicExponent(CryptoPP::Integer(cc.c_str()));
        PublicKey.SetModulus(CryptoPP::Integer(dd.c_str()));
    }
private:
    static bool Save(const std::string& filename, const CryptoPP::BufferedTransformation& bt)
    {
        CryptoPP::FileSink file(filename.c_str());
        bt.CopyTo(file);
        return file.MessageEnd();
    }

    static bool Load(const std::string& filename, CryptoPP::BufferedTransformation& bt)
    {
        bool bLoadSuccess = false;
        try
        {
            CryptoPP::FileSource file(filename.c_str(), true /*pumpAll*/);
            file.TransferTo(bt);
            bLoadSuccess = bt.MessageEnd();
        }
        catch (CryptoPP::FileSource::Err e)
        {
            return;
        }
        catch (...)
        {
            throw CryptoPP::FileSource::Err::INVALID_ARGUMENT;
        }
        return bLoadSuccess;
    }
};

NS_FK_END

#endif // END OF BLOCKCHAIN_RSAKEYPAIR_HPP
