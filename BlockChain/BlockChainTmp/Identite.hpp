/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKITUNITTEST_IDENTITE_HPP
#define FOUNDATIONKITUNITTEST_IDENTITE_HPP


#include <string>
#include <iostream>
#include "FoundationKit/GenericPlatformMacros.hpp"
#include "KeyPair.hpp"

NS_FK_BEGIN

class Identite
{
public:
    Identite(std::string _nom, std::string _prenom)
        : nom(_nom)
        , prenom(_prenom)
    {
        cle = KeyPair();
    }

    Identite(std::string _nom, std::string _prenom, const KeyPair &cles)
        : nom(_nom)
        , prenom(_prenom)
        , cle(cles)
    {

    }

    ~Identite() {};

    std::string getNom() const
    {
        return nom;
    }
    std::string getPrenom() const
    {
        return prenom;
    }

    RSA::PublicKey  getPublicKey() const
    {
        return cle.getPublicKey();
    }

    RSA::PrivateKey getPrivateKey() const
    {
        return cle.getPrivateKey();
    }

    KeyPair getKeyPair() const
    {
        return cle;
    }

    std::string toString() const
    {
        return getNom() + getPrenom();
    }

    std::ostream& operator<<(std::ostream& os)
    {
        os << prenom << " " << nom << std::endl;
        return os;
    }


    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        ar << nom;
        ar << prenom;
        ar << cle;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        ar >> nom;
        ar >> prenom;
        ar >>cle;
    }
private:
    std::string nom;
    std::string prenom;
    KeyPair		cle;
};

NS_FK_END


#endif // END OF FOUNDATIONKITUNITTEST_IDENTITE_HPP
