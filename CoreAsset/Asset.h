#pragma once

#include "CoreAsset/AssetType.h"
#include <CoreBase/FString.h>
#include <stdint.h>
#include <string>

#include "CoreAsset/CoreAssetDLLMacro.h"

class Arch;

namespace QuadRW
{
class BinaryWriter;
class BinaryReader;
} // namespace QuadRW

namespace CoreAsset
{
struct AssetHeaderContext
{
    AssetID mID; // 정수형 uniqueID
    EAssetType mType;

    CORE_ASSET_API void Serialize(QuadRW::BinaryWriter &binaryWriter);
    CORE_ASSET_API void DeSerialize(QuadRW::BinaryReader &binaryReader);
};

class CORE_ASSET_API Asset
{
    friend class AssetManager;
    friend class GlobalAssetRegistrySystem;

  public:
    virtual ~Asset();
    Asset(const Asset &) = delete;
    Asset &operator=(const Asset &) = delete;

    AssetID GetID() const
    {
        return mID;
    }
    const FString &GetName() const;
    EAssetType GetType() const
    {
        return mType;
    }

    enum class LoadState
    {
        Unloaded,
        Loading,
        Loaded,
        Failed
    };
    LoadState GetLoadState() const
    {
        return mLoadState;
    }

    void SetTag(const std::string &tag);
    const std::string &GetTag() const;

    void SetDirty();
    bool GetDirty() const;

    // 오직 GlobalAssetRegistrySystem만호출한다.
    void ClearDirty();

    virtual void Serialize(Arch &arch);
    virtual void Serialize(QuadRW::BinaryWriter &writer) const;

    bool IsEmptyAsset() const;

    void SetName(const FString &name);

  protected:
    // 생성자를 protected로 두어 파생 클래스만 생성 가능하도록 제한하는 것이 일반적
    Asset(EAssetType type, AssetID id = NoneAssetID);

    // Loader나 AssetManager에서 이 값들을 설정할 수 있도록 setter 제공
    // 또는 friend 클래스로 접근 허용
    void SetLoadState(LoadState state)
    {
        mLoadState = state;
    }

    void SetEmptyAssetFlag(bool flag);

  private:
    AssetID mID; // 정수형 uniqueID
    FString mName;
    std::string mTag;

    EAssetType mType;
    LoadState mLoadState;

    // Dirty flag가 켜졌다면 저장시 write될것이다.
    bool mDirtyFlag;

    // 데이터가 온전히 채워지지않은 빈 에셋인지의 여부
    bool mIsEmpty;
};

} // namespace CoreAsset
