/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKITUNITTEST_BLOCKCHAIN_HPP
#define FOUNDATIONKITUNITTEST_BLOCKCHAIN_HPP



#include <tuple>
#include <set>
#include <string>
#include <functional>
#include "Block.hpp"

NS_FK_BEGIN

class BlockChain
{
    // there is a redundancy in this tuples, but it help for handling the blockchain 
public:
    using BlockTuple = std::tuple<int, std::string, Block>; // the number of the Block, the hash of the block and the Block itself

    BlockChain() :
        blocks([](const BlockTuple& x, const BlockTuple& y)
    {
        if ((std::get<0>(x) > std::get<0>(y)))
            return true;
        if (std::get<0>(x) == std::get<0>(y))
        {
            return (std::get<1>(x) > std::get<1>(y));
        }
        return false;
    }),
        orphans([](const BlockTuple& x, const BlockTuple& y)
    {
        return (std::get<0>(x) < std::get<0>(y) && std::get<2>(x).getHeader().getTime() < std::get<2>(y).getHeader().getTime());
    }),
        leadingBlock(blocks.begin())
    {

    }

    ~BlockChain()
    {

    }

    int push_back(const Block& bloc)
    {
        // If the block is not valid
        if (!bloc.isValid())
            return BlockChain::ERROR_BLOCK_INVALID;

        if (blocks.size() == 0)
        {
            try 
            {
                blocks.insert(BlockTuple(bloc.getHeader().getNumeroBloc(), bloc.getBlockHash(), bloc));
                leadingBlock = blocks.begin();
            }
            catch (const std::exception& e)
            {
                std::cerr << "An incorrect block has failed to be insert into the BlockChain chain : " << e.what();
            };
            return	BlockChain::FIRS_BLOCK_ADDED;
        }

        // If the block is not fully compose !
        if (bloc.getBlockHash() == "")
        {
            try 
            {
                orphans.insert(BlockTuple(bloc.getHeader().getNumeroBloc(), bloc.getBlockHash(), bloc));
            }
            catch (const std::exception& e)
            {
                std::cerr << "An incorrect block has failed to be insert into the Orphans chain : " << e.what();
            };
            return	BlockChain::PREVIOUS_BLOCK_UNKNOWN;
        }

        // Pointer to the last element
        auto block_ite = blocks.begin();
        for (; block_ite != blocks.end(); block_ite++)
        {
            // If we find the parent
            if (std::get<1>(*block_ite) == bloc.getPreviousBlockHash())
            {
                try 
                {
                    BlockTuple newBloc = BlockTuple(bloc.getHeader().getNumeroBloc(), bloc.getBlockHash(), bloc);
                    // inserting into the BlockChain
                    blocks.insert(newBloc);

                    // modify the leadingBlock if necessary
                    if (bloc.getHeader().getNumeroBloc() > std::get<2>(*leadingBlock).getHeader().getNumeroBloc()) {
                        for (std::set<BlockTuple>::iterator x = blocks.begin(); x != blocks.end(); x++)
                        {
                            if (std::get<2>(*x) == std::get<2>(newBloc))
                            {
                                leadingBlock = x;
                                break;
                            }
                        }
                    }
                    return BlockChain::INSERT_NEW_BLOCK;
                }
                catch (const std::exception& e)
                {
                    std::cerr << "An incorrect block has failed to be insert into the Block chain : " << e.what();
                    return BlockChain::UNKNOWN_ERROR_WHILE_ADDIND;
                };
            }
        }
        /*!< If a parent has not been found. */
        if (!(block_ite != blocks.end()))
        {
            // add it the orphans set !
            try 
            {
                orphans.insert(BlockTuple(bloc.getHeader().getNumeroBloc(), bloc.getBlockHash(), bloc));
            }
            catch (const std::exception& e)
            {
                std::cerr << "An incorrect block has failed to be insert into the Orphans chain : " << e.what();
            };
            return	BlockChain::PREVIOUS_BLOCK_UNKNOWN;
        }
        return BlockChain::UNKNOWN_ERROR_WHILE_ADDIND;
    }

    bool find(const Transaction& trans) const
    {
        if (std::any_of(blocks.rbegin(), blocks.rend(), [&trans](const BlockTuple& bloc) 
        {

            if (std::get<2>(bloc).getBlockHash() == FIRST_BLOCK_HASH) // TODO for the first Block
                return false;
            return std::get<2>(bloc).containsTransactions(trans);
        }))
            return true;
        else
            return false;
    }

    size_t size() const
    {
        return blocks.size() + orphans.size();
    }

    void clear(std::ostream& os)
    {
        // Deleting ancient orphans that are no more useful
        while (orphans.size() > MAX_SIZE_ORPHANS)
        {

            auto it = orphans.begin();
            push_back(std::get<2>(*it)); // Try a last time...
            orphans.erase(it);
        }

        // If the blockChain is too small, no need to continue
        if (std::get<2>(*blocks.begin()).getHeader().getNumeroBloc() < DEPTH_DELETION)
            return;


        auto  block_ite = leadingBlock;

        // The Hash of the previous Block
        std::string previous_Block_Hash = std::get<2>(*leadingBlock).getPreviousBlockHash();
        os << "Last hash : " << previous_Block_Hash << std::endl;
        while (block_ite != blocks.end())
        {
            // If it is to early to delete the bloc
            if (std::get<2>(*block_ite).getHeader().getNumeroBloc() > std::get<2>(*leadingBlock).getHeader().getNumeroBloc() - DEPTH_DELETION) 
            {

                os << "To early to delete " << std::get<0>(*block_ite) << std::endl;

                // If the block is in the main chain, update the local variable previous_Block_Hash
                if (std::get<2>(*block_ite).getBlockHash() == previous_Block_Hash)
                {
                    previous_Block_Hash = std::get<2>(*block_ite).getPreviousBlockHash();
                }
                block_ite++;
            }

            // If the block are really far, and there should only be the main BlockChain with confirmed Block
            else
            {
                // If it is in the main chain
                if (std::get<2>(*block_ite).getBlockHash() == previous_Block_Hash)
                {
                    os << "No deletion : " << std::get<0>(*block_ite) << std::endl;

                    previous_Block_Hash = std::get<2>(*block_ite).getPreviousBlockHash();
                    block_ite++;
                }
                // else delete it
                else
                {
                    os << "No too early but deletion : " << std::get<0>(*block_ite) << std::endl;
                    block_ite = blocks.erase(block_ite);
                }
            }
        }
    }


    void print(std::ostream& os) const
    {
        for (const auto& exp : blocks)
        {
            os << std::get<0>(exp) << " " << std::get<1>(exp) << std::endl
                << std::endl;
        }
    }

    const BlockTuple getLeadingBlock() const
    {
        return *leadingBlock;
    }

    BlockTuple getPreviousBlock(const BlockTuple& cuple) const
    {
        // Get the previous hash
        std::string PreviousHash = std::get<2>(cuple).getPreviousBlockHash();
        if (PreviousHash == "")
            return cuple;
        std::set<BlockTuple>::iterator iter = blocks.find(cuple);
        while (iter != blocks.end() && std::get<1>(*iter) != PreviousHash)
        {
            iter++;
        }
        return *iter;
    }

    enum 
    {
        FIRS_BLOCK_ADDED = 0,
        ERROR_BLOCK_INVALID = 1,
        PREVIOUS_BLOCK_UNKNOWN = 2,
        INSERT_NEW_BLOCK = 3,
        UNKNOWN_ERROR_WHILE_ADDIND = 4
    };
private:
    std::set<BlockTuple, std::function<bool(const BlockTuple&, const BlockTuple&)> > blocks; // La blockChain
    std::set<BlockTuple, std::function<bool(BlockTuple, BlockTuple)> > orphans; // Block that don't have previous Block in the chain
    std::set<BlockTuple>::iterator leadingBlock;
};

NS_FK_END


#endif // END OF FOUNDATIONKITUNITTEST_BLOCKCHAIN_HPP
