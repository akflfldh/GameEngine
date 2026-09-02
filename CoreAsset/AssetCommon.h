#pragma once

// loader,storer가 읽고 쓰는 데이터타입
#include "CoreAsset/AssetType.h"
#include "CoreAsset/CoreAssetDLLMacro.h"
#include <stdint.h>
#include <string>
#include <vector>

namespace CoreAsset
{
// 가공된 에셋데이터+메타데이터 파일 확장자
// asset(앞의 .은 포함하지않는다)
CORE_ASSET_API const char *GetAssetFileExtension();
CORE_ASSET_API const char *GetAssetDotFileExtension();

// 헬퍼함수
CORE_ASSET_API const char *GetAssetFileExtension(EAssetType assetType);

CORE_ASSET_API const char *GetMapFileExtension();

// 순수 에셋데이터파일 확장자(즉 에디터의 메타데이터가없는)
// raw
CORE_ASSET_API const char *GetAssetRawFileExtension();

class Asset;

CORE_ASSET_API std::string GetAssetFileName(Asset *asset); // name + .asaset;

// Root , Asset/Folder1/Folder2/assetName    =>   Root.Asset.Folder1.Folder2.assetName.raw 를 돌려준다
CORE_ASSET_API std::string GetAssetRawFileName(const std::string &logicalPathToken); // name + .raw;

} // namespace CoreAsset