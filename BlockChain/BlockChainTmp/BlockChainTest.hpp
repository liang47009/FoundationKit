/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKITUNITTEST_BLOCKCHAINTEST_HPP
#define FOUNDATIONKITUNITTEST_BLOCKCHAINTEST_HPP


#include <vector>
#include <string>
#include <iostream>
#include "Block.hpp"
#include "Identite.hpp"
#include "BlockChain.hpp"
#include "SHA256.hpp"
#include "FoundationKit/Foundation/ElapsedTimer.hpp"
NS_FK_BEGIN

void TestBlock()
{
    Identite id("Franc", "Jerome");
    Identite id1("CAVA", "ETtoi");
    Identite id2("Hola", "QueTal");
    Identite id3("MuyBien", "YTu");
    Identite id4("Ou", "COmmenet");
    Identite id5("Reviser", "L'espagnol");

    Transaction t(id, "facebook.com", "meilleur site internet");
    Transaction t1(id1, "google.com", "la vie priv¨¦e on y tient");
    Transaction t2(id2, "microsoft.com", "ils ont un bon editeur de texte");
    Transaction t22(id2, "microsoft.fr", "ils sont cools en plus");
    Transaction t3(id3, "apple.com", "f**k le FBI");
    Transaction t4(id4, "codingame", "simply de best");

    Transaction t6(id, "chill.com", "ca existe?");
    Transaction t7(id1, "spion.com", "on s'amuse");
    Transaction t8(id2, "amazon.com", "riche comme cresus");
    Transaction t82(id2, "insa.fr", "hum hum hum");
    Transaction t9(id3, "holahola.com", "hablo espagnol");
    Transaction t10(id4, "darty.com", "LOL");

    std::vector<Transaction> transactions1{ t1,t,t2,t22,t3,t4 };
    std::vector<Transaction> transactions2{ t6,t8,t82,t7,t9,t10 };

    Block genesis(0);
    std::shared_ptr<Block> ptr0 = std::make_shared<Block>(genesis);

    Block block1(ptr0, transactions1);
    std::shared_ptr<Block> ptr1 = std::make_shared<Block>(block1);

    Block block2(ptr1, transactions2);

    std::cout << "---------------------------- Test sur la BlockChain ----------------------------" << std::endl;
    BlockChain blockchain;
    blockchain.push_back(genesis);
    blockchain.push_back(block1);
    std::cout << (blockchain.find(t1) == true ? "la transaction t1 existe" : "t1 n'existe pas") << std::endl;

    std::cout << (blockchain.find(t6) == true ? "la transaction t6 existe" : "t6 n'existe pas") << std::endl;
    blockchain.push_back(block2);
    std::cout << (blockchain.find(t6) == true ? "la transaction t6 existe" : "t6 n'existe pas") << std::endl;


    std::cout << "---------------------------- Test sur les blocs ----------------------------" << std::endl;
    std::cout << "Construction de l'arbre de Merkle" << std::endl;
    std::cout << "Resolution du probleme" << std::endl;

    ElapsedTimer et;
    std::pair<unsigned long long, unsigned long long> nonce = block1.solveProofofWork();
    std::cout << "solveProofofWork Time:" << et.Secondsf() << " s | isVoild " << block1.isValid()<<  std::endl;

    std::cout << "Hash trouv¨¦ : " << SHA256Ex(std::string(block1.getHeader().getHashMerkleRoot() + std::to_string(nonce.first) + std::to_string(nonce.second))) << std::endl;
    std::cout << "Les 2 nonces sont" << nonce.first << " " << nonce.second << std::endl;
    std::cout << std::endl;
    std::cout << "Test copie bloc" << std::endl;
    Block b3 = block2;
    std::cout << b3.getHeader().getHashMerkleRoot() << " " << b3.getHeader().getNumeroBloc() << b3.getHeader().getTime() << std::endl;
    std::cout << block2.getHeader().getHashMerkleRoot() << " " << block2.getHeader().getNumeroBloc() << block2.getHeader().getTime() << std::endl;

    std::cout << std::endl << std::endl;

}


NS_FK_END


#endif // END OF FOUNDATIONKITUNITTEST_BLOCKCHAINTEST_HPP
