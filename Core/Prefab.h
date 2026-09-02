#pragma once

#include <CoreAsset/Asset.h>
#include <CoreMath/CoreMath.h>

#include <Core/CoreDllExport.h>

class Object;
class Map;
class Component;

class CORE_API_LIB Prefab : public CoreAsset::Asset
{
  public:
    Prefab();
    virtual ~Prefab();
    static CoreAsset::EAssetType GetAssetType()
    {
        return CoreAsset::EAssetType::ePrefab;
    }
    Object *mDefaultObject;
    virtual void Serialize(Arch &arch) override;

    Object *Instantiate(Map *map, const char *instanceName = "");

    Component *AddComponent(const std::string &componentClassName);

    // 생성직후,로드직후 프리팹의 디폴트오브젝트의 컴포넌트들에대해서 프리팹키설정을한다.
    void EnsureDefaultComponentPrefabKey();

    // entity 일경우 성공 (매프레임 호출하는경우라면 직접 Object에 접근해서 설정)
    void SetPositionWorld(const CoreMath::Vector3 &pos);

  private:
    void Serialize_Object(Arch &arch, Object *&object);
    // void SetPrefabContextToInstanceComponents(Object *object);
};
