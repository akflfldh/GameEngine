#pragma once

#include"UiEntity.h"
#include"EditorMacro.h"
namespace Quad
{

	class DirectoryUiEntity :public UiEntity
	{
	public:
		DirectoryUiEntity(const std::string& name="");
		~DirectoryUiEntity() = default;


		CLASSNAME(DirectoryUiEntity)
		CLASSSIZE(DirectoryUiEntity)

		virtual void Initialize() override;

		static DirectoryUiEntity* Create(Map* map, int mapLayerIndex = 0);


		void Update(float deltaTime)override;

	protected:
		void InitCreating();


	private:




	};
	REGISTEREDITORCLASS(DirectoryUiEntity)
}