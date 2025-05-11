#pragma once


#include"Predeclare.h"
#include"Core/PanelUiEntity.h"
#include"TextCharacterEntity.h"
#include"Allocator/DestructingPoolAllocator.h"
#include"Font.h"

#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB TextCharacterPanelEntity:public PanelUiEntity
	{
		friend class DestructingPoolAllocator<TextCharacterPanelEntity>;

	public:
		TextCharacterPanelEntity();
		virtual ~TextCharacterPanelEntity();
		//nullptr 리턴
		//이함수를 호출후에는 더이상 이 인스턴스는 사용해서는안된다.



		void Initialize(float width, float height);

		static TextCharacterPanelEntity* Create(Map* map, int mapLayerIndex, float width, float height);


		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		virtual void SetDrawFlag(bool flag)override;


		CLASSNAME(TextCharacterPanelEntity)
		CLASSSIZE(TextCharacterPanelEntity)


		void SetPosition(float x, float y, float z)override;

		void SetGlyphTexture(Texture* texture);

		void SetAdvanceX(float advance);
		float GetAdvanceX()const;
		

		void SetGlyphMetrics(const GlyphMetrics & metrics, float advance ,float baselineLocalPosY);


		void SetCharacter(wchar_t ch);
		wchar_t GetCharacter() const;

		TextCharacterEntity* GetCharacterEntity();
		void SetLineNum(int num);
		int GetLineNum() const;

		virtual void SetStencilRefValue(unsigned char value) override;
		

		void BeforeRemoveBehavior() override;

	protected:
		void InitCreating(float width, float height);

	private:
		virtual void Initialize() override;
	

		TextCharacterEntity* mCharacterEntity;
		//static DestructingPoolAllocator<TextCharacterPanelEntity> mTextCharacterPanelEntityPoolAllocator;


		float mAdvanceX;

		wchar_t mCharacter;
		int mLineNum;


	};
	REGISTERCLASS(TextCharacterPanelEntity)
}