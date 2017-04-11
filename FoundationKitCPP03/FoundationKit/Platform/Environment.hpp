/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef FOUNDATIONKIT_ENVIRONMENT_HPP
#define FOUNDATIONKIT_ENVIRONMENT_HPP

#pragma once
#include <string>
#include "FoundationKit/GenericPlatformMacros.hpp"

NS_FK_BEGIN

class Environment
{
public:
    static std::string GetEnvironmentVariable(const std::string& variable);
    static bool        HasEnvironmentVariable(const std::string& variable);
    static void        SetEnvironmentVariable(const std::string& variable, const std::string& value);
};


NS_FK_END

#endif // FOUNDATIONKIT_ENVIRONMENT_HPP
