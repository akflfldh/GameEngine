#include "INetWorkTransport.h"

NetWork::INetWorkTransport::~INetWorkTransport() {}

NetWork::ENetWorkRole NetWork::INetWorkTransport::GetNetWorkRole() const
{
    return mNetWorkRole;
}

void NetWork::INetWorkTransport::SetNetWorkRole(ENetWorkRole role)
{

    mNetWorkRole = role;
}
