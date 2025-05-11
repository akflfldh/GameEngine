#pragma once

#include"Object/UiEntity.h"
#include"EditorMacro.h"
namespace Quad
{
	class FileUiEntity :public UiEntity
	{

	public:
		FileUiEntity(const std::string& name="");
		~FileUiEntity() = default;

		virtual void Initialize() override;

		static FileUiEntity* Create(Map* map, int mapLayerIndex = 0);

		virtual void Update(float deltaTime) override;

		CLASSNAME(FileUiEntity)
		CLASSSIZE(FileUiEntity)
	protected:
		void InitCreating();


	private:


	};
	REGISTEREDITORCLASS(FileUiEntity)
}