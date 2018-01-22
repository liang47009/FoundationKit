/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKITUNITTEST_KEYPAIR_HPP
#define FOUNDATIONKITUNITTEST_KEYPAIR_HPP


#include <string>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/lexical_cast.hpp"
#include "cryptopp/sha.h"
#include "cryptopp/rsa.h"
#include "cryptopp/osrng.h"
#include "cryptopp/files.h"

using namespace CryptoPP;

NS_FK_BEGIN
class KeyPair
{
public:
    KeyPair() 
        : publicKey()
        , privateKey()
    {
        AutoSeededRandomPool rng;
        InvertibleRSAFunction parameters;
        parameters.GenerateRandomWithKeySize(rng, 1024);
        publicKey = RSA::PublicKey(parameters);
        privateKey = RSA::PrivateKey(parameters);
    }

    KeyPair(const RSA::PublicKey& pbrkey, const RSA::PrivateKey& prkey)
        : publicKey(pbrkey)
        , privateKey(prkey)
    {

    }

    KeyPair(const KeyPair&rhs)
        : publicKey(rhs.publicKey)
        , privateKey(rhs.privateKey)
    {

    }

    ~KeyPair() {};

    RSA::PublicKey  getPublicKey() const
    {
        return publicKey;
    }

    RSA::PrivateKey getPrivateKey() const
    {
        return privateKey;
    }

    void setPrivateKey(const RSA::PrivateKey& priKey)
    {
        privateKey = priKey;
    }

    void setPublicKey(const RSA::PublicKey& pubKey)
    {
        publicKey = pubKey;
    }

    std::string encrypt(const std::string& message)
    {
        AutoSeededRandomPool rng;
        std::string cipher;
        RSAES_OAEP_SHA_Encryptor e(publicKey);
        try 
        {
            StringSource ss(message, true,new PK_EncryptorFilter(rng, e,new StringSink(cipher)));
        }
        catch (std::exception e) 
        {
           FKLog("Erreur while encrypting a string");
        }
        return cipher;
    }

    std::string decrypt(std::string cipher)
    {
        AutoSeededRandomPool rng;
        std::string pl_text;
        try 
        {
            RSAES_OAEP_SHA_Decryptor d(privateKey);
            StringSource(cipher, true,new PK_DecryptorFilter(rng, d,new StringSink(pl_text)));
        }
        catch (...) 
        {
            FKLog("erreur while decrypting ");
        }
        return pl_text;
    }
    static void savePrivateKey(const std::string& filename, const RSA::PrivateKey& key)
    {
        CryptoPP::ByteQueue queue;
        key.Save(queue);
        Save(filename, queue);
    }

    static void savePublicKey(const std::string& filename, const RSA::PublicKey& key)
    {
        CryptoPP::ByteQueue queue;
        key.Save(queue);
        Save(filename, queue);
    }

    static bool loadPrivateKey(const std::string& filename, RSA::PrivateKey& key)
    {
        try 
        {
            CryptoPP::ByteQueue queue;
            Load(filename, queue);
            key.Load(queue);
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

    static bool loadPublicKey(const std::string& filename, RSA::PublicKey& key)
    {
        try 
        {
            CryptoPP::ByteQueue queue;
            Load(filename, queue);
            key.Load(queue);
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

    template<typename Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        std::string aa, bb, cc, dd;
        aa = lexical_cast<std::string>(privateKey.GetPrivateExponent());
        bb = lexical_cast<std::string>(privateKey.GetModulus());
        cc = lexical_cast<std::string>(publicKey.GetPublicExponent());
        aa = lexical_cast<std::string>(publicKey.GetModulus());
        ar << aa << bb << cc << dd;
    }

    template<typename Archive>
    void load(Archive& ar, const unsigned int version)
    {
        std::string aa, bb, cc, dd;
        ar >> aa >> bb >> cc >> dd;
        privateKey.SetPrivateExponent(CryptoPP::Integer(aa.c_str()));
        privateKey.SetModulus(CryptoPP::Integer(bb.c_str()));
        publicKey.SetPublicExponent(CryptoPP::Integer(cc.c_str()));
        publicKey.SetModulus(CryptoPP::Integer(dd.c_str()));
    }
private:
    static void Save(const std::string& filename, const CryptoPP::BufferedTransformation& bt)
    {
        FileSink file(filename.c_str());
        bt.CopyTo(file);
        file.MessageEnd();
    }

    static void Load(const std::string& filename, CryptoPP::BufferedTransformation& bt)
    {

        try 
        {
            FileSource file(filename.c_str(), true /*pumpAll*/);
            file.TransferTo(bt);
            bt.MessageEnd();
        }
        catch (FileSource::Err e)
        {
            return;
        }
        catch (...)
        {
            throw FileSource::Err::INVALID_ARGUMENT;
        }
    }

    RSA::PrivateKey privateKey;
    RSA::PublicKey publicKey;

};


NS_FK_END


#endif // END OF FOUNDATIONKITUNITTEST_KEYPAIR_HPP
