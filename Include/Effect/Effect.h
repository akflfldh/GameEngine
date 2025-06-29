#pragma once

#include"Predeclare.h"
#include<vector>
#include<unordered_map>
#include"Utility/UniqueIDTable.h"

#include<string>
#include"Asset/Texture/TextureFormat.h"
#include"CoreDllExport.h"


namespace Quad
{

	

	class CORE_API_LIB Effect
	{

		friend class ResourceController;
		friend class RenderSystem;
		
		friend class EffectManager;
		friend class UniqueIDTable<Effect>;
		friend class EffectLoader;
		friend class CoreEffectInstaller;


	public:
		//Effect(const std::string& name="");
		Effect();

		~Effect();
		
		Effect(const Effect& effect);

		const std::vector<RenderPassTwo*>& GetStaticRenderPassVector() const;
		const std::vector<RenderPassTwo*>& GetSkinningRenderPassVector() const;
		const std::string& GetName() const;

		unsigned long long GetUniqueID() const;

		virtual void Serialize();
		virtual void DeSerialize();

		void AddCustomTexture(ETextureType textureType, std::pair<float,float>resizeRate);
		unsigned int GetCustomTextureNum() const;

		const std::vector<std::pair<float, float>>& GetCustomTextureResizeRateVector()const;
		const std::vector<unsigned long long>& GetCustomTextureIDVector() const;

		const std::vector<ETextureType>& GetCustomTextureTypeVector() const;


		void SetCustomTextureIDVector(std::vector<unsigned long long>&& customTextureIDVector);



		//이함수는 엔진에서만 사용
		void SetCustomTextureVector(std::vector<Texture*>&& customTextureVector);

		const std::vector<Texture*>& GetCustomTextureVector() const;

		void OnResize(unsigned int clientWidth, unsigned int clientHeight);

		void ResetRenderPassShaderResource();

	private:
		void SetStaticMeshRenderPassVector(std::vector<RenderPassTwo*> && renderPassVector);
		void SetSkinningMeshRenderPassVector(std::vector<RenderPassTwo*> && renderPassVector);


	private:
		void SetUniqueID(unsigned long long id);

		std::string mName;
		std::vector<RenderPassTwo *> mStaticMeshRenderPassVector;
		std::vector<RenderPassTwo *> mSkinningMeshRenderPassVector;



		unsigned long long mID;



		//이 effect에서 사용할 렌더타켓의개수 
		//추가되는 렌더타켓은 0번부터 시작하는 id를 가진다. 
		//따라서 렌더패스에 사용할  렌더타켓의 적절한 번호를 지정하면된다. 
		unsigned int mCustomTextureNum;		//
		
		std::vector<unsigned long long > mCustomTextureIDVector;
		std::vector< ETextureType> mCustonTextureTypeVector;
		std::vector<Texture*> mCustomTextureVector;

		//텍스처(일반 텍스처 , 렌더타겟용 텍스처 )들이 창의 크기가변할때 조정해야하는 전체창크기에대한 비율
		// (0,0) : 고정 / (1,1) : 항상 전체 /  
		std::vector< std::pair<float, float>> mWindowResizeRateCustomTextureVector;



	};

}

