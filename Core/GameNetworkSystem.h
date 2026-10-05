#pragma once
#include <Core/CoreDllExport.h>
#include <Core/GameNetworkType.h>
#include <NetWork/NetWorkType.h>
#include <unordered_map>

#include <Core/ObjectPtr.h>

namespace NetWork
{
class NetWorkSystem;
}
class GameMode;
class Map;
class ObjectController;
class PlayerController;

namespace Core
{
struct NetworkPlayer
{
    NetWork::ConnectHandle mConnectHandle;
    ObjectPtr<Object> mObjectPtr;
    ObjectPtr<PlayerController> mControllerPtr; // host쪽에서만 생성해서 각 플레이어들에대해  들고있을거고
    // 클라이언트들은 자기자신의 controller외에 다른플레이어들은 그냥object만
};

class CORE_API_LIB GameNetworkSystem
{
  public:
    static GameNetworkSystem *GetInstance();
    GameNetworkSystem();
    ~GameNetworkSystem();

    bool Initialize();
    void BindPlayContext(GameMode *gameMode, Map *map);
    void UnbindPlayContext();


    //새로운 MAP 시작될때마다 호출 
    void BeginPlay();
    void Update();
    void EndUpdate();

    PlayerNetID GetLocalPlayerID() const;

    bool StartHost();
    bool StartClient();

  private:
    PlayerNetID AcceptNewPlayer(NetWork::ConnectHandle connectHandle);

    // 호스트에 등록된 원격 연결의 플레이어를 조회한다. 실패하면 출력값을 유지한다.
    bool TryGetPlayerIDByConnectHandle(NetWork::ConnectHandle connectHandle, PlayerNetID &outPlayerID) const;

    // host로의 연결요청을한 새로운 player의 연결승인-playerID 전달 등등
    void AcknowledgePlayerConnect(PlayerNetID newPlayerID);

    // 새로운 player말고 다른 player들에게 새로운 player의 연결을 알린다.
    void BroadCastNewPlayerConnect(PlayerNetID newPlayerID);

    void SendInputAction(PlayerNetID playerID);
    void SendPlayerState();

    PlayerNetID GetNewPlayerNetID();

    void ProcessAckPlayerConnectPacket(const AcknowledgePlayerConnectPacket &packet,
                                       NetWork::ConnectHandle hostConnectHandle);
    void ProcessNewPlayerPacket(const NewPlayerConnectPacket &packet);

    void ProcessPlayerActionPacket(const PlayerActionPacket &packet, NetWork::ConnectHandle connectHandle);

    void ProcessPlayerStatePacket(const PlayerStatePacket &packet);

    ObjectPtr<Object> SpawnPlayerCharacter();
    ObjectPtr<PlayerController> SpawnPlayerCharacterController();

  private:
    NetWork::NetWorkSystem *mNetWorkSystem;
    NetWork::ConnectHandle mHostConnectHandle;

    std::unordered_map<PlayerNetID, NetworkPlayer> mNetIDTable;

    PlayerNetID mNextPlayerNetID = 1;

    PlayerNetID mLocalPlayerNetID = 0;

    GameMode *mGameMode = nullptr;
    Map *mMap = nullptr;
};

} // namespace Core
