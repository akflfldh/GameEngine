#pragma once


#include<string>
#include<vector>

#include"UIType.h"


#include<CoreMath.h>


namespace UI
{
	
	class UIElement;
	
	
	




	class UICanvas
	{
		friend class UIManager;

	public:
		UICanvas(UICanvasID id, const std::string & name ,ECanvasSizeMode sizeMode);
		~UICanvas();


		void Update(float deltaTime);


		void AddChild(UIElement* uiElement);

		ECanvasSizeMode GetSizeMode() const;


		void SetSize(CoreMath::Vector2 size);
		bool GetActiveFlag()const;


		const std::vector<UIElement*>& GetChildUIElementAll() const;

	private:

		void AddChildInternal(UIElement* uiElement);

	private:
		UICanvasID mID;
		std::string mName;
		bool mActiveFlag;


		ECanvasSizeMode mCanvasSizeMode;

		std::vector<UIElement*> mChildUIElement;//최상위UIElement들

		CoreMath::Vector2 mSize;

	};

}
