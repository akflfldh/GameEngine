#pragma once




#include<string>
#include<unordered_map>


#include"UIType.h"
#include"UICanvas.h"

namespace UI
{
	class UIElement;
	class UIManager
	{
	public:
		static UIManager* GetInstance();
		~UIManager();



		//캔버스생성
		UICanvasID CreateCanvas(const std::string & name,ECanvasSizeMode sizeMode);
		// 캔버스를 파괴
		void DestroyCanvas(UICanvasID id);
		UICanvas* GetCanvas(UICanvasID id) const;



		//UIElement생성
		UIElement * CreateUIElement(const std::string& name);
		//UIElement파괴
		void DestoryUIElement(UIElementID uiElementID);
		UIElement* GetUIElement(UIElementID uiElementID) const;

		//Manager에서 직접호출하거나, UIElement,UICanvas에서 호출하면 간접적으로 호출된다.
		void AddUIElement(UICanvasID canvasID, UIElementID uiElementID);
		void AddUIElement(UICanvas * canvas, UIElement * uiElementID);
		void SetParent(UIElement* parent, UIElement* child);


		const std::unordered_map<UICanvasID, UICanvas*>& GetUICanvasAll() const;


	private:

		UIManager();

	private:


		std::unordered_map<UICanvasID, UICanvas*> mUICanvasTable;


		std::unordered_map<UIElementID, UIElement*> mUIElementTable;

		UICanvasID mNextUICanvasID;
		std::vector<UICanvasID> mFreeCanvasIDVector;

		UIElementID mNextUIElementID;
		std::vector<UIElementID> mFreeUIElementIDVector;

	};
}

