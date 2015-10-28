/****************************************************************************
Definition of the AbstractCache class.

Copyright (c) 2006, Applied Informatics Software Engineering GmbH and Contributors.
Copyright (c) 2015-2016 libo.

losemymind.libo@gmail.com

Original file from pocoproject: https://github.com/pocoproject/poco

 This file was modified to fit the FoundationKit

****************************************************************************/
#pragma once

#include "LRUCache.h"
NS_FK_BEGIN

static void TestLRUCache()
{

    LRUCache<int, int>  cache0(3);
    cache0.add(0, 100);
    cache0.add(1, 101);
    cache0.add(2, 102);
    cache0.add(3, 103);
    cache0.dumpDebug();
    int i0 = *cache0.get(1);
    cache0.dumpDebug();
    i0 = *cache0.get(3);
    cache0.dumpDebug();
    cache0.add(4, 104);
    cache0.dumpDebug();
    cache0.update(0, 1000);
    cache0.update(4, 1004);
    cache0.dumpDebug();
    cache0.remove(4);
    bool bhas = cache0.has(4);
    bhas = cache0.has(0);
    cache0.dumpDebug();

}


NS_FK_END