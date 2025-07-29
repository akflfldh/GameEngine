#pragma once

#include"AssetType.h"


#include<vector>
#include"CoreAssetDLLMacro.h"
namespace CoreAsset
{
	class CORE_ASSET_API AssetIDGenerator
	{
	public:
		AssetIDGenerator();
		~AssetIDGenerator();

		AssetID GetNewAssetID();

		//중복해서반납하는지검사하지않기에 사용자측에서 올바르게사용할것
		void ReleaseAssetID(AssetID id);


	private:
		AssetID mNextNewAssetID;

		//비어있지않다면 다음의 NewAssetID는 벡터끝에서 가져온다.
		std::vector<AssetID> mFreeIDVector;




	};
}
