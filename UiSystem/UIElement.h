#pragma once


#include<stdint.h>
#include<string>
#include<memory>
#include<vector>
#include"UIType.h"
#include"UIRectTransform.h"


namespace UI
{
	class UIRectTransform;
	class UICanvas;


	class UIElement
	{
		friend class UIManager;
	public:
		UIElement(UIElementID id , const std::string & name);
		virtual ~UIElement();

		virtual void Update(float deltaTime);


		UIElementID  GetID() const;

		UIElement* GetParent() const;

		UIRectTransform* GetRectTransform() const;

		void SetParent(UIElement* parent);
		



		void SetActiveFlag(bool flag);
		bool GetActiveFlag() const;



		const std::vector<UIElement*> GetChildVector() const ;
		//아에 Destroy메서드
		


		void SetMaterialID(uint32_t materialID);
		void SetTextureID(uint32_t textureID);

		uint32_t GetMaterialID() const;
		uint32_t GetTextureID() const;






	protected:
		
			

	private:
		//manager가호출할것이다.
		void AddChildInternal(UIElement* child);
		void RemoveChildInternal(UIElement* child);//단순히 child목록에서 뺴기만한다.
		void SetCanvasInternal(UICanvas* canvas);

	private:
		std::string mName;
		UIElementID mID;

		UIElement* mParent;

		//비활성화되면 자식들도 모두 간접적으로 비활성화된다.
		bool mActiveFlag;


		std::unique_ptr<UIRectTransform> mTransform;

		std::vector<UIElement*> mChildVector;


		UICanvas* mDestCanvas;



		//외부시스템이 단지 이UIElement들이 어떤머터리얼을사용하는지 구별의 편의성을 제공하는 변수
		//둘다 0으로 초기화된다.
		uint32_t mMaterialID;	//머터리얼을 나타낼수있는 정수변수
		uint32_t mTextureID;	//머터리얼에서 사용하는 기존텍스처말고 새로운텍스처를 나타내는 정수변수 



	};


}