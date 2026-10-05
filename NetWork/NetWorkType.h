#pragma once

#include <cstddef>
#include <stdint.h>
#include <vector>

namespace NetWork
{
using ConnectHandle = uint64_t;

// 전송 계층의 연결 변화이며, 게임 참가 승인이나 애플리케이션 handshake 완료를 의미하지 않는다.
enum class EConnectionEventType
{
    eNone = 0,
    eRequestConnect,
    eConnected,
    eConnectFailed,
    eDisconnected
};

// 외부에서 나중에 소비할 연결 알림의 값 복사본이다.
// 소켓이나 SocketContext를 소유/참조하지 않으므로 연결이 제거된 뒤에도 알림 정보는 유지된다.
struct ConnectionEvent
{
    EConnectionEventType mType = EConnectionEventType::eNone;
    // 연결 핸들 발급 전 접속 실패처럼 연결을 식별할 수 없는 경우에는 0이다.
    ConnectHandle mHandle = 0;
    // 플랫폼 오류 진단용 코드다. 정상 연결/종료 등 네이티브 오류가 없으면 0이다.
    int32_t mNativeErrorCode = 0;
};

// 모듈
struct NetWorkPacketHeader
{
    // payload 사이즈
    uint64_t mDataSize = 0;

    static size_t GetHeaderSize()
    {
        return sizeof(mDataSize);
    }
};

struct SendResult
{
    bool bFailure = false;
};

// 모듈
struct ReceivePacket
{
    std::vector<uint8_t> mReceiveBuffer;
};

// 외부
struct ReceivePayload
{
    ConnectHandle mHandle;
    std::vector<uint8_t> mPayload;
};

enum class ENetWorkRole
{
    eOffline = 0,
    eHost,
    eClient
};

} // namespace NetWork
