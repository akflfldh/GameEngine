#pragma once

#include "CoreAsset/AssetType.h"

#include "CoreAsset/CoreAssetDLLMacro.h"
#include <vector>
namespace CoreAsset
{
class CORE_ASSET_API AssetIDGenerator
{
  public:
    AssetIDGenerator();
    ~AssetIDGenerator();

    // 이메서드 호출시 ID를 소비한다.
    AssetID GetNewAssetID();

    // 다음에 제공될 ID를 확인만 한다.
    AssetID PeekNextAssetID() const;

    // 중복해서반납하는지검사하지않기에 사용자측에서 올바르게사용할것
    void ReleaseAssetID(AssetID id);

    // 초기 설정시에 사용할 메서드
    void SetNextAssetID(AssetID id);

  private:
    AssetID mNextNewAssetID;

    // 비어있지않다면 다음의 NewAssetID는 벡터끝에서 가져온다.
    std::vector<AssetID> mFreeIDVector;
};
} // namespace CoreAsset
