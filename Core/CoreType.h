#pragma once

#include <CoreAsset/IntermediateAsset.h>
#include <CoreBase/Arch.h>
#include <CoreMath/Geometry.h>
#include <InputSystem/InputType.h>
#include <Physics/PhysicsType.h>
#include <RenderSystem/RenderType.h>

namespace Core
{

// 플레이어 오브젝트 타입이  c++클래스타입인지 아니면 프리팹인지여부
enum class ESpawnObjectSourceType
{

    eNone = 0,
    eClass,
    ePrefab
};

struct ObjectSourceCandidate
{
    ESpawnObjectSourceType mType;
    std::string mDisplayName;
    std::string mClassName;
    CoreAsset::AssetID mPrefabID = NoneAssetID;
};

struct GameModeSetting
{
    Core::ESpawnObjectSourceType mDefaultPlayerObjectSource = ESpawnObjectSourceType::eNone;
    std::string mDefaultPlayerObjectClassName;
    CoreAsset::AssetID mDefaultPlayerObjectPrefabID = NoneAssetID;

    std::string mDefaultPlayerControllerClassName = "PlayerController";
};

struct SpawnRequestContext
{
    ESpawnObjectSourceType mSpawnSourceType;
    std::string mClassName;
    CoreAsset::AssetID mPrefabID = NoneAssetID;

    std::string mInstanceName;
    CoreMath::Vector3 mPosition = {0, 0, 0};
    CoreMath::Vector3 mRotation = {0, 0, 0};
    CoreMath::Vector3 mScale = {1, 1, 1};
};

enum class EViewportMode
{
    // 앵커모드 ( 상대적비율- 픽셀모드로 나누어짐)
    eAnchored = 0,
    // 고정모드(위치,사이즈가 픽셀값으로 고정됨)
    eFixed
};

enum class EViewportAnchoredMode
{
    eRelative = 0,
    ePixel
};

// 논리적윈도우(하나의창-하나의vieport와 연결된)별 장치입력에 대한 정보
struct LogicalWindowInputContext
{
    bool mMouseCaptureFlag = false;
};

struct IntermediateMap : public CoreAsset::IntermediateAsset
{
};

struct IntermediatePrefab : public CoreAsset::IntermediateAsset
{
    std::string mClassName;
};

// 전역적인 데이터이자
// 렌더파이프라인에서 pass버퍼를 채우는데 사용되는데이터들, VIEWPORT,등등 pass별 전역데이터들의 모음
struct GlobalFrameData
{
    // viewproj
    CoreMath::Matrix4X4 mViewProj;

    // viewport정보
    Render::Viewport mSceneViewport;

    CoreMath::Vector3 mCameraPositionWorld;

    // 3d world 에서 유효
    CoreMath::Vector3 mAmbientLight;
};

struct InputData
{
    Quad::RawInputData mRawInputData;
    float mNdcX;
    float mNdcY;
    CoreMath::Ray mWorldRay;
};

enum class EObjectFlag : uint32_t
{
    eNone = 0,
    // 저장시 맵에 기록되지않음.
    eTemporary = 1 << 0,

    // 에디터의 오브젝트계층 창에 보이지않음.
    eHidden = 1 << 1,

    // 엔진내부적으로 생성한 객체
    eInternal = 1 << 2,

    // 엔진전용 객체
    eEngineEntity = 1 << 3
};

inline EObjectFlag operator|(EObjectFlag a, EObjectFlag b)
{
    uint32_t ret = (uint32_t)(a) | (uint32_t)(b);
    return (EObjectFlag)ret;
}

enum class EComponentFlag : uint32_t
{
    eNone = 0,
    eEngineAdded = 1 << 0
};

inline EComponentFlag operator|(EComponentFlag a, EComponentFlag b)
{
    uint32_t ret = (uint32_t)(a) | (uint32_t)(b);
    return (EComponentFlag)ret;
}
inline EComponentFlag operator&(EComponentFlag a, EComponentFlag b)
{
    uint32_t ret = (uint32_t)(a) & (uint32_t)(b);
    return (EComponentFlag)ret;
}

enum class EComponentCreationMethod : uint8_t
{
    eNative = 0,    // c++
    eInstancedAdded // editor, prefab, runtime 추가
};

enum class ELightType : uint8_t
{
    eDirectional = 0,
    ePoint,
    eSpot
};

struct AmbientLightSettings
{
    CoreMath::Vector3 mColor = {1.0f, 1.0f, 1.0f};
    float mIntensity = 0.3f;
    bool mEnable = true;
};

struct SkySphereSettings
{
    bool mEnable = true;
    CoreAsset::AssetID mTexID = NoneAssetID;
    CoreMath::Vector3 mTintColor = {1, 1, 1};
    float mIntensity = 1.0f;
    float mRadius = 10000.0f;
};

} // namespace Core
