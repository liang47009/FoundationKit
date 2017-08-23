#ifndef LOSEMYMIND_IPROTOCOL_H
#define LOSEMYMIND_IPROTOCOL_H

#if 0

#include <unordered_map>
#include "FoundationKit/FoundationMacros.hpp"
#include "FoundationKit/Base/types.hpp"
#include "FoundationKit/Base/DataStream.hpp"

USING_NS_FK;

class IProtocol
{
    typedef std::unordered_map< int32, IProtocol * > PROTOCOLS;
private:

    IProtocol() {}
        
    static PROTOCOLS & GetProtoclsPool ();
        
    static IProtocol * GetMatchedProtocol(int32 idx);
public:
    IProtocol(int32 idx);
    virtual ~IProtocol() {}

    static void DispathStreamProtocol(uint64 clientID, DataStream & stream);
    static void RemoveProtocol(int32 idx);
        
    virtual void ProcessStreamProtocol(uint64 clientID, DataStream & stream) { UNUSED_ARG(clientID); UNUSED_ARG(stream); }
};

#define IMPLEMENT_PROTOCOL(CLS,IDX) CLS G_PROTOCOL_##CLS(IDX)
#endif // 0

#endif // LOSEMYMIND_IPROTOCOL_H





