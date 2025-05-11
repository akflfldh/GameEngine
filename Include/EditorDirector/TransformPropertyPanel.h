#pragma once

#include"Predeclare.h"
#include"PanelUiEntity.h"
#include"TextBox.h"
#include<functional>
#include"EditorMacro.h"
namespace Quad
{
	class TransformPropertyPanel:public PanelUiEntity
	{
	public:
		TransformPropertyPanel(const std::string& name="");
		virtual ~TransformPropertyPanel();

		virtual void Initialize() override;

		static TransformPropertyPanel* Create(Map* map, int mapLayerIndex, float width, float height, const std::string& tagName, int textBoxNum = 3);

		virtual void Update(float deltaTime)override;
		virtual void OnEvent(Event* event) override;
		CLASSNAME(TransformPropertyPanel)
		CLASSSIZE(TransformPropertyPanel)
		void SetSelectedObject(Object* object);
		Object* GetSelectedObject() const;


		void SetXYZ(const DirectX::XMFLOAT3& xyz);

		void SetXTextBoxEnterCallback(const std::function<void()> &callback);
		void SetYTextBoxEnterCallback(const std::function<void()> &callback);
		void SetZTextBoxEnterCallback(const std::function<void()> &callback);

		 std::string GetTextX()const;
		 std::string GetTextY() const;
		 std::string GetTextZ() const;


		void SetDrawFlag(bool flag) override;
	//	void SetUpdateCallback(std::function<void(float deltaTime)> callback);



		virtual void HandleLButtonDown(Event* pEvent) override;
		virtual void HandleLButtonUp(Event* pEvent) override;


	protected:
		void InitCreating(float width, float height, const std::string& tagName, int textBoxNum );


	private:

		//void HandleLButtonDown(Event* pEvent)override;
		//void HandleLButtonUp(Event* pEvent)override;
	//	void MouseMove(Event* pEvent);



		TextBox* mTagTextBox;
		//TextBox mXTextBox;
		//TextBox mYTextBox;
		//TextBox mZTextBox;

		std::vector<TextBox*> mTextBoxVector;


		Object* mSelectedObject = nullptr;


	//	std::function<void(float deltaTime)> mUpdateCallback = nullptr;



	};
	REGISTEREDITORCLASS(TransformPropertyPanel)



}
