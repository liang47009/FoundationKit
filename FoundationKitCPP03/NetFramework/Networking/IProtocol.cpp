
#include "IProtocol.h"
#include "network.hpp"
#if 0

IProtocol::PROTOCOLS & IProtocol::GetProtoclsPool ()
{
    static IProtocol::PROTOCOLS protocols;
    return protocols;
}

IProtocol::IProtocol(int32 idx)
{
    PROTOCOLS & pool = GetProtoclsPool();
    pool.insert(std::pair< int32, IProtocol* >(idx, this));
}

IProtocol * IProtocol::GetMatchedProtocol(int32 idx)
{
    PROTOCOLS & pool = GetProtoclsPool();
    PROTOCOLS::iterator it = pool.find( idx );
    if ( it != pool.end() )
        return it->second;
    return nullptr;
}

void IProtocol::DispathStreamProtocol(uint64 clientID, DataStream & stream)
{
    int32 idx = stream.read<int32>();
    IProtocol * pProtocol = GetMatchedProtocol( idx );
    if ( pProtocol )
    {
        pProtocol->ProcessStreamProtocol(clientID, stream);
    }
    else
    {
        FKLog("***** Cannot found procotol by id[%d]", idx);
    }
}

void IProtocol::RemoveProtocol(int32 idx)
{
    PROTOCOLS & pool = GetProtoclsPool();
    PROTOCOLS::iterator it = pool.find(idx);
    if (it != pool.end())
        pool.erase(it);

}
#endif // 0


