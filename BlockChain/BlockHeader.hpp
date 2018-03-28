/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef BLOCKCHAIN_BLOCKHEADER_HPP
#define BLOCKCHAIN_BLOCKHEADER_HPP

#include "FoundationKit/GenericPlatformMacros.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Foundation/Version.hpp"
#include "FoundationKit/Foundation/BitStream.hpp"

NS_FK_BEGIN
using paire = std::pair<uint64, uint64>;
class BlockHeader
{
    BlockHeader() = delete;
public:
    enum {BlockHashSize = 64};

    BlockHeader(int64 Index)
        : BlockVersion()
        , BlockIndex(Index)
        , Timestamp()
        , BlockNonce()
    {
        memset(BlockHash, 0, sizeof(BlockHash));
        memset(PreviousBlockHash, 0, sizeof(PreviousBlockHash));
        memset(MerkleRootHash, 0, sizeof(MerkleRootHash));
    }

    BlockHeader& SetVersion(const Version InVersion)
    {
        BlockVersion = InVersion;
        return (*this);
    }

    BlockHeader& SetBlockIndex(int64 InBlockIndex)
    {
        BlockIndex = InBlockIndex;
        return (*this);
    }

    BlockHeader& SetTimestamp(int64 InTimestamp)
    {
        Timestamp = InTimestamp;
        return (*this);
    }

    BlockHeader& SetBlockHash(const char* InBlockHash)
    {
        memcpy(BlockHash, InBlockHash, BlockHashSize);
        return (*this);
    }

    BlockHeader& SetPreviousBlockHash(const char* InPreviousBlockHash)
    {
        memcpy(PreviousBlockHash, InPreviousBlockHash, BlockHashSize);
        return (*this);
    }

    BlockHeader& SetMerkleRootHash(const char* InMerkleRootHash)
    {
        memcpy(MerkleRootHash, InMerkleRootHash, BlockHashSize);
        return (*this);
    }

    BlockHeader& SetBlockNonce(const paire& InBlockNonce)
    {
        BlockNonce = InBlockNonce;
        return (*this);
    }

    Version GetBlockVersion()const
    {
        return BlockVersion;
    }

    int64 GetBlockIndex() const 
    {
        return BlockIndex;
    }

    int64 GetTimestamp() const
    {
        return Timestamp;
    }

    const char* GetBlockHash()const
    {
        return BlockHash;
    }

    const char* GetPreviousBlockHash() const
    {
        return PreviousBlockHash;
    }

    const char* GetMerkleRootHash() const
    {
        return MerkleRootHash;
    }

    paire GetBlockNonce() const
    {
        return BlockNonce;
    }

    void Save(BitStream& bs) const
    {
        bs << BlockVersion;
        bs << BlockIndex;
        bs << Timestamp;
        bs << BlockNonce.first << BlockNonce.second;
        bs << BlockHash;
        bs << PreviousBlockHash;
        bs << MerkleRootHash;

    }

    void Load(BitStream& bs)
    {
        bs >> BlockVersion;
        bs >> BlockIndex;
        bs >> Timestamp;
        bs >> BlockNonce.first >> BlockNonce.second;
        bs >> BlockHash;
        bs >> PreviousBlockHash;
        bs >> MerkleRootHash;

    }

    bool operator==(const BlockHeader& other)
    {
        return (BlockVersion == other.GetBlockVersion() &&
            BlockIndex       == other.GetBlockIndex() &&
            Timestamp        == other.GetTimestamp() &&
            BlockNonce       == other.GetBlockNonce() &&
            memcmp(BlockHash, other.GetBlockHash(), BlockHashSize) == 0 &&
            memcmp(PreviousBlockHash, other.GetPreviousBlockHash(), BlockHashSize) == 0 &&
            memcmp(MerkleRootHash, other.GetMerkleRootHash(), BlockHashSize) == 0);
    }

    bool operator!=(const BlockHeader& other)
    {
        return !(*this == other);
    }

protected:     
    Version     BlockVersion;
    int64       BlockIndex;
    int64       Timestamp;
    paire       BlockNonce;
    char        BlockHash[BlockHashSize+1];
    char        PreviousBlockHash[BlockHashSize+1];
    char        MerkleRootHash[BlockHashSize + 1];
};

NS_FK_END


#endif // END OF BLOCKCHAIN_BLOCKHEADER_HPP
