/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKITUNITTEST_BLOCK_HPP
#define FOUNDATIONKITUNITTEST_BLOCK_HPP


#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Foundation/Time.hpp"
#include "BlockHeader.hpp"
#include "Transaction.hpp"
#include "SHA256.hpp"

NS_FK_BEGIN
static std::string FIRST_BLOCK_HASH = "e037f671bf37164c071d526e8960fd9122383d5d73ef3b60f1bc9f330a15c1e1";
static int DIFFICULTY_MINING = 6;
static int MAX_SIZE_ORPHANS = 50;
static int DEPTH_DELETION = 20;

class Block
{
    Block() = delete;
public:
    typedef std::shared_ptr<Block> Pointer;

    explicit Block(int)
        : previousBlockHash()
        , blockHash(FIRST_BLOCK_HASH)
        , nombreTransaction(0)
        , header(0)
        , transactions()
    {

    }

    Block(const Pointer prevBlockPtr, const std::vector<Transaction>& transactionList)
        : previousBlockHash(prevBlockPtr->blockHash)
        , nombreTransaction(transactionList.size())
        , header(prevBlockPtr->getHeader().getNumeroBloc() + 1)
        , transactions()
        , blockHash()
    {
        // Copy the Hash of the Transactions
        for (auto t : transactionList)
        {
            transactions.emplace_back(t.getHashTransaction());
        }
        BuildMerkleRoot();

    }

    Block(std::string previousBlockHash, int nbtransaction, std::vector<std::string> tr, const BlockHeader _header)
        : previousBlockHash(previousBlockHash)
        , nombreTransaction(nbtransaction)
        , transactions(tr)
        , header(_header)
        , blockHash()
    {
        BuildMerkleRoot();
    }

    ~Block()
    {

    }

    Block& operator=(Block rhs)
    {
        if (rhs == *this)
            return *this;
        header            = rhs.header;
        previousBlockHash = rhs.previousBlockHash;
        blockHash         = rhs.blockHash;
        nombreTransaction = rhs.nombreTransaction;
        transactions.clear();
        transactions = rhs.transactions;
        return *this;
    }

    bool operator==(const Block& rhs) const
    {
        return (header        == rhs.getHeader() && 
            transactions      == rhs.transactions&&
            previousBlockHash == rhs.getPreviousBlockHash()&&
            nombreTransaction == rhs.nombreTransaction);
    }

    const std::vector<std::string> getTransactions() const
    {
        return transactions;
    }

    const BlockHeader& getHeader() const
    {
        return header;
    }

    std::string getPreviousBlockHash() const
    {
        return previousBlockHash;
    }

    std::string getBlockHash() const
    {
        return blockHash;
    }


    bool isValid() const
    {
        if (nombreTransaction != transactions.size())
            return false;
        if (header.getNumeroBloc() == 0)
            return true;
        std::string HashPlainText = header.getHashMerkleRoot() + std::to_string(header.getNonce().first) + std::to_string(header.getNonce().second);
        if (SHA256Ex(HashPlainText).substr(0, DIFFICULTY_MINING) != std::string(DIFFICULTY_MINING, '0'))
            return false;
        std::vector<std::string> tr_buf = transactions;
        std::sort(tr_buf.begin(), tr_buf.end());
        if (std::unique(tr_buf.begin(), tr_buf.end()) != tr_buf.end())
            return false;
        return true;
    }

    bool containsTransactions(const Transaction&tr) const
    {
        // There is an actually improvement possible with the Merkle Root but it isn't implemented yet
        return (std::find(transactions.begin(), transactions.end(), tr.getHashTransaction()) != transactions.end());
    }

    paire solveProofofWork()
    {
        unsigned long long nonce = 0, incr = 0;
        unsigned long long const limit = (std::numeric_limits<unsigned long long>::max)();
        std::string sol(DIFFICULTY_MINING, '0');
        while (true) 
        {
            std::string HashText = SHA256Ex(header.getHashMerkleRoot() + std::to_string(incr) + std::to_string(nonce));
            if (HashText.substr(0, DIFFICULTY_MINING) == sol)
                break;
            else
                ++nonce;
            if (limit - 1 == nonce) // Increment the incr when nonce reach unsigned long long int max value
            {
                incr++;
                nonce = 0;
            }
        }
        header.setNonce(paire(incr, nonce));
        return paire(incr, nonce);
    }

    friend std::ostream& operator<<(std::ostream& os, const Block& p)
    {
        os << std::endl;
        os << "***************************************************************************" << std::endl;
        os << "Bloc hash         : " << p.blockHash << std::endl;
        os << "Previous hash     : " << p.previousBlockHash << std::endl;
        os << "Numero bloc       : " << p.getHeader().getNumeroBloc() << std::endl;
        os << "Nonce             : " << p.getHeader().getNonce().second << std::endl;
        os << "Time mined        : " << p.getHeader().getTime() << std::endl;
        os << "Liste des transactions" << std::endl;
        for (const auto& tr : p.getTransactions())
        {
            std::cout << "    " << tr << std::endl;
        }
        os << "***************************************************************************" << std::endl;
        os << std::endl;
        return os;
    }

    template<typename Archive>
    void save(Archive& ar, const unsigned int version) const
    {
        ar << nombreTransaction;
        ar << header;
        ar << transactions;
        ar << previousBlockHash;
        ar << blockHash;
    }

    template<typename Archive>
    void load(Archive &ar, const unsigned int version)
    {
        ar >> nombreTransaction;
        ar >> header;
        ar >> transactions;
        ar >> previousBlockHash;
        ar >> blockHash;
    }
private:

    /** 
     * 梅克尔树特点如下 :
     * 1.数据结构是一个树，可以是二叉树，也可以是多叉树（本BLOG以二叉树来分析）
     * 2.Merkle Tree的叶子节点的value是数据集合的单元数据或者单元数据HASH。
     * 3.Merke Tree非叶子节点value是其所有子节点value的HASH值。
     *
     *                            [   node root  ]
     *                            /              \
     *                           /                \
     *                          /                  \
     *                         /                    \
     *                        /                      \
     *                       /                        \
     *                [node1]                         [node2]
     *               /       \                       /      \
     *        [node3]         [node4]         [node5]        [node6]
     *         /   \          /   \            /    \         /    \
     *    [node7][node8] [node9][node10] [node11][node12][node13][node14]
     *    node7/8/9/10/11/12/13/14 = hash(something)
     *    node3 = hash(node7+node8)
     *    node4 = hash(node9+node10)
     *    node5 = hash(node11+node12)
     *    node6 = hash(node13+node14)
     *
     *    node1 = hash(node3+node4)
     *    node2 = hash(node5+node6)
     *
     *    noderoot = hash(node1+node2)
     */
    void BuildMerkleRoot()
    {
        // To get a even number of transactions
        if (transactions.size() & 1) 
        {
            transactions.push_back(transactions.at(transactions.size() - 1)); nombreTransaction++;
        }
        int N = transactions.size();

        std::vector<std::string> hashTree;
        hashTree.resize(2 * N - 1);
        for (int i = 0; i < N; i++)
            hashTree.at(2 * N - 2 - i) = transactions.at(i);
        for (int i = N - 2; i > -1; i--)
        {
            std::string HashPlainText = hashTree.at(2 * i + 1) + hashTree.at(2 * i + 2);
            hashTree.at(i) = SHA256Ex(SHA256Ex(HashPlainText));
        }
        header.setHashMerkleRoot(hashTree.at(0));
        header.setTime(Time::GetUTCUnixTimeStamp());
        blockHash = SHA256Ex(SHA256Ex(header.getHashMerkleRoot()));
    }


    std::string previousBlockHash; //链中前一个区块（父区块）的哈希值
    BlockHeader header;
    std::string blockHash;	  // Hash of the Block
    int nombreTransaction;
    std::vector<std::string> transactions;

};

NS_FK_END




#endif // END OF FOUNDATIONKITUNITTEST_BLOCK_HPP
