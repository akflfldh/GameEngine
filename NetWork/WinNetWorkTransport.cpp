#include "WinNetWorkTransport.h"

#include <utility>

NetWork::WinNetWorkTransport::WinNetWorkTransport() {}

NetWork::WinNetWorkTransport::~WinNetWorkTransport()
{
    // accept로 얻은 연결 소켓과 리슨/클라이언트 소켓은 각각 별도로 소유한다.
    // Winsock 사용을 종료하기 전에 모든 소켓을 명시적으로 닫는다.
    for (const auto &entry : mConnectedSocketTable)
    {
        if (entry.second.mSocket != INVALID_SOCKET)
            ::closesocket(entry.second.mSocket);
    }
    mConnectedSocketTable.clear();

    /*   if (mClientSocket != INVALID_SOCKET)
       {
           ::closesocket(mClientSocket);
           mClientSocket = INVALID_SOCKET;
       }*/

    if (mListenSocket != INVALID_SOCKET)
    {
        ::closesocket(mListenSocket);
        mListenSocket = INVALID_SOCKET;
    }

    WSACleanup();
}

bool NetWork::WinNetWorkTransport::Initialize()
{

    WSADATA wsaData = {};

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        // log
        return false;
    }

    return true;
}

void NetWork::WinNetWorkTransport::Update()
{

    ENetWorkRole role = GetNetWorkRole();
    if (role == ENetWorkRole::eOffline)
    {
        return;
    }

    if (role == ENetWorkRole::eHost)
    {
        SOCKET clientSocket = accept(mListenSocket, nullptr, nullptr);

        if (clientSocket != INVALID_SOCKET)
        {
            uint64_t connectHandle = GetNextNewConnectHandle();
            mConnectedSocketTable[connectHandle].mSocket = clientSocket;
            mNewConnectHandleQueue.push(connectHandle);

            // 기존 핸들 큐를 유지하면서 Host/Client 공통 이벤트 소비 경로에도 알린다.
            ConnectionEvent event;
            event.mHandle = connectHandle;
            event.mType = EConnectionEventType::eConnected;
            mConnectionEventQueue.push(event);
        }
    }
    else if (role == ENetWorkRole::eClient)
    {
    }

    std::vector<ConnectHandle> disconnectedHandleList;

    for (auto &socketContext : mConnectedSocketTable)
    {
        if (UpdateSend(socketContext.second) == false)
        {

            ConnectionEvent connectionEvent;
            connectionEvent.mHandle = socketContext.first;
            connectionEvent.mType = EConnectionEventType::eDisconnected;

            mConnectionEventQueue.push(connectionEvent);
            disconnectedHandleList.push_back(socketContext.first);
            continue;
        }

        int32_t receiveErrorCode = 0;
        if (ReceviePacket(socketContext.first, socketContext.second, receiveErrorCode) == false)
        {
            ConnectionEvent connectionEvent;
            connectionEvent.mHandle = socketContext.first;
            connectionEvent.mType = EConnectionEventType::eDisconnected;
            connectionEvent.mNativeErrorCode = receiveErrorCode;

            // 순회 중 컨텍스트를 제거하지 않는다. 송신 실패 경로는 continue하므로 중복 등록되지 않는다.
            mConnectionEventQueue.push(connectionEvent);
            disconnectedHandleList.push_back(socketContext.first);
        }
    }

    for (auto handle : disconnectedHandleList)
    {
        closesocket(mConnectedSocketTable[handle].mSocket);
        mConnectedSocketTable.erase(handle);
    }

    if (role == ENetWorkRole::eClient)
    {
        if (mConnectedSocketTable.empty())
        {
            SetNetWorkRole(ENetWorkRole::eOffline);
        }
    }
}

bool NetWork::WinNetWorkTransport::StartHost()
{
    // 재시작 정책이 없는 상태에서 기존 핸들을 덮어쓰면 소켓 소유권을 잃는다.
    if (mListenSocket != INVALID_SOCKET)
        return false;
    // listen socket

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (listenSocket == INVALID_SOCKET)
    {
        // log
        return false;
    }

    sockaddr_in listenSockAddr = {};
    listenSockAddr.sin_family = AF_INET;
    listenSockAddr.sin_port = htons(27015);
    listenSockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    int result = bind(listenSocket, (const sockaddr *)&listenSockAddr, sizeof(listenSockAddr));
    if (result != 0)
    {
        // log
        // 멤버로 소유권을 넘기기 전 실패한 소켓은 이 함수에서 정리한다.
        ::closesocket(listenSocket);
        return false;
    }

    result = listen(listenSocket, SOMAXCONN);

    if (result != 0)
    {
        // log
        ::closesocket(listenSocket);
        return false;
    }

    u_long nonBlocking = 1;

    // Update의 accept가 블로킹되지 않아야 하므로 설정 실패도 시작 실패로 정리한다.
    if (ioctlsocket(listenSocket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
    {
        ::closesocket(listenSocket);
        return false;
    }

    mListenSocket = listenSocket;

    SetNetWorkRole(ENetWorkRole::eHost);

    return true;
}

bool NetWork::WinNetWorkTransport::StartClient()
{
    // 살아 있는 연결을 덮어쓰지 않는다. 재연결은 별도의 종료 정책이 필요하다.
    if (mConnectedSocketTable.empty() == false)
        return false;

    // 핸들 발급 전 실패이므로 handle은 0이다. 오류 코드는 소켓 정리 전에 전달받아 보존한다.
    const auto failConnection = [this](SOCKET failedSocket, int32_t nativeErrorCode)
    {
        if (failedSocket != INVALID_SOCKET)
            ::closesocket(failedSocket);

        ConnectionEvent event;
        event.mType = EConnectionEventType::eConnectFailed;
        event.mHandle = 0;
        event.mNativeErrorCode = nativeErrorCode;
        mConnectionEventQueue.push(event);
        return false;
    };

    SOCKET clientSocket;

    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        return failConnection(clientSocket, WSAGetLastError());
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(27015);
    //"175.197.244.121"
    //"127.0.0.1"
    int addressResult = inet_pton(AF_INET, "175.197.244.121", &addr.sin_addr);
    if (addressResult != 1)
    {
        // 잘못된 주소 형식(0)은 네이티브 오류를 제공하지 않으므로 이전 오류를 재사용하지 않는다.
        int32_t errorCode = addressResult == SOCKET_ERROR ? WSAGetLastError() : 0;
        return failConnection(clientSocket, errorCode);
    }

    int result = connect(clientSocket, (const sockaddr *)(&addr), sizeof(addr));

    if (result != 0)
    {
        return failConnection(clientSocket, WSAGetLastError());
    }

    u_long nonBlocking = 1;

    // 연결 이후 Update의 송수신이 블로킹되지 않아야 하므로 설정 실패도 시작 실패로 정리한다.
    if (ioctlsocket(clientSocket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
    {
        return failConnection(clientSocket, WSAGetLastError());
    }

    ConnectHandle handle = GetNextNewConnectHandle();
    mConnectedSocketTable[handle].mSocket = clientSocket;
    ConnectionEvent event;
    event.mHandle = handle;
    event.mType = EConnectionEventType::eRequestConnect;
    mConnectionEventQueue.push(event);

    SetNetWorkRole(ENetWorkRole::eClient);

    return true;
}

NetWork::ConnectHandle NetWork::WinNetWorkTransport::GetNewConnectHandle()
{

    if (mNewConnectHandleQueue.empty())
        return 0;

    ConnectHandle handle = mNewConnectHandleQueue.front();
    mNewConnectHandleQueue.pop();
    return handle;
}

NetWork ::SendResult NetWork::WinNetWorkTransport::Send(ConnectHandle handle, const std::vector<uint8_t> &data)
{

    SendResult sendResult;

    SocketContext *destSocketContext = GetSocketContext(handle);
    if (destSocketContext == nullptr)
    {
        sendResult.bFailure = true;
        return sendResult;
    }

    if (destSocketContext->mSocket == INVALID_SOCKET)
    {
        // log
        sendResult.bFailure = true;
        return sendResult;
    }

    NetWorkPacketHeader header;
    header.mDataSize = data.size();

    std::vector<uint8_t> serializedPacketBuffer;
    SerializePacketHeader(header, serializedPacketBuffer);
    MakePacketBuffer(serializedPacketBuffer, data);

    bool sendRet = SendAll(*destSocketContext, serializedPacketBuffer.data(), serializedPacketBuffer.size());
    if (sendRet == false)
    {

        sendResult.bFailure = true;
        return sendResult;
    }

    return sendResult;
}

bool NetWork::WinNetWorkTransport::GetReceivePayload(ReceivePayload &oPayload)
{

    if (mReceviePayloadQueue.empty())
        return false;

    oPayload = std::move(mReceviePayloadQueue.front());
    mReceviePayloadQueue.pop();

    return true;
}

bool NetWork::WinNetWorkTransport::GetConnectionEvent(ConnectionEvent &oEvent)
{
    if (mConnectionEventQueue.empty())
        return false;

    oEvent = std::move(mConnectionEventQueue.front());
    mConnectionEventQueue.pop();

    return true;
}

bool NetWork::WinNetWorkTransport::Disconnect(ConnectHandle handle)
{

    SocketContext *socketContext = GetSocketContext(handle);
    if (socketContext == nullptr)
        return true;

    auto netWorkRole = GetNetWorkRole();

    if (netWorkRole == ENetWorkRole::eOffline)
        return true;

    closesocket(socketContext->mSocket);
    mConnectedSocketTable.erase(handle);

    if (mConnectedSocketTable.empty())
        SetNetWorkRole(ENetWorkRole::eOffline);
    return true;
}

uint64_t NetWork::WinNetWorkTransport::GetNextNewConnectHandle()
{

    return mNextConnectHandle++;
}

NetWork::SocketContext *NetWork::WinNetWorkTransport::GetSocketContext(ConnectHandle handle)
{

    auto it = mConnectedSocketTable.find(handle);

    if (it == mConnectedSocketTable.end())
        return nullptr;

    return &it->second;
}

void NetWork::WinNetWorkTransport::SerializePacketHeader(const NetWorkPacketHeader &header,
                                                         std::vector<uint8_t> &oBuffer)
{
    uint64_t totalSize = sizeof(header.mDataSize);

    oBuffer.resize(totalSize);

    uint64_t bufferIndex = 0;
    memcpy(&oBuffer[bufferIndex], &header.mDataSize, sizeof(header.mDataSize));
}

void NetWork::WinNetWorkTransport::DeserializedPacketHeader(const uint8_t *const buffer, NetWorkPacketHeader &oHeader)
{
    size_t headerSize = NetWorkPacketHeader::GetHeaderSize();

    uint64_t index = 0;
    memcpy(&oHeader.mDataSize, &buffer[index], sizeof(oHeader.mDataSize));
}

void NetWork::WinNetWorkTransport::MakePacketBuffer(std::vector<uint8_t> &oHeaderBuffer,
                                                    const std::vector<uint8_t> &dataBuffer)
{

    oHeaderBuffer.insert(oHeaderBuffer.end(), dataBuffer.begin(), dataBuffer.end());
}

bool NetWork::WinNetWorkTransport::SendAll(SocketContext &socketContext, const uint8_t *pData, uint64_t size)
{

    socketContext.mWriteBuffer.insert(socketContext.mWriteBuffer.end(), pData, pData + size);

    return true;
}

bool NetWork::WinNetWorkTransport::UpdateSend(SocketContext &socketContext)
{
    if (socketContext.mWriteBuffer.size() == 0)
    {
        return true;
    }

    // 전송한 데이터 크기
    uint64_t sendSize = 0;

    int sent = send(socketContext.mSocket, (const char *)(socketContext.mWriteBuffer.data()),
                    socketContext.mWriteBuffer.size(), 0);

    if (sent == SOCKET_ERROR)
    {

        if (WSAGetLastError() != WSAEWOULDBLOCK)
        {
            return false;
        }
    }
    else
    {

        socketContext.mWriteBuffer.erase(socketContext.mWriteBuffer.begin(), socketContext.mWriteBuffer.begin() + sent);
    }

    return true;
}

bool NetWork::WinNetWorkTransport::ReceviePacket(ConnectHandle handle, SocketContext &socketContext,
                                                 int32_t &oNativeErrorCode)
{
    oNativeErrorCode = 0;
    SOCKET socket = socketContext.mSocket;

    if (socket == INVALID_SOCKET)
    {
        oNativeErrorCode = WSAENOTSOCK;
        return false;
    }

    char buffer[1024];

    int receivedBytes = recv(socket, buffer, sizeof(buffer), 0);

    if (receivedBytes == SOCKET_ERROR)
    {
        // 다른 Winsock 호출 전에 오류를 보존한다. 데이터 대기는 연결 종료가 아니며 누적 버퍼도 유지한다.
        int errorCode = WSAGetLastError();
        if (errorCode == WSAEWOULDBLOCK)
            return true;

        // 현재 전송 계층은 일시적인 수신 대기 이외의 오류에서는 연결을 종료하는 정책을 사용한다.
        oNativeErrorCode = errorCode;
        return false;
    }

    if (receivedBytes == 0)
    {
        // TCP 상대의 송신 종료다. 현재는 half-close를 유지하지 않고 연결 전체를 정리한다.
        return false;
    }

    socketContext.mReceivePacket.mReceiveBuffer.insert(socketContext.mReceivePacket.mReceiveBuffer.end(), buffer,
                                                       buffer + receivedBytes);

    while (1)
    {

        if (socketContext.mDataType == EReceiveDataType::eHeader)
        {
            // header size는 고정이라 size변수 안봐도

            if (socketContext.mReceivePacket.mReceiveBuffer.size() < NetWorkPacketHeader::GetHeaderSize())
            {
                return true;
            }

            NetWorkPacketHeader header;
            DeserializedPacketHeader(socketContext.mReceivePacket.mReceiveBuffer.data(), header);

            socketContext.mDataType = EReceiveDataType::ePayload;
            socketContext.mSize = header.mDataSize;

            socketContext.mReceivePacket.mReceiveBuffer.erase(socketContext.mReceivePacket.mReceiveBuffer.begin(),
                                                              socketContext.mReceivePacket.mReceiveBuffer.begin() +
                                                                  NetWorkPacketHeader::GetHeaderSize());
        }
        else if (socketContext.mDataType == EReceiveDataType::ePayload)
        {
            auto &receiveBuffer = socketContext.mReceivePacket.mReceiveBuffer;
            // TCP 수신은 payload 중간에서 끊길 수 있으므로 완성될 때까지 상태와 누적 데이터를 유지한다.
            if (receiveBuffer.size() < socketContext.mSize)
                return true;

            auto payloadEnd = receiveBuffer.begin() + socketContext.mSize;
            ReceivePayload payload;
            payload.mHandle = handle;
            // 완료 큐가 데이터를 소유해야 이후 수신 버퍼를 소비하거나 재할당해도 안전하다.
            payload.mPayload.assign(receiveBuffer.begin(), payloadEnd);
            mReceviePayloadQueue.push(std::move(payload));

            // 뒤에 붙은 다음 패킷은 남겨두고, 빈 payload도 한 번 전달한 뒤 헤더 상태로 전환한다.
            receiveBuffer.erase(receiveBuffer.begin(), payloadEnd);
            socketContext.mDataType = EReceiveDataType::eHeader;
            socketContext.mSize = NetWorkPacketHeader::GetHeaderSize();
        }
    }

    // 현재 무엇을 기다리는가? header? payload
    // 그 기다리는 size가 변수로존재

    // 헤더라면
    // header만큼있는가?/  없으면 - >바로 리턴

    // header를 읽고 payload 의 크기를 얻는다
    // 기다리는 타입을 payload로, 기다리는 사이즈를 payload 사이즈로 그리고 다시 반복

    // 기다리는 타입이 payload라면
    // payload만큼의 크기가있는가? -> 없으면 바로 리턴
    // payload 이상의 크기가 있으면 읽고  큐에 삽입
    // 다시 기다리는 사이즈를 헤더사이즈로 ,타입도 헤더로해서  반복
}
