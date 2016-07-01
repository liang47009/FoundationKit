#ifndef LOSEMYMIND_IPROTOCOL_H
#define LOSEMYMIND_IPROTOCOL_H

#include <unordered_map>
#include "FoundationKit/Base/Types.h"
#include "FoundationKit/Base/DataStream.h"

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
        
    virtual void ProcessStreamProtocol(uint64 clientID, DataStream & stream) {}
};

#define IMPLEMENT_PROTOCOL(CLS,IDX) CLS G_PROTOCOL_##CLS(IDX)

#endif // LOSEMYMIND_IPROTOCOL_H





