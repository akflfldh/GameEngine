#include "GameNetworkSystem.h"
#include "NetWork/NetWorkSystem.h"
#include <Core/AnimatorComponent.h>
#include <Core/ControllableEntity.h>
#include <Core/Entity.h>
#include <Core/GameMode.h>
#include <Core/LightComponent.h>
#include <Core/Map.h>
#include <Core/ObjectController.h>
#include <Core/PlayerController.h>

Core::GameNetworkSystem *Core::GameNetworkSystem::GetInstance()
{
    static GameNetworkSystem instance;

    return &instance;
}

Core::GameNetworkSystem::GameNetworkSystem() : mNetWorkSystem(nullptr) {}

Core::GameNetworkSystem::~GameNetworkSystem() {}

bool Core::GameNetworkSystem::Initialize()
{
    if (mNetWorkSystem == nullptr)
        mNetWorkSystem = NetWork::NetWorkSystem::GetInstance();

    return mNetWorkSystem->Initialize();
}

void Core::GameNetworkSystem::BindPlayContext(GameMode *gameMode, Map *map)
{

    mGameMode = gameMode;
    mMap = map;



    

    // 새로운맵전환
    // 기존 id에 새 맵의 객체 연결 ,
    if (mNetWorkSystem->GetNetWorkRole() == NetWork::ENetWorkRole::eHost)
    {
        if (mMap)
        {
            auto localPlayerController = mMap->GetCurrentObjectController();
            Object *localPlayerObject = nullptr;
            if (localPlayerController)
                localPlayerObject = mMap->GetCurrentObjectController()->GetPossessObject();

            if (mLocalPlayerNetID == 0)
                mLocalPlayerNetID = GetNewPlayerNetID();

            mNetIDTable[mLocalPlayerNetID].mObjectPtr = localPlayerObject;
            mNetIDTable[mLocalPlayerNetID].mConnectHandle = 0;
            mNetIDTable[mLocalPlayerNetID].mControllerPtr = static_cast<PlayerController *>(localPlayerController);
        }
    }
}

void Core::GameNetworkSystem::UnbindPlayContext()
{

    mMap = nullptr;
    mGameMode = nullptr;
}

void Core::GameNetworkSystem::BeginPlay()
{

    //
    // host 일경우에만
    if (mNetWorkSystem->GetNetWorkRole() == NetWork::ENetWorkRole::eHost)
    {
        // 이미등록되어있다 실패
        if (mLocalPlayerNetID != 0)
            return;

        // 맵 시작 전 Host 시작 : 서버만 시작하고, 객체 연결은 `BeginPlay()`에서 처리
        if (mMap)
        {
            auto localPlayerController = mMap->GetCurrentObjectController();
            Object *localPlayerObject = nullptr;
            if (localPlayerController)
                localPlayerObject = mMap->GetCurrentObjectController()->GetPossessObject();

            mLocalPlayerNetID = GetNewPlayerNetID();
            mNetIDTable[mLocalPlayerNetID].mObjectPtr = localPlayerObject;
            mNetIDTable[mLocalPlayerNetID].mConnectHandle = 0;
            mNetIDTable[mLocalPlayerNetID].mControllerPtr = static_cast<PlayerController *>(localPlayerController);
        }
    }
}

void Core::GameNetworkSystem::Update()
{

    if (mNetWorkSystem == nullptr)
        return;

    // 받은 NetWork 이벤트 처리

    NetWork::ConnectionEvent connectionEvent;
    while (mNetWorkSystem->GetConnectionEvent(connectionEvent))
    {
        if (connectionEvent.mType == NetWork::EConnectionEventType::eConnected)
        {
            // host로 client의 접속요청이 들어옴 이라고 볼수있음

            // 일단은 무조건 승인, -> 플레이어 NETID 생성 및 호스트쪽에도 플레이어 오브젝트생성 등등
            PlayerNetID newPlayerID = AcceptNewPlayer(connectionEvent.mHandle);

            // 그 클라이언트에게는  해당플레이어 ID 전달및, 서버에있던 다른플레이어들의 ID도 전달 및 오브젝트를
            // 생성하도록하는 패킷전달해야함
            AcknowledgePlayerConnect(newPlayerID);
            // acknowledgeClientConnect(packet(playerid ) );

            // 다른클라이언트들에게는 새로들어온 클라이언트의 ID 및 오브젝트를 생성하도록하는패킷을 전달해야함.
            BroadCastNewPlayerConnect(newPlayerID);
        }
        else if (connectionEvent.mType == NetWork::EConnectionEventType::eConnectFailed)
        {
        }
        else if (connectionEvent.mType == NetWork::EConnectionEventType::eDisconnected)
        {
        }
    }

    // 받은 패킷을 꺼내서 처리
    NetWork::ReceivePayload payload;
    while (mNetWorkSystem->GetReceivePayload(payload))
    {
        // header부분은 고정이니깐 header부분크기만큼 가져와서 Deserialized

        // bool ret =      DeserializedPacketHeader(payload,header);
        PacketHeader packetHeader;

        if (packetHeader.GetHeaderSize() > payload.mPayload.size())
        { // 잘못됨
            continue;
        }

        packetHeader.DeSerialize(payload.mPayload.data());

        uint64_t packetHeaderSize = packetHeader.GetHeaderSize();
        uint8_t *packetData = payload.mPayload.data() + packetHeaderSize;
        // header type
        switch (packetHeader.mPacketType)
        {
        case EPacketType::eAckPlayerConnect:
        {

            AcknowledgePlayerConnectPacket ackPlayerConnectPacket;
            if ((ackPlayerConnectPacket.GetPacketDataSize() + packetHeaderSize) != payload.mPayload.size())
                continue;

            ackPlayerConnectPacket.DeSerialize(packetData);

            ProcessAckPlayerConnectPacket(ackPlayerConnectPacket, payload.mHandle);
        }
        break;
        case EPacketType::eNewPlayerConnect:
        {
            NewPlayerConnectPacket newPlayerConnectPacket;
            if ((newPlayerConnectPacket.GetPacketDataSize() + packetHeaderSize) != payload.mPayload.size())
                continue;

            newPlayerConnectPacket.DeSerialize(packetData);

            ProcessNewPlayerPacket(newPlayerConnectPacket);
        }
        break;
        case EPacketType::ePlayerAction:
        {

            PlayerActionPacket playerActionPacket;
            if (!playerActionPacket.DeSerialize(packetData, payload.mPayload.size() - packetHeaderSize))
                continue;

            ProcessPlayerActionPacket(playerActionPacket, payload.mHandle);
        }
        break;
        case EPacketType::ePlayerState:
        {

            PlayerStatePacket playerStatePacket;
            playerStatePacket.DeSerialize(packetData);

            ProcessPlayerStatePacket(playerStatePacket);
        }
        break;
        }

        // type에 맞게 Deserialize 등 처리
    }

    if (mNetWorkSystem->GetNetWorkRole() == NetWork::ENetWorkRole::eClient)
    {
        auto localPlayerIDIt = mNetIDTable.find(mLocalPlayerNetID);
        if (localPlayerIDIt != mNetIDTable.end())
        {

            auto playerController = localPlayerIDIt->second.mControllerPtr.Get();

            std::unordered_map<std::string, InputActionValue> logicalInputActions =
                playerController->CollectLocalInputActions();

            // send host로
            // 만약 host면 전송안함
            // 그냥 업데이트하자

            for (auto &action : logicalInputActions)
            {
                PlayerActionPacket playerActionPacket;
                playerActionPacket.mActionName = action.first;
                playerActionPacket.mActionValueType = action.second.mType;
                playerActionPacket.mActionValue = action.second.mValue;

                std::vector<uint8_t> serializedBuffer;
                playerActionPacket.Serialize(serializedBuffer);

                //            SendInputAction(localPlayerIDIt->first);

                mNetWorkSystem->Send(mHostConnectHandle, serializedBuffer);
            }
        }
    }

    // 패킷을 전달

    if (mNetWorkSystem)
    {
        mNetWorkSystem->Update();
    }
}

void Core::GameNetworkSystem::EndUpdate()
{

    // ProcessPlayerStatePacket();
    // ProcessPlayerStatePacket()

    // 모든 클라이언트들에 playerStatePacket 전달
    SendPlayerState();
}

Core::PlayerNetID Core::GameNetworkSystem::GetLocalPlayerID() const
{
    return mLocalPlayerNetID;
}

bool Core::GameNetworkSystem::StartHost()
{

    // Offline 플레이 도중 Host 시작: 서버 시작 성공 후, 이미 존재하는 로컬 객체·Controller를 최초 등록.
    if (mNetWorkSystem)
    {
        // 이미 호스트이다. - > 실패
        if (mNetWorkSystem->GetNetWorkRole() == NetWork::ENetWorkRole::eHost)
            return true;

        // 이미 등록되어있다 실패
        if (mLocalPlayerNetID != 0)
            return true;

        bool ret = mNetWorkSystem->StartHost(0);

        if (ret)
        {

            if (mMap)
            {

                auto localPlayerController = mMap->GetCurrentObjectController();

                Object *localPlayerObject = nullptr;
                if (localPlayerController)
                    localPlayerObject = mMap->GetCurrentObjectController()->GetPossessObject();

                // localPlayer등록은 미룬다.
                if (localPlayerController == nullptr || localPlayerObject == nullptr)
                    return true;

                mLocalPlayerNetID = GetNewPlayerNetID();
                mNetIDTable[mLocalPlayerNetID].mObjectPtr = localPlayerObject;
                mNetIDTable[mLocalPlayerNetID].mConnectHandle = 0;
                mNetIDTable[mLocalPlayerNetID].mControllerPtr = static_cast<PlayerController *>(localPlayerController);
            }
        }

        return ret;
    }

    return false;
}

bool Core::GameNetworkSystem::StartClient()
{

    if (mNetWorkSystem)
    {
        return mNetWorkSystem->StartClient("", 0);
    }

    return false;
}

Core::PlayerNetID Core::GameNetworkSystem::AcceptNewPlayer(NetWork::ConnectHandle connectHandle)
{

    // player Net ID 생성  - 테이블 등록
    PlayerNetID playerNetID = GetNewPlayerNetID();
    mNetIDTable[playerNetID].mConnectHandle = connectHandle;

    // 캐릭터생성 (일단은 고정된캐릭터이겠고)
    mNetIDTable[playerNetID].mObjectPtr = SpawnPlayerCharacter();
    mNetIDTable[playerNetID].mControllerPtr = SpawnPlayerCharacterController();
    mNetIDTable[playerNetID].mControllerPtr.Get()->Possess(
        static_cast<ControllableEntity *>(mNetIDTable[playerNetID].mObjectPtr.Get()));

    return playerNetID;
}

bool Core::GameNetworkSystem::TryGetPlayerIDByConnectHandle(NetWork::ConnectHandle connectHandle,
                                                            PlayerNetID &outPlayerID) const
{
    // 클라이언트의 여러 플레이어는 같은 호스트 연결을 공유하므로 역조회는 호스트에서만 가능하다.
    // handle 0은 로컬 호스트에 사용하는 값이며 원격 연결로 취급하지 않는다.
    if (connectHandle == 0 || mNetWorkSystem == nullptr ||
        mNetWorkSystem->GetNetWorkRole() != NetWork::ENetWorkRole::eHost)
        return false;

    for (const auto &player : mNetIDTable)
    {
        if (player.second.mConnectHandle == connectHandle)
        {
            outPlayerID = player.first;
            return true;
        }
    }

    return false;
}

void Core::GameNetworkSystem::AcknowledgePlayerConnect(PlayerNetID newPlayerID)
{

    auto it = mNetIDTable.find(newPlayerID);

    if (it == mNetIDTable.end())
        return;

    it->second.mConnectHandle;

    AcknowledgePlayerConnectPacket packet;
    packet.mPlayerID = newPlayerID;

    std::vector<uint8_t> serializedBuffer;

    packet.Serialize(serializedBuffer);

    NetWork::SendResult sendResult = mNetWorkSystem->Send(it->second.mConnectHandle, serializedBuffer);

    // 다른 기존 플레이어들의 정보 패킷 보내기
    for (auto player : mNetIDTable)
    {
        if (player.first == newPlayerID)
            continue;

        NewPlayerConnectPacket newPlayerConnectPacket;
        newPlayerConnectPacket.mPlayerID = player.first;
        std::vector<uint8_t> serializedBuffer;
        newPlayerConnectPacket.Serialize(serializedBuffer);

        mNetWorkSystem->Send(it->second.mConnectHandle, serializedBuffer);
    }
}

void Core::GameNetworkSystem::BroadCastNewPlayerConnect(PlayerNetID newPlayerID)
{

    NewPlayerConnectPacket newPlayerPacket;
    newPlayerPacket.mPlayerID = newPlayerID;

    std::vector<uint8_t> serializedNewPlayerPacket;
    newPlayerPacket.Serialize(serializedNewPlayerPacket);

    for (auto &player : mNetIDTable)
    {
        if (player.first == newPlayerID || player.first == mLocalPlayerNetID)
        {
            continue;
        }

        NetWork::SendResult sendResult = mNetWorkSystem->Send(player.second.mConnectHandle, serializedNewPlayerPacket);
    }
}

void Core::GameNetworkSystem::SendInputAction(PlayerNetID playerID) {}

void Core::GameNetworkSystem::SendPlayerState()
{
    if (mNetWorkSystem->GetNetWorkRole() != NetWork::ENetWorkRole::eHost)
        return;

    for (auto &e : mNetIDTable)
    {

        Entity *player = static_cast<Entity *>(e.second.mObjectPtr.Get());

        if (player == nullptr)
            continue;

        PlayerStatePacket playerStatePacket;

        playerStatePacket.mPlayerID = e.first;
        playerStatePacket.mPosition = player->GetPositionWorld();
        playerStatePacket.mRotation = player->GetRotationWorld();
        playerStatePacket.mScale = player->GetScaleWorld();

        auto lightCom = player->GetComponent<LightComponent>();
        if (lightCom)
        {
            playerStatePacket.mLightEnabled = lightCom->GetLightEnabled();
        }

        auto animatorCom = player->GetComponent<AnimatorComponent>();

        if (animatorCom)
        {
            Core::AnimPlaybackState animPlaybackState = animatorCom->GetPlayBackState();
            playerStatePacket.mAnimationPlaybackState.mClipID = animPlaybackState.mCurrClip.GetAssetID();
            playerStatePacket.mAnimationPlaybackState.mCurrTime = animPlaybackState.mCurrPlayTime;
            playerStatePacket.mAnimationPlaybackState.mLoop = animPlaybackState.bLoop;
            playerStatePacket.mAnimationPlaybackState.mPause = animPlaybackState.bPause;
        }

        std::vector<uint8_t> serializedBuffer;
        playerStatePacket.Serialize(serializedBuffer);

        for (auto &other : mNetIDTable)
        {
            if (other.second.mConnectHandle == 0)
                continue;

            mNetWorkSystem->Send(other.second.mConnectHandle, serializedBuffer);
        }
    }
}

Core::PlayerNetID Core::GameNetworkSystem::GetNewPlayerNetID()
{
    return mNextPlayerNetID++;
}

void Core::GameNetworkSystem::ProcessAckPlayerConnectPacket(const AcknowledgePlayerConnectPacket &packet,
                                                            NetWork::ConnectHandle hostConnectHandle)
{

    // 호스트쪽에서 연결승낙 및 부여한 playerID가 담긴 packet을 보낸것

    // player id를 이 클라이언트의 local player id로 등록한다.

    mLocalPlayerNetID = packet.mPlayerID;

    mHostConnectHandle = hostConnectHandle;

    mNetIDTable[mLocalPlayerNetID].mConnectHandle = 0;
    mNetIDTable[mLocalPlayerNetID].mControllerPtr = static_cast<PlayerController *>(mMap->GetCurrentObjectController());
    mNetIDTable[mLocalPlayerNetID].mObjectPtr = mMap->GetCurrentObjectController()->GetPossessObject();
}

void Core::GameNetworkSystem::ProcessNewPlayerPacket(const NewPlayerConnectPacket &packet)
{

    // 호스트에서 새로연결된 플레이어에대한 id가 담긴 packet을 보낸것
    // table에 등록은 하는데 connect handl은 어차피 서버 핸들로만 보내느거니깐

    mNetIDTable[packet.mPlayerID].mConnectHandle = mHostConnectHandle;

    auto player = SpawnPlayerCharacter();
    mNetIDTable[packet.mPlayerID].mObjectPtr = player;
}

void Core::GameNetworkSystem::ProcessPlayerActionPacket(const PlayerActionPacket &packet,
                                                        NetWork::ConnectHandle connectHandle)
{

    // 지금 호스트쪽으로만들어오는 packet이 될거고

    PlayerNetID playerID;
    if (!TryGetPlayerIDByConnectHandle(connectHandle, playerID))
    {
        return;
    }

    auto playerObject = mNetIDTable[playerID].mObjectPtr;
    auto playerController = mNetIDTable[playerID].mControllerPtr.Get();

    playerController->PutInputAction(packet.mActionName, {packet.mActionValueType, packet.mActionValue});

    return;
}

void Core::GameNetworkSystem::ProcessPlayerStatePacket(const PlayerStatePacket &packet)
{

    auto playerIDIt = mNetIDTable.find(packet.mPlayerID);
    if (playerIDIt == mNetIDTable.end())
        return;

    auto player = static_cast<Entity *>(playerIDIt->second.mObjectPtr.Get());

    if (player == nullptr)
        return;

    player->SetPositionWorld(packet.mPosition);

    player->SetRotationWorld(packet.mRotation);

    auto lightCom = player->GetComponent<LightComponent>();

    if (lightCom)
    {
        lightCom->SetLightEnabled(packet.mLightEnabled);
    }

    auto animatorCom = player->GetComponent<AnimatorComponent>();
    if (animatorCom)
    {

        Core::AnimPlaybackState animPlaybackState;
        animPlaybackState.mCurrClip = packet.mAnimationPlaybackState.mClipID;
        animPlaybackState.mCurrPlayTime = packet.mAnimationPlaybackState.mCurrTime;
        animPlaybackState.bLoop = packet.mAnimationPlaybackState.mLoop;
        animPlaybackState.bPause = packet.mAnimationPlaybackState.mPause;
        animatorCom->ApplyPlayBackState(animPlaybackState);
    }
}

Core::ObjectPtr<Object> Core::GameNetworkSystem::SpawnPlayerCharacter()
{

    auto player = mGameMode->SpwanPlayerObject(mMap);

    if (player == nullptr)
    {

        return nullptr;
    }
    ObjectPtr<Object> objectPtr(player);

    return objectPtr;
}

Core::ObjectPtr<PlayerController> Core::GameNetworkSystem::SpawnPlayerCharacterController()
{
    auto controller = mGameMode->SpwanPlayerController(mMap);

    if (controller == nullptr)
    {

        return nullptr;
    }
    ObjectPtr<PlayerController> controllerPtr(controller);

    return controllerPtr;
}
