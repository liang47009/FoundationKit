#ifndef FoundationKit_anyTest_H
#define FoundationKit_anyTest_H

#include <string>
#include "any.hpp"
#include "Logger.h"

using namespace FoundationKit;

void TestAny()
{
    any a1 = 10;
    any a2 = std::string("This is any test.");

    any& a3 = a1;
    any  a4(a3);
    any  a5 = std::string("This is any test.");
}


#endif // FoundationKit_anyTest_H