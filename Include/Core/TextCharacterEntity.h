#pragma once




#include"Predeclare.h"

#include"Object/UiEntity.h"
#include"Font.h"

#include"Core/CoreDllExport.h"
namespace Quad
{
	class CORE_API_LIB TextCharacterEntity:public UiEntity
	{
	public:
		TextCharacterEntity();
		virtual ~TextCharacterEntity();

		virtual void Initialize() override;

		static TextCharacterEntity* Create(Map* map, int mapLayerIndex = 0);

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(TextCharacterEntity)
		CLASSSIZE(TextCharacterEntity)
	//	void SetGlyphMetrics(const GlyphMetrics& metrics);

		void SetCh(wchar_t ch);
	protected:
		void InitCreating();

	private:
		//GlyphMetrics mGlyphMetrics;
		wchar_t mCh;

	};
	REGISTERCLASS(TextCharacterEntity)
}
