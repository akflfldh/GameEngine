#pragma once

#include <NetWork/NetWorkType.h>
#include <stdint.h>
#include <vector>

namespace NetWork
{
class INetWorkTransport
{
  public:
    INetWorkTransport() = default;
    virtual ~INetWorkTransport() = 0;

    virtual bool Initialize() = 0;
    virtual void Update() = 0;

    virtual bool StartHost() = 0;
    virtual bool StartClient() = 0;

    // 없으면 ConnectHandle == 0
    virtual ConnectHandle GetNewConnectHandle() = 0;

    virtual SendResult Send(ConnectHandle handle, const std::vector<uint8_t> &data) = 0;

    virtual bool GetReceivePayload(ReceivePayload &oPayload) = 0;

    // 가장 오래된 연결 알림 하나를 꺼낸다. 비어 있으면 출력값을 유지하고 false를 반환한다.
    virtual bool GetConnectionEvent(ConnectionEvent &oEvent) = 0;

    ENetWorkRole GetNetWorkRole() const;

    virtual bool Disconnect(ConnectHandle handle) = 0;

  protected:
    void SetNetWorkRole(ENetWorkRole role);

  private:
    ENetWorkRole mNetWorkRole = ENetWorkRole::eOffline;
};

} // namespace NetWork
