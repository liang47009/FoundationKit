/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef BLOCKCHAIN_TRANSACTION_HPP
#define BLOCKCHAIN_TRANSACTION_HPP

#include <string>
#include "FoundationKit/forward.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "SHA256.hpp"
NS_FK_BEGIN
class Transaction
{
public:
    Transaction(const std::string& InRecipient, const std::string& InSender, int InAmount)
        : TransactionTimestamp(Time::GetUTCUnixTimeStamp())
        , Recipient(InRecipient)
        , Sender(InSender)
        , Amount(InAmount)
    {
        std::string PlainText = InRecipient;
        PlainText += InSender;
        PlainText += lexical_cast<std::string>(Amount);
        PlainText += lexical_cast<std::string>(TransactionTimestamp);
        TransactionHash = SHA256Ex(PlainText);
    }

    ~Transaction()
    {
    
    }

    int64 GetTimestamp()
    {
        return TransactionTimestamp;
    }

    std::string GetHash()
    {
        return TransactionHash;
    }

    std::string GetRecipient()
    {
        return Recipient;
    }

    std::string GetSender()
    {
        return Sender;
    }

    int GetAmount()
    {
        return Amount;
    }

private:
    int64       TransactionTimestamp;
    std::string TransactionHash;
    std::string Recipient;
    std::string Sender;
    int         Amount;
};
NS_FK_END
#endif // END OF BLOCKCHAIN_TRANSACTION_HPP