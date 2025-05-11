#pragma once



#include"Predeclare.h"

#include"PanelUiEntity.h"
#include"TextBox.h"
#include"EditorMacro.h"
namespace Quad
{
	class TextBoxPanel :public PanelUiEntity
	{
	public:
		TextBoxPanel(const std::string& name="");
		virtual ~TextBoxPanel();
		
		virtual void Initialize() override;
		
		static TextBoxPanel* Create(Map* map, int mapLayerIndex, float width, float height ,int fontSize =0);


		void SetText(const std::string& text);
		std::string GetText()const;

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(TextBoxPanel)
		CLASSSIZE(TextBoxPanel)
	//	virtual void AddChildPanelUiEntity(UiEntity* childPanel);
		


		virtual void SetDrawFlag(bool flag) override;
		virtual void SetActiveFlag(bool flag)override;

		virtual void SetStencilRefValue(unsigned char value) override;

		void SetTextColor(const DirectX::XMFLOAT3& textColor);
		void SetTextAlignmentDirection(ETextAlignmentDirection dir);
	protected:
		void InitCreating(float width, float height, int fontSizeIndex);

	private:



		TextBox* mTextBox;



	};
	REGISTEREDITORCLASS(TextBoxPanel)






}