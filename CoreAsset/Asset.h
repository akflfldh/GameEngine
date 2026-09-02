#pragma once

#include "CoreAsset/AssetType.h"
// #include <CoreBase/BaseClass.h>
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
template <typename T> struct AssetName; // 기본 템플릿 (정의는 없어도 됨)

#define AssetClassName(AssetClass)                                                                                     \
    template <> struct AssetName<AssetClass>                                                                           \
    {                                                                                                                  \
        static const char *Get()                                                                                       \
        {                                                                                                              \
            return #AssetClass;                                                                                        \
        }                                                                                                              \
    };

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
    friend class AssetIOManager;

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

    static EAssetType GetAssetType()
    {
        return EAssetType::eUnknown;
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

    void SetEmptyAssetFlag(bool flag);

    void SetRawDataDirty(bool flag);
    bool GetRawDataDirty() const;

  protected:
    // 생성자를 protected로 두어 파생 클래스만 생성 가능하도록 제한하는 것이 일반적
    Asset(EAssetType type, AssetID id = NoneAssetID);

    // Loader나 AssetManager에서 이 값들을 설정할 수 있도록 setter 제공
    // 또는 friend 클래스로 접근 허용
    void SetLoadState(LoadState state)
    {
        mLoadState = state;
    }

  private:
    void SetAssetID(AssetID id);

  private:
    AssetID mID; // 정수형 uniqueID
    FString mName;
    std::string mTag;

    EAssetType mType;
    LoadState mLoadState = LoadState::Unloaded;

    // Dirty flag가 켜졌다면 저장시 write될것이다.
    bool mDirtyFlag;

    // RawData DirtyFlag ,
    bool mRawDirtyFlag;

    // 데이터가 온전히 채워지지않은 빈 에셋인지의 여부
    bool mIsEmpty;
};

AssetClassName(Asset)

} // namespace CoreAsset
