#pragma once

#include"Predeclare.h"
#include"Object/Object.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB UiEntity :public Object
	{
	public:
		UiEntity(const std::string& name="");
		virtual ~UiEntity();

		virtual void Initialize() override;

		static UiEntity* Create(Map* map, int mapLayerIndex);
		virtual void DefaultCreatingInitialize() override;

		virtual void Update(float deltaTime)override;
		virtual void EndUpdate(float deltaTime)override;
		virtual void OnEvent(Event* event) override;



		void SetTexture(const std::string &name, EUiEntityMouseState uiEntityMouseState = EUiEntityMouseState::eDefault);
		void SetTexture(Texture * texture, EUiEntityMouseState uiEntityMouseState = EUiEntityMouseState::eDefault);

		virtual void SetSize(float width, float height);
		virtual void SetPosition(float x, float y, float z);
		virtual void SetPosition(const DirectX::XMFLOAT3 & posLocal);

		void SetColorItensity(const DirectX::XMFLOAT3& itensity);
		void SetColor(const DirectX::XMFLOAT3& color);

		void SetTextureTransform(const DirectX::XMFLOAT4X4& transform);

		CLASSNAME(UiEntity)

		 float GetWidth() const;
		 float GetHeight() const;

		void SetEffect(const std::string & name);


		virtual void SetDrawFlag(bool flag) override;

		virtual void Serialize() override;
		virtual void DeSerialize() override;

	protected:
		void InitCreating();


		void SetCurrentTexture(Texture* texture);

		virtual void HandleLButtonDown(Event* pEvent);
		virtual void HandleLButtonUp(Event* pEvent);

		virtual void HandleHover(Event* pEvent);
		

		Texture* GetDefaultTexture()const;
		Texture* GetHoverTexture()const;
		Texture* GetHoverSelectTexture()const;
		Texture* GetNotHoverSelectTexture()const;


		

	private:
	




	private:


		//선택플래그는 good근데 lbuttondown플래그는 inputsystem 키보드에서 얻어오는게 사실정상인거같다.
		bool mSelectState = false;
		bool mLButtonDown = false;
		//유저가 설정하는 속성들어가는지

		float mWidth;
		float mHeight;



		Texture* mDefaultTexture;
		Texture* mHoverTexture;
		Texture* mHoverSelectTexture;
		Texture* mNotHoverSelectTexture;



	};

}