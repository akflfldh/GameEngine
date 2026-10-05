#pragma once

#include <NetWork/NetWorkDllMacro.h>
#include <NetWork/NetWorkType.h>

#include <memory>
#include <string>
#include <vector>

namespace NetWork
{

class INetWorkTransport;

// 외부 모듈의 네트워크 진입점이다. 플랫폼별 소켓과 transport는 외부에 노출하지 않는다.
// transport의 소유·수명 관리와 호출 전달은 구현 단계에서 연결하며, 게임 handshake 정책은 외부가 담당한다.
// 갱신, 송신 요청, 큐 소비는 같은 스레드에서 수행하는 것을 전제로 한다.
class NETWORK_API NetWorkSystem
{
  public:
    static NetWorkSystem *GetInstance();

    NetWorkSystem();
    ~NetWorkSystem();

    // TODO: 초기화 구현 시 정의와 함께 bool 반환형으로 변경하여 실패를 외부에 전달한다.
    bool Initialize();

    void Update();
    void EndUpdate();

    // 전체 연결과 transport를 정리한다. 개별 연결 종료는 Disconnect를 사용한다.
    void Shutdown();

    bool StartHost(uint16_t port);
    bool StartClient(const std::string &address, uint16_t port);
    bool Disconnect(ConnectHandle handle);

    // 성공은 송신 버퍼 등록을 뜻하며 상대의 수신 완료를 보장하지 않는다.
    SendResult Send(ConnectHandle handle, const std::vector<uint8_t> &data);

    // 각 큐의 가장 오래된 항목 하나를 꺼낸다. 비어 있으면 출력값을 유지하고 false를 반환한다.
    bool GetReceivePayload(ReceivePayload &oPayload);
    bool GetConnectionEvent(ConnectionEvent &oEvent);

    ENetWorkRole GetNetWorkRole() const;

  private:
    std::unique_ptr<INetWorkTransport> mNetWorkTransport;
};

} // namespace NetWork
