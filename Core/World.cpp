#include "World.h"
#include <Core/IEngineMode.h>
#include <Core/Map.h>
#include <Core/PhysicsBridgeSystem.h>
#include <Core/RenderIDManager.h>
#include <CoreAsset/AssetManager.h>
#include <algorithm>

World::World()
    : mCurrentMap(nullptr), mCurrentEngineMode(nullptr), mRenderID(Core::RenderIDManager::GetInstance()->AllocID()),
      mPhysicsBridgeSystem(PhysicsBridgeSystem::GetInstance())
{
}

World::~World()
{
    UnRegisterMapAll();

    mOnWorldRemovedCallbackSystem.ExecuteCallbacks();
}

void World::Begin() {}
void World::StartPlay()
{
    if (mCurrentEngineMode == nullptr || mPlayStarted)
        return;

    // 시작 콜백에서 다시 진입해도 세션 초기화를 반복하지 않도록 먼저 상태를 설정한다.
    mPlayStarted = true;
    mCurrentEngineMode->StartPlay();
}

void World::BeginMap()
{

    if (mCurrentMap != nullptr && mCurrentEngineMode != nullptr)
    {
        mCurrentEngineMode->BeginMap(mCurrentMap);
    }
}
void World::EndMap()
{

    if (mCurrentMap != nullptr && mCurrentEngineMode != nullptr)
    {
        mCurrentEngineMode->EndMap(mCurrentMap);
    }

    // 이전맵의 endPlay호출
    /*
    - 이전 플레이어·컨트롤러와 네트워크 참조 정리
- 에디터 플레이 복사본 등 환경별 정리
    */
}
void World::EndPlay()
{
    // 로딩/메뉴처럼 현재 맵이 없는 상태에서도 GameInstance 종료가 필요하다.
    if (mCurrentEngineMode)
        mCurrentEngineMode->EndPlay(mCurrentMap);

    mPlayStarted = false;
}

void World::Update(float DeltaTime)
{
    if (mCurrentMap != nullptr && mCurrentEngineMode != nullptr)
    {
        mCurrentEngineMode->Update(mCurrentMap, DeltaTime);
        // mCurrentMap->Update(DeltaTime);
    }
}
void World::EndUpdate(float DeltaTime)
{
    if (mCurrentMap != nullptr && mCurrentEngineMode != nullptr)
    {
        mCurrentEngineMode->EndUpdate(mCurrentMap, DeltaTime);
    }
}

void World::CleanUp()
{
    if (mCurrentEngineMode)
        mCurrentEngineMode->CleanUp(mCurrentMap);
}

void World::EndFrame()
{

    ChangeMapIfRequested();
}

bool World::SetCurrentMap(const std::string &name)
{

    std::unordered_map<std::string, Map *>::iterator it = mMapTable.find(name);
    if (it == mMapTable.end())
        return false;

    if (mCurrentMap != it->second)
    {
        if (mCurrentMap)
            mCurrentMap->ClearCallbackSystems();

        mCurrentMap = it->second;

        // 기존 맵과 프록시는 유지하되, 일반 맵 전환에서 이전 맵이 계속 그려지지 않도록 참여 목록만 교체한다.
        // 오버레이나 추가 지역 맵은 호출 측에서 AddRenderingMap으로 다시 포함한다.
        mRenderingMaps.clear();
        mRenderingMaps.push_back(mCurrentMap);

        mCurrentMap->mObjectRemovedCallbackSystem.Register(
            [this](Object *object) { mOnMapObjectRemovedCallbackSystem.ExecuteCallbacks(object); });
    }

    return true;
}

bool World::SetCurrentMap(Map *map)
{

    if (map == nullptr)
        return false;

    return SetCurrentMap(map->GetName().c_str());
}

Map *World::GetCurrentMap() const
{
    return mCurrentMap;
}

bool World::AddRenderingMap(Map *map)
{
    if (map == nullptr || map->GetWorld() != this)
        return false;

    // 등록 여부는 표시 이름이 아니라 인스턴스로 확인한다. 같은 맵을 중복 포함해 두 번 그리지 않는다.
    const auto registered =
        std::find_if(mMapTable.begin(), mMapTable.end(), [map](const auto &entry) { return entry.second == map; });
    if (registered == mMapTable.end())
        return false;

    if (std::find(mRenderingMaps.begin(), mRenderingMaps.end(), map) == mRenderingMaps.end())
        mRenderingMaps.push_back(map);
    return true;
}

void World::RemoveRenderingMap(Map *map)
{
    mRenderingMaps.erase(std::remove(mRenderingMaps.begin(), mRenderingMaps.end(), map), mRenderingMaps.end());
}

const std::vector<Map *> &World::GetRenderingMaps() const
{
    return mRenderingMaps;
}

ObjectController *World::GetCurrentObjectController() const
{

    if (mCurrentEngineMode == nullptr)
        return nullptr;

    return mCurrentEngineMode->GetCurrentObjectController(mCurrentMap);
}

void World::AddPrefabObject(Object *object)
{

    if (mCurrentMap)
    {
        mCurrentMap->AddPrefabObject(object);
    }
}

void World::RemovePrefabObject(Object *object)
{
    if (mCurrentMap)
    {
        mCurrentMap->RemovePrefabObject(object);
    }
}

bool World::Register(Map *map)
{

    if (map == nullptr)
        return false;

    std::pair<std::unordered_map<std::string, Map *>::iterator, bool> ret =
        mMapTable.insert({map->GetName().c_str(), map});

    if (ret.second == false)
        return false;

    map->SetWorld(this);

    mPhysicsBridgeSystem->RegisterMap(map);

    return ret.second;
}

void World::OnInputEvent(const Core::InputData &inputData)
{

    // 현재 맵에 전달
    if (mCurrentMap)
    {
        mCurrentMap->OnInputEvent(inputData);
    }
}

CameraComponent *World::GetCurrentCameraCom() const
{
    if (mCurrentEngineMode)
    {
        return mCurrentEngineMode->GetActiveCameraComponent(mCurrentMap);
    }

    return nullptr;
}

void World::SetEngineMode(Core::IEngineMode *mode)
{

    mCurrentEngineMode = mode;
}

Core::IEngineMode *World::GetEngineMode() const
{
    return mCurrentEngineMode;
}
uint32_t World::GetRenderID() const
{
    return mRenderID;
}
void World::SetActiveState(bool flag)
{

    mActiveState = flag;
}
bool World::GetActiveState() const
{

    return mActiveState;
}

void World::UnRegisterMapAll()
{
    mRenderingMaps.clear();
    for (auto e : mMapTable)
    {
        Map *map = e.second;

        if (map)
        {
            mPhysicsBridgeSystem->UnRegisterMap(map);
        }
    }

    mMapTable.clear();
    mCurrentMap = nullptr;
}

void World::SetPause()
{

    if (mCurrentEngineMode)
    {
        mCurrentEngineMode->SetPause();
    }
}

void World::ReleasePause()
{

    if (mCurrentEngineMode)
    {
        mCurrentEngineMode->ReleasePause();
    }
}

CoreMath::Vector3 World::GetAmbientLight() const
{

    if (mCurrentMap)
    {
        const auto &ambientLightSettings = mCurrentMap->GetAmbientLightSettings();
        if (ambientLightSettings.mEnable == false)
            return {0, 0, 0};
        else
        {
            return ambientLightSettings.mColor * ambientLightSettings.mIntensity;
        }
    }

    return {0, 0, 0};
}

PhysicsBridgeSystem *World::GetPhysicsBridgeSystem() const
{
    return mPhysicsBridgeSystem;
}

Core::MouseMode World::GetMouseMode() const
{
    if (mCurrentMap == nullptr)
        return Core::MouseMode::Free;

    return mCurrentEngineMode->GetMouseMode(mCurrentMap);
}

std::vector<Map *> World::GetMapList() const
{
    std::vector<Map *> mapList;

    for (auto entry : mMapTable)
    {
        mapList.push_back(entry.second);
    }

    return mapList;
}

std::vector<std::string> World::GetMapNameList() const
{
    std::vector<std::string> mapList;

    for (auto entry : mMapTable)
    {
        mapList.push_back(entry.second->GetName().c_str());
    }

    return mapList;
}

void World::RequestChangeMap(const std::string &mapName)
{

    mRequestMapChange = true;

    mRequestedMapName = mapName;
}

Object *World::CreateEntity(const char *entityClassName, const char *entityInstanceName)
{

    if (mCurrentMap == nullptr)
        return nullptr;

    return mCurrentMap->CreateEntity(entityClassName, entityInstanceName);
}

void World::ChangeMapIfRequested()
{

    if (!mRequestMapChange)
        return;

    auto it = mMapTable.find(mRequestedMapName);
    if (it == mMapTable.end())
        return;

    auto nextMap = it->second;

    // 변경하면 engine mode한테 알려야하나 ?  어차피 editorPlaymode랑 runtime mode에서만쓰는데  조금연결을 봐야할듯

    // 이것도 world에서 담당해줘야하나? 원래는 에셋인데
    if (nextMap->GetLoadState() != CoreAsset::EAssetLoadState::Loaded)
    {
        auto assetManager = CoreAsset::AssetManager::GetInstance();
        assetManager->LoadAssetRawData(nextMap);
    }

    // 기존 맵 종료
    EndMap();

    SetCurrentMap(nextMap);
    BeginMap(); // 여기서하던가 아니면 이후 앞에서 새로운 MAP에대한 설정이나 이런건 프레임맨앞에서하던가 그러면 렌더까지
                // 안전할수도? 아닌가 이미렌더는 SNAPSHOT이라서 안전하나

    mRequestMapChange = false;

    mOnMapChangedCallbackSystem.ExecuteCallbacks(nextMap);
}