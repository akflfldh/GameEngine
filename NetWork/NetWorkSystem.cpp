#include "NetWorkSystem.h"

#if defined(_WIN32)
#include <NetWork/WinNetWorkTransport.h>
#endif

NetWork::NetWorkSystem *NetWork::NetWorkSystem::GetInstance()
{
    static NetWorkSystem instance;
    return &instance;
}

NetWork::NetWorkSystem::NetWorkSystem() {}

NetWork::NetWorkSystem::~NetWorkSystem() {}

bool NetWork::NetWorkSystem::Initialize()
{

#if defined(_WIN32)
    // WinNetWorkTransport 생성

    mNetWorkTransport = std::make_unique<WinNetWorkTransport>();

#else
    // 미지원 플랫폼: 초기화 실패
#endif

    return mNetWorkTransport->Initialize();
}

void NetWork::NetWorkSystem::Update()
{
    mNetWorkTransport->Update();
}

void NetWork::NetWorkSystem::EndUpdate() {}

bool NetWork::NetWorkSystem::StartHost(uint16_t port)
{

    return mNetWorkTransport->StartHost();
}
bool NetWork::NetWorkSystem::StartClient(const std::string &address, uint16_t port)
{

    return mNetWorkTransport->StartClient();
}

bool NetWork::NetWorkSystem::Disconnect(ConnectHandle handle)
{

    return mNetWorkTransport->Disconnect(handle);
}

NetWork::SendResult NetWork::NetWorkSystem::Send(ConnectHandle handle, const std::vector<uint8_t> &data)
{

    return mNetWorkTransport->Send(handle, data);
}

bool NetWork::NetWorkSystem::GetReceivePayload(ReceivePayload &oPayload)
{

    return mNetWorkTransport->GetReceivePayload(oPayload);
}
bool NetWork::NetWorkSystem::GetConnectionEvent(ConnectionEvent &oEvent)
{

    return mNetWorkTransport->GetConnectionEvent(oEvent);
}

NetWork::ENetWorkRole NetWork::NetWorkSystem::GetNetWorkRole() const
{

    return mNetWorkTransport->GetNetWorkRole();
}