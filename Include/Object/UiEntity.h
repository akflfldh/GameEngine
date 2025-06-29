#pragma once

#include"Predeclare.h"
#include"Object/Object.h"
#include"Core/CoreDllExport.h"

#include"InputType.h"
namespace Quad
{

	class UIMeshComponent;
	class UiComponent;

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


		void SetWidthHeightLocal(float width, float height);
		void SetWidthHeightWorld(float width, float height);


		void SetTextureTransform(const DirectX::XMFLOAT4X4& transform);

		CLASSNAME(UiEntity)

		virtual void Serialize() override;
		virtual void DeSerialize() override;

		void HandleUiComponentCallback(const MouseInputData& mouseInputData);

		UiComponent* GetUiComponent() const;
		UIMeshComponent* GetUiMeshComponent() const;

	protected:
		void InitCreating();


	
		virtual void HandleHover(const MouseInputData& mouseInputData);
		virtual void HandleLButtonDown(const MouseInputData& mouseInputData);
		virtual void HandleLButtonUp(const MouseInputData& mouseInputData);
		virtual void HandleRButtonDown(const MouseInputData& mouseInputData);
		virtual void HandleRButtonUp(const MouseInputData& mouseInputData);
		virtual void HandleMouseMove(const MouseInputData& mouseInputData);



		

		//Texture* GetDefaultTexture()const;
		//Texture* GetHoverTexture()const;
		//Texture* GetHoverSelectTexture()const;
		//Texture* GetNotHoverSelectTexture()const;



	protected:
	

		
	private:
		UIMeshComponent* mUiMeshComponent;
		UiComponent* mUiComponent;


	};

}