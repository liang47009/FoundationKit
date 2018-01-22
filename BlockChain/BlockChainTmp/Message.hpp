/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKITUNITTEST_MESSAGE_HPP
#define FOUNDATIONKITUNITTEST_MESSAGE_HPP



#include <string>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Base/lexical_cast.hpp"
#include "cryptopp/rsa.h"
#include "KeyPair.hpp"
#include "SHA256.hpp"
using namespace CryptoPP;

NS_FK_BEGIN

class Message
{
public:
    Message() 
        : nom_de_domaine()
        , information()
        , hashDomainName()
        , publicKey()
        , longueurMessage()
        , signature() 
    {

    };

    // Just for the main
    bool operator<(const Message& rhs) const
    {
        if (information < rhs.information)
        {
            return true;
        }
        else if (information == rhs.information)
        {
            return (nom_de_domaine < rhs.nom_de_domaine);
        }
        else 
            return false;
    }

    bool operator==(const Message& rhs) const
    {
        return (rhs.information == this->information && rhs.nom_de_domaine == this->nom_de_domaine
            && signature == rhs.signature);
    }
    // For the MultiMap
    Message& operator=(const Message& rhs)
    {
        if (*this == rhs)
            return *this;
        information = rhs.information;
        nom_de_domaine = rhs.nom_de_domaine;
        signature = rhs.signature;
        hashDomainName = rhs.hashDomainName;
        publicKey = rhs.publicKey;
        longueurMessage = rhs.longueurMessage;
        return *this;
    }

    Message(std::string _domainName, std::string _info, const KeyPair& keypair)
        : nom_de_domaine(_domainName)
        , information(_info)
        , hashDomainName(SHA256Ex(nom_de_domaine))
        , publicKey(keypair.getPublicKey())
        , longueurMessage(nom_de_domaine.size() + information.size() + hashDomainName.size())
    {
        sign(keypair.getPrivateKey());
    }
    ~Message(){}

    std::string getNomDomaine() const
    {
        return nom_de_domaine;
    }

    std::string getinformation() const
    {
        return information;

    }
    std::string getHashDomainName() const
    {
        return hashDomainName;
    }

    RSA::PublicKey getPublicKey() const
    {
        return publicKey;
    }

    bool verifier() const
    {
        CryptoPP::RSASSA_PKCS1v15_SHA_Verifier verifier(publicKey);
        std::string message = nom_de_domaine + information;
        // Verify
        bool result = verifier.VerifyMessage((const byte*)message.c_str(), message.length(), signature, signature.size());
        return result;
    }

    SecByteBlock sign(const RSA::PrivateKey& privateKey)
    {
        AutoSeededRandomPool rng;
        CryptoPP::RSASSA_PKCS1v15_SHA_Signer signer(privateKey);
        SecByteBlock localSignature = SecByteBlock(signer.MaxSignatureLength());
        // Sign message
        std::string message = nom_de_domaine + information;
        size_t length = signer.SignMessage(rng, (const byte*)message.c_str(),message.length(), localSignature);
        signature.resize(length);
        signature = SecByteBlock(localSignature.data(), localSignature.size());
        return signature;
    }


    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar << nom_de_domaine << information << longueurMessage;
        string aa = lexical_cast<std::string>(publicKey.GetPublicExponent());
        string bb = lexical_cast<std::string>(publicKey.GetModulus());
        ar << aa << bb;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        string d, e, f;
        ar >> nom_de_domaine >> information >> longueurMessage;
        ar >> d >> e;
        publicKey.SetPublicExponent(CryptoPP::Integer(d.c_str()));
        publicKey.SetModulus(CryptoPP::Integer(e.c_str()));
    }
private:
    std::string    nom_de_domaine;	// TODO perhaps let const if serialization is ok with it
    std::string    information;
    std::string    hashDomainName;
    RSA::PublicKey publicKey;
    int64          longueurMessage;
    SecByteBlock   signature;


};

NS_FK_END

#endif // END OF FOUNDATIONKITUNITTEST_MESSAGE_HPP
