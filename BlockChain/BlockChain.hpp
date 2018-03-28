/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef BLOCKCHAIN_BLOCKCHAIN_HPP
#define BLOCKCHAIN_BLOCKCHAIN_HPP

#include <vector>
#include <string>
#include "Block.hpp"
#include "Transaction.hpp"
NS_FK_BEGIN
class BlockChain
{
public:
    BlockChain()
    {

    }

    virtual~BlockChain()
    {

    }

protected:
    std::vector<Block> Blocks;
    std::vector<Transaction> Transactions;
};
NS_FK_END
#endif // END OF BLOCKCHAIN_BLOCKCHAIN_HPP