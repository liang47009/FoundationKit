/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKITUNITTEST_TRANSACTION_HPP
#define FOUNDATIONKITUNITTEST_TRANSACTION_HPP



#include <string>
#include "FoundationKit/Foundation/Time.hpp"
#include "Identite.hpp"
#include "Message.hpp"

NS_FK_BEGIN

class Transaction
{
public:
    Transaction(const Identite& ident, const std::string& _domaineName, const std::string& _information)
        : identiteSender(ident)
    {
        message = Message(_domaineName, _information, ident.getKeyPair());
        hashTransaction = SHA256Ex(identiteSender.toString() + message.getHashDomainName() + message.getinformation());
        timestamp = Time::GetUTCUnixTimeStamp();
    }

    ~Transaction() {}

    bool operator==(const Transaction& tr)
    {
        if (tr.timestamp == timestamp &&
            tr.identiteSender.getNom() == identiteSender.getNom() &&
            tr.hashTransaction == hashTransaction &&
            tr.message.getHashDomainName() == message.getHashDomainName() &&
            tr.message.getinformation() == message.getinformation() &&
            tr.message.getNomDomaine() == message.getNomDomaine())
            return true;
        return false;
    }
    bool operator<(const Transaction& tr) const
    {
        if (hashTransaction < tr.getHashTransaction())
            return true;
        else return false;
    }

    int64 getTime() const
    {
        return timestamp;
    }

    Identite getIdentite() const
    {
        return identiteSender;
    }

    Message getMessage() const
    {
        return message;
    }
    
    std::string getHashTransaction() const
    {
        return hashTransaction;
    }

    std::string toString() const
    {
        std::string os;
        os += identiteSender.toString() + message.getNomDomaine() + message.getinformation();
        return os;
    }

    bool isCorrect() const
    {
        if (identiteSender.getNom().size() <= 1 && identiteSender.getPrenom().size() <= 1)
            return false;

        if ((message.getHashDomainName() != SHA256Ex(message.getNomDomaine())) ||
            message.getinformation().size() <= 1 ||
            // message.getNomDomaine().size() <= 2  ||
            message.getPublicKey().GetPublicExponent() == identiteSender.getPublicKey().GetPublicExponent() ||
            hashTransaction != SHA256Ex(identiteSender.toString() + message.getHashDomainName() + message.getinformation())
            // TODO Something about the time
            )
            return false;
        return true;
    }

    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar << identiteSender;
        ar << message;
        ar << hashTransaction;
        ar << timestamp;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar >> identiteSender;
        ar >> message;
        ar >> hashTransaction;
        ar >> timestamp;
        message.sign(identiteSender.getKeyPair().getPrivateKey());
    }
private:
    Identite identiteSender;
    Message message;
    std::string hashTransaction;
    int64 timestamp;
};

NS_FK_END


#endif // END OF FOUNDATIONKITUNITTEST_TRANSACTION_HPP
