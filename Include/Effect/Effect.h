#pragma once

#include"Predeclare.h"
#include<vector>
#include<unordered_map>
#include"Utility/UniqueIDTable.h"

#include<string>

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

	public:
		//Effect(const std::string& name="");
		Effect();

		~Effect() = default;
		
		Effect(const Effect& effect);

		const std::vector<RenderPassTwo*>& GetStaticRenderPassVector() const;
		const std::vector<RenderPassTwo*>& GetSkinningRenderPassVector() const;
		const std::string& GetName() const;

		unsigned long long GetUniqueID() const;

		virtual void Serialize();
		virtual void DeSerialize();

	private:
		void SetStaticMeshRenderPassVector(std::vector<RenderPassTwo*> && renderPassVector);
		void SetSkinningMeshRenderPassVector(std::vector<RenderPassTwo*> && renderPassVector);


	private:
		void SetUniqueID(unsigned long long id);

		std::string mName;
		std::vector<RenderPassTwo *> mStaticMeshRenderPassVector;
		std::vector<RenderPassTwo *> mSkinningMeshRenderPassVector;



		unsigned long long mID;





	};

}

