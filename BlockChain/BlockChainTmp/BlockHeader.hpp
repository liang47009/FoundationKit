/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKITUNITTEST_BLOCKHEADER_HPP
#define FOUNDATIONKITUNITTEST_BLOCKHEADER_HPP

#include <string>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Foundation/Version.hpp"

NS_FK_BEGIN

using paire = std::pair<unsigned long long, unsigned long long>;

class BlockHeader
{
    BlockHeader() = delete;
public:
    explicit BlockHeader(int _numeroBloc) 
        : numeroBloc(_numeroBloc)
        , merkleRootHash("")
        , timestamp()
        , nonce()
    {
    }

    ~BlockHeader()
    {

    }

    bool operator==(const BlockHeader& rhs) const
    {
        return (merkleRootHash == rhs.merkleRootHash && numeroBloc == rhs.getNumeroBloc() && timestamp == rhs.getTime());
    }

    void setNonce(paire nonceVal)
    {
        nonce = nonceVal;
    }

    void setNumero(int numeroVal)
    {
        numeroBloc = numeroVal;
    }

    void setHashMerkleRoot(std::string merkHash)
    {
        merkleRootHash = merkHash;
    }

    void setTime(int64 timeVal)
    {
        timestamp = timeVal;
    }

    int64 getTime() const
    {
        return timestamp;
    }

    std::string getHashMerkleRoot() const
    {
        return merkleRootHash;
    }

    int getNumeroBloc() const
    {
        return numeroBloc;
    }

    paire getNonce()	const
    {
        return nonce;
    }

    template<typename Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar << numeroBloc;
        ar << timestamp;
        ar << nonce.first << nonce.second;
        ar << merkleRootHash;
    }

    template<typename Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar >> numeroBloc;
        ar >> timestamp;
        unsigned long long a, b;
        ar >> a >> b;
        nonce = paire(a, b);
        ar >> merkleRootHash;
    }

private:
    Version     blockVersion;
    int         numeroBloc;  // 区块索引
    int64       timestamp;   // 区块创建的时间
    paire       nonce;       // 一个用于工作量证明算法的计数器
    std::string merkleRootHash; // Merkle根, 一个哈希值，表示这个区块中全部交易构成的merkle树的根
};

NS_FK_END


#endif // END OF FOUNDATIONKITUNITTEST_BLOCKHEADER_HPP


