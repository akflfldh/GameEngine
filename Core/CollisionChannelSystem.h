#pragma once
#include <Core/CoreDllExport.h>
#include <Core/CorePhysicsType.h>
#include <CoreBase/CallbackSystem.h>
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <vector>

class Arch;

using mOnChannelListChangedCallbackSystem = Core::MultiCallbackSystem<>;

namespace Core
{

struct CollisionChannelInfo
{
    // owner name
    std::string mName;
    // 상대방 id - 반응 타입
    std::unordered_map<uint64_t, ECollisionResponseType> mResponseTypeTable;
};

class CORE_API_LIB CollisionChannelSystem
{
  public:
    static CollisionChannelSystem *GetInstance();
    CollisionChannelSystem();
    ~CollisionChannelSystem();

    // 새 프로젝트용 기본 채널과 반응으로 초기화한다. 기존 설정과 ID 발급 상태를 지우므로 로드 후 호출하지 않는다.
    void InitDefault();

    // 이름이 중복되면 실패
    bool CreateNewChannel(const std::string &channelName);

    // 해당 채널이없으면 실패
    bool DeleteChannel(const std::string &channelName);

    bool GetCollisionChannelInfo(const std::string &channelName, CollisionChannelInfo &channelInfo);
    bool GetCollisionChannelInfo(CollisionChannelID id, CollisionChannelInfo &channelInfo);

    bool ChangeChannelResponse(const std::string &channelName, const std::string &targetChannelName,
                               ECollisionResponseType responseType, bool bCallback = true);

    void Serialize(Arch &arch);

    std::vector<std::string> GetAllChannelName() const;
    std::vector<CollisionChannelID> GetAllChannelD() const;
    bool GetChannelID(const std::string &channelName, CollisionChannelID &channelID) const;
    mOnChannelListChangedCallbackSystem mOnChannelListChanged;

    // id 로부터 -> name얻는메서드 , false이면 outName은 DefaultChannelName
    bool GetChannelName(uint64_t channelID, std::string &outName) const;

  private:
    CollisionChannelID GetNewChannelID();

  private:
    CollisionChannelID mNextChannelID = 1;
    // name  - id
    std::unordered_map<std::string, CollisionChannelID> mChannelNameIDTable;

    std::unordered_map<CollisionChannelID, CollisionChannelInfo> mCollisionChannelInfoTable;
};
} // namespace Core
