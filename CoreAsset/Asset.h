#pragma once



#include<stdint.h>
#include<string>
#include"AssetType.h"

#include"CoreAssetDLLMacro.h"

namespace QuadRW { class BinaryWriter; }


namespace CoreAsset
{



	class CORE_ASSET_API Asset
	{
	public:
		virtual ~Asset();
		Asset(const Asset&) = delete;
		Asset& operator=(const Asset&) = delete;



		AssetID GetID() const { return mID; }
		const std::string& GetName() const { return mName; }
		EAssetType GetType() const { return mType; }


		enum class LoadState {
			Unloaded,
			Loading,
			Loaded,
			Failed
		};
		LoadState GetLoadState() const { return mLoadState; }

		void SetTag(const std::string& tag);
		const std::string& GetTag() const;


		
		void SetDirty();
		void ClearDirty();
		bool GetDirty()const;




		virtual void Serialize(QuadRW::BinaryWriter & writer) const;





	protected:
		// 생성자를 protected로 두어 파생 클래스만 생성 가능하도록 제한하는 것이 일반적
		Asset(AssetID id, const std::string& name, EAssetType type)
			: mID(id),  mName(name), mTag(""), mType(type), mLoadState(LoadState::Unloaded) , mDirtyFlag(false)
		{
		}

		// Loader나 AssetManager에서 이 값들을 설정할 수 있도록 setter 제공
		// 또는 friend 클래스로 접근 허용
		void SetLoadState(LoadState state) { mLoadState = state; }


	private:
		AssetID mID;//정수형 uniqueID
		std::string mName;
		std::string mTag;

		EAssetType mType;
		LoadState mLoadState;


		//Dirty flag가 켜졌다면 저장시 write될것이다.
		bool mDirtyFlag;

	};

}
