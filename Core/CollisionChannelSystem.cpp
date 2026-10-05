#include "CollisionChannelSystem.h"
#include <CoreBase/Arch.h>

Core::CollisionChannelSystem *Core::CollisionChannelSystem::GetInstance()
{

    static CollisionChannelSystem instance;
    return &instance;
}

Core::CollisionChannelSystem::CollisionChannelSystem() {}

Core::CollisionChannelSystem::~CollisionChannelSystem() {}

void Core::CollisionChannelSystem::InitDefault()
{
    // ID는 프로젝트 범위다. 새 프로젝트 생성에만 사용하며, 기존 프로젝트의 삭제된 ID를 재사용하는 용도가 아니다.
    mChannelNameIDTable.clear();
    mCollisionChannelInfoTable.clear();
    mNextChannelID = 1;

    constexpr const char *channelNames[] = {"Default", "WorldStatic", "Player", "Pickup"};
    for (const auto channelName : channelNames)
        CreateNewChannel(channelName);

    using Response = ECollisionResponseType;
    // 행은 자기 채널, 열은 상대 채널이다. Pickup은 Player와만 Overlap하며 바닥/일반 물체에는 반응하지 않는다.
    // BodyType은 여기서 설정하지 않는다. WorldStatic 이름만으로 Static body가 되는 것은 아니다.
    constexpr Response responses[4][4] = {
        {Response::eBlock, Response::eBlock, Response::eBlock, Response::eIgnore},
        {Response::eBlock, Response::eBlock, Response::eBlock, Response::eIgnore},
        {Response::eBlock, Response::eBlock, Response::eBlock, Response::eOverlap},
        {Response::eIgnore, Response::eIgnore, Response::eOverlap, Response::eIgnore}};

    for (int channelIndex = 0; channelIndex < 4; ++channelIndex)
    {
        for (int targetIndex = 0; targetIndex < 4; ++targetIndex)
            ChangeChannelResponse(channelNames[channelIndex], channelNames[targetIndex],
                                  responses[channelIndex][targetIndex], false);
    }
}

bool Core::CollisionChannelSystem::CreateNewChannel(const std::string &channelName)
{

    auto it = mChannelNameIDTable.find(channelName);

    if (it != mChannelNameIDTable.end())
        return false;

    uint64_t newID = GetNewChannelID();

    mChannelNameIDTable[channelName] = newID;
    mCollisionChannelInfoTable[newID].mName = channelName;

    for (auto &entry : mChannelNameIDTable)
    {
        mCollisionChannelInfoTable[newID].mResponseTypeTable[entry.second] = {};
        mCollisionChannelInfoTable[entry.second].mResponseTypeTable[newID] = ECollisionResponseType::eIgnore;
    }

    mOnChannelListChanged.ExecuteCallbacks();

    return true;
}

// 해당 채널이없으면 실패
bool Core::CollisionChannelSystem::DeleteChannel(const std::string &channelName)
{

    auto it = mChannelNameIDTable.find(channelName);

    if (it == mChannelNameIDTable.end())
        return false;

    uint64_t id = it->second;
    mChannelNameIDTable.erase(it);

    mCollisionChannelInfoTable.erase(id);
    for (auto &entry : mCollisionChannelInfoTable)
    {
        entry.second.mResponseTypeTable.erase(id);
    }

    mOnChannelListChanged.ExecuteCallbacks();
    return true;
}

bool Core::CollisionChannelSystem::GetCollisionChannelInfo(const std::string &channelName,
                                                           CollisionChannelInfo &channelInfo)
{

    auto it = mChannelNameIDTable.find(channelName);

    if (it == mChannelNameIDTable.end())
        return false;

    channelInfo = mCollisionChannelInfoTable[it->second];
    return true;
}

bool Core::CollisionChannelSystem::GetCollisionChannelInfo(CollisionChannelID id, CollisionChannelInfo &channelInfo)
{

    auto it = mCollisionChannelInfoTable.find(id);

    if (it == mCollisionChannelInfoTable.end())
        return false;

    channelInfo = mCollisionChannelInfoTable[id];

    return true;
}

bool Core::CollisionChannelSystem::ChangeChannelResponse(const std::string &channelName,
                                                         const std::string &targetChannelName,
                                                         ECollisionResponseType responseType, bool bCallback)
{
    auto sIt = mChannelNameIDTable.find(channelName);

    if (sIt == mChannelNameIDTable.end())
        return false;

    auto tIt = mChannelNameIDTable.find(targetChannelName);

    if (tIt == mChannelNameIDTable.end())
        return false;

    mCollisionChannelInfoTable[sIt->second].mResponseTypeTable[tIt->second] = responseType;

    if (bCallback)
        mOnChannelListChanged.ExecuteCallbacks();

    return true;
}

void Core::CollisionChannelSystem::Serialize(Arch &arch)
{
    arch << mNextChannelID;
    uint64_t channelNum = mCollisionChannelInfoTable.size();
    arch << channelNum;

    if (arch.GetLoadingFlag())
    {
        std::unordered_map<std::string, uint64_t> channelNameIDTable;
        std::unordered_map<uint64_t, CollisionChannelInfo> collisionChannelInfoTable;

        for (uint64_t i = 0; i < channelNum; ++i)
        {
            uint64_t id;
            std::string name;

            arch << id << name;

            channelNameIDTable[name] = id;
            collisionChannelInfoTable[id].mName = name;
            for (int j = 0; j < channelNum; ++j)
            {
                uint64_t otherID;
                ECollisionResponseType responseType;

                arch << otherID << responseType;

                collisionChannelInfoTable[id].mResponseTypeTable[otherID] = responseType;
            }
        }

        mChannelNameIDTable = std::move(channelNameIDTable);
        mCollisionChannelInfoTable = std::move(collisionChannelInfoTable);

        mOnChannelListChanged.ExecuteCallbacks();
    }
    else
    {

        for (auto &entry : mCollisionChannelInfoTable)
        {
            uint64_t id = entry.first;
            std::string name = entry.second.mName;
            arch << id << name;

            for (auto &channelInfoEntry : entry.second.mResponseTypeTable)
            {
                uint64_t otherID = channelInfoEntry.first;
                ECollisionResponseType responseType = channelInfoEntry.second;
                arch << otherID << responseType;
            }
        }
    }
}

std::vector<std::string> Core::CollisionChannelSystem::GetAllChannelName() const
{
    std::vector<std::string> channelNames;

    for (const auto &entry : mChannelNameIDTable)
    {
        channelNames.push_back(entry.first);
    }
    return channelNames;
}

std::vector<Core::CollisionChannelID> Core::CollisionChannelSystem::GetAllChannelD() const
{
    std::vector<Core::CollisionChannelID> idList;

    for (const auto &entry : mChannelNameIDTable)
    {
        idList.push_back(entry.second);
    }
    return idList;
}

bool Core::CollisionChannelSystem::GetChannelName(uint64_t channelID, std::string &outName) const
{

    auto it = mCollisionChannelInfoTable.find(channelID);

    if (it == mCollisionChannelInfoTable.end())
    {
        outName = mCollisionChannelInfoTable.at(1).mName; // default;
        return false;
    }
    outName = it->second.mName;

    return true;
}

Core::CollisionChannelID Core::CollisionChannelSystem::GetNewChannelID()
{
    return mNextChannelID++;
}

bool Core::CollisionChannelSystem::GetChannelID(const std::string &channelName, CollisionChannelID &channelID) const
{
    const auto it = mChannelNameIDTable.find(channelName);
    if (it == mChannelNameIDTable.end())
    {
        channelID = 0;
        return false;
    }
    channelID = it->second;
    return true;
}
