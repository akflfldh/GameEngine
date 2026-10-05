#pragma once
#include <NetWork/INetWorkTransport.h>
#include <NetWork/NetWorkType.h>

#include <WS2tcpip.h>
#include <WinSock2.h>
#include <queue>
#include <unordered_map>

namespace NetWork
{
enum class EReceiveDataType
{
    eHeader = 0,
    ePayload
};

struct SocketContext
{
    SOCKET mSocket = INVALID_SOCKET;
    ReceivePacket mReceivePacket;

    // 받을려고하는 데이터의 종류
    EReceiveDataType mDataType = EReceiveDataType::eHeader;

    // 받을려고 하는 크기
    size_t mSize = 0;

    std::vector<uint8_t> mWriteBuffer;
};

class WinNetWorkTransport : public INetWorkTransport
{
  public:
    WinNetWorkTransport();
    virtual ~WinNetWorkTransport();

    virtual bool Initialize() override;
    virtual void Update() override;

    virtual bool StartHost() override;
    virtual bool StartClient() override;

    // 없으면 ConnectHandle == 0
    virtual ConnectHandle GetNewConnectHandle() override;

    virtual SendResult Send(ConnectHandle handle, const std::vector<uint8_t> &data) override;

    virtual bool GetReceivePayload(ReceivePayload &oPayload) override;

    virtual bool GetConnectionEvent(ConnectionEvent &oEvent) override;

    virtual bool Disconnect(ConnectHandle handle) override;

  private:
    uint64_t GetNextNewConnectHandle();
    SocketContext *GetSocketContext(ConnectHandle handle);

    void SerializePacketHeader(const NetWorkPacketHeader &header, std::vector<uint8_t> &oBuffer);
    void DeserializedPacketHeader(const uint8_t *const oBuffer, NetWorkPacketHeader &oHeader);

    void MakePacketBuffer(std::vector<uint8_t> &oHeaderBuffer, const std::vector<uint8_t> &dataBuffer);

    bool SendAll(SocketContext &socketContext, const uint8_t *pData, uint64_t size);
    bool UpdateSend(SocketContext &socketContext);

    // true는 연결 유지, false는 종료 필요를 의미한다. 소켓/테이블 정리는 Update에서 수행한다.
    // 정상 수신 종료의 오류 코드는 0이며, 수신 오류는 발생 직후의 네이티브 코드를 전달한다.
    bool ReceviePacket(ConnectHandle handle, SocketContext &socket, int32_t &oNativeErrorCode);

  private:
    SOCKET mListenSocket = INVALID_SOCKET;
    // SOCKET mClientSocket = INVALID_SOCKET;
    //  0은 올바르지못한 handle 의미
    uint64_t mNextConnectHandle = 1;
    std::unordered_map<ConnectHandle, SocketContext> mConnectedSocketTable;

    std::queue<ConnectHandle> mNewConnectHandleQueue;

    std::queue<ReceivePayload> mReceviePayloadQueue;

    // 전송 갱신과 외부 소비는 같은 스레드에서 수행한다. 수신 payload 큐와는 별도의 FIFO다.
    std::queue<ConnectionEvent> mConnectionEventQueue;
};

} // namespace NetWork
