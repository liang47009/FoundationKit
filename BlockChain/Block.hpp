/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef BLOCKCHAIN_BLOCK_HPP
#define BLOCKCHAIN_BLOCK_HPP
#include <vector>
#include "BlockHeader.hpp"
#include "Transaction.hpp"
//Genesis Block
NS_FK_BEGIN
class Block
{
public:
    Block(const BlockHeader& InHeader, const std::vector<Transaction>& InTransactions)
        : Header(InHeader)
        , Transactions(InTransactions)
    {

    }

    ~Block()
    {

    }

    BlockHeader GetHeader()
    {
        return Header;
    }

    std::vector<Transaction> GetTransactions()
    {
        return Transactions;
    }
protected:
    BlockHeader Header;
    std::vector<Transaction> Transactions;
};

NS_FK_END

#endif // END OF BLOCKCHAIN_BLOCK_HPP
