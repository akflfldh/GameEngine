#pragma once



#include"Predeclare.h"

#include"Object/UiEntity.h"
#include"EditorMacro.h"

namespace Quad
{
	class AssetImageFrameEntity:public UiEntity
	{
	public:
		AssetImageFrameEntity(const std::string& name="");
		virtual ~AssetImageFrameEntity();

		virtual void Initialize() override;

		static AssetImageFrameEntity* Create(Map* map, int mapLayerIndex = 0);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;

		CLASSNAME(AssetImageFrameEntity)
		CLASSSIZE(AssetImageFrameEntity)

		void SetSelectedObject(Object* object);
		Object* GetSelectedObject() const;

		int GetSubMeshIndex()const;
		void SetSubMeshIndex(int index);
	protected:
		void InitCreating();

	private:
		//void HandleGameObjectSelect(Event * pEvent);


		Object* mSelectedObject;
		int mSubMeshIndex;




	};
	REGISTEREDITORCLASS(AssetImageFrameEntity)

}
