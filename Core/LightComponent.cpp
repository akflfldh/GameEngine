#include "LightComponent.h"
#include <Core/Map.h>
#include <Core/Object.h>
LightComponent::LightComponent()
    : mStrength({1.0f, 1.0f, 1.0f}), mLightType(Core::ELightType::eDirectional),
      mLightProxy(std::make_unique<Core::LightProxy>())
{
}

LightComponent::~LightComponent()
{
    if (Object *object = GetOwnerObject())
    {
        if (Map *map = object->GetMap())
        {
            map->UnRegisterLightComponent(this);
        }
    }
    Core::IRenderProxyManager::GetInstance()->UnRegisterLightProxy(mLightProxy.get());
}

void LightComponent::SetLightType(Core::ELightType type)
{

    mLightType = type;
    // mLightProxy->mLightType = mLightType;
    MarkPropertyDirty();
}

Core::ELightType LightComponent::GetLightType() const
{
    return mLightType;
}

void LightComponent::SetLightEnabled(bool enabled)
{
    if (mLightEnabled == enabled)
        return;

    mLightEnabled = enabled;
    MarkPropertyDirty();

    // 조명 토글은 등록/해제를 반복하지 않고 프록시의 발광 상태만 바꾼다.
    // EndTick 이전에도 렌더 명령 생성이 최신 켜짐 상태를 읽을 수 있도록 즉시 전달한다.
    if (mLightProxy)
        mLightProxy->mLightEnabled = mLightEnabled;
}

bool LightComponent::GetLightEnabled() const
{
    return mLightEnabled;
}

CoreMath::Vector3 LightComponent::GetStrength() const
{
    return mStrength;
}

float LightComponent::GetFalloffStart() const
{
    return mFalloffStart;
}

float LightComponent::GetFalloffEnd() const
{
    return mFalloffEnd;
}

float LightComponent::GetSpotPower() const
{
    return mSpotPower;
}

void LightComponent::SetStrength(const CoreMath::Vector3 &strength)
{

    mStrength = strength;
    // if (mLightProxy)
    //{
    //     mLightProxy->mStrength = strength;
    // }
    MarkPropertyDirty();
}

void LightComponent::SetFalloffStart(float value)
{

    mFalloffStart = value;
    // if (mLightProxy)
    //{
    //     mLightProxy->mFalloffStart = mFalloffStart;
    // }
    MarkPropertyDirty();
}

void LightComponent::SetFalloffEnd(float value)
{

    mFalloffEnd = value;
    /*  if (mLightProxy)
      {
          mLightProxy->mFalloffEnd = mFalloffEnd;
      }*/
    MarkPropertyDirty();
}

void LightComponent::SetSpotPower(float value)
{

    mSpotPower = value;

    /* if (mLightProxy)
     {
         mLightProxy->mSpotPower = value;
     }*/

    MarkPropertyDirty();
}

void LightComponent::OnTransformChanged()
{

    SceneComponent::OnTransformChanged();

    if (Object *Onwer = GetOwnerObject())
    {
        // 아쉬운점은 OnTransformChange 될때마다 position을설정하기위해서
        // positionWorld를 가져오니 트랜스폼의 지연계산이 효과가 없어져버린다.
        // 만약 set만있다면 마지막에 딱한번 get에서 proxy를 업데이트하면되는데
        /*  mLightProxy->mPosition = GetPositionWorld();
          mLightProxy->mDirection = GetForwardWorld();
          mLightProxy->mRight = GetRightWorld();*/

        UpdateProxy();
    }
}

void LightComponent::OnOwnerObjectAddedToMap()
{

    SceneComponent::OnOwnerObjectAddedToMap();

    if (Object *Onwer = GetOwnerObject())
    {
        if (Map *map = Onwer->GetMap())
        {
            mLightProxy->mRenderID = map->GetRenderID();
            map->RegisterLightComponent(this);
        }
        UpdateProxy();

        if (Onwer->GetActive())
            Core::IRenderProxyManager::GetInstance()->RegisterLightProxy(mLightProxy.get());
    }
}

void LightComponent::OnActiveStateChanged(bool state)
{
    if (state)
    {
        UpdateProxy();
        Core::IRenderProxyManager::GetInstance()->RegisterLightProxy(mLightProxy.get());
    }
    else
    {
        Core::IRenderProxyManager::GetInstance()->UnRegisterLightProxy(mLightProxy.get());
    }
}

void LightComponent::FlushPropertyDirty()
{
    UpdateProxy();
}

void LightComponent::Serialize(Arch &arch)
{

    SceneComponent::Serialize(arch);

    arch << mLightType;
    arch << mStrength;
    arch << mFalloffStart;
    arch << mFalloffEnd;
    arch << mSpotPower;
}

void LightComponent::EndTick(float deltaTime)
{

    SceneComponent::EndTick(deltaTime);

    if (GetPropertyDirty())
        UpdateProxy();
}

void LightComponent::UpdateProxy()
{
    if (mLightProxy)
    {
        mLightProxy->mLightEnabled = mLightEnabled;
        mLightProxy->mPosition = GetPositionWorld();
        mLightProxy->mDirection = GetForwardWorld();
        mLightProxy->mRight = GetRightWorld();
        mLightProxy->mUp = GetUpWorld();
        mLightProxy->mFalloffEnd = mFalloffEnd;
        mLightProxy->mFalloffStart = mFalloffStart;
        mLightProxy->mSpotPower = mSpotPower;
        mLightProxy->mStrength = mStrength;
        mLightProxy->mLightType = mLightType;

        ClearPropertyDirty();
    }
}

void LightComponent::OnOwnerObjectRemovedFromMap()
{

    if (Map *map = GetMap())
    {
        map->UnRegisterLightComponent(this);
    }

    SceneComponent::OnOwnerObjectRemovedFromMap();
}
