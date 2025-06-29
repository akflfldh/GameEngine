#pragma once

#include"Component/IMeshComponent.h"
#include"Core/CoreDllExport.h"
namespace Quad
{

	class CORE_API_LIB UIMeshComponent:public IMeshComponent
	{
	public:
		UIMeshComponent();
		virtual ~UIMeshComponent();

		virtual void Start() override;
		virtual void Update(float deltaTime) override;


		//uiMeshComponent에서는 mesh에상관없이 항상 rect를 메시로가진다.
		virtual void SetMesh(Mesh* mesh) override;

		virtual void Serialize(const std::string& tag = "") override;
		virtual void DeSerialize(const std::string& tag = "") override;


		//transform local scale값과 동일 
		void SetWidthLocal(float width); //x
		void SetHeightLocal(float height); //y

		float GetWidthLocal() const;	
		float GetHeightLocal() const;

		void SetWidthHeightLocal(float width, float height);

		float GetWidthWorld() const;
		float GetHeightWorld() const;




		void SetColorItensity(const DirectX::XMFLOAT3& colorIntensity);
		void SetColor(const DirectX::XMFLOAT3& color);

		DirectX::XMFLOAT3 GetColor() const;

		void SetTexture(Texture* texture);
		void SetTexture(const char* textureName);
		void SetEffect(const std::string& effectName);


		CLASSSIZE(UIMeshComponent)
		CLASSNAME(UIMeshComponent)
		CLASSNAMESTATIC(UIMeshComponent)

	private:
		void SetMeshToCollider(Mesh* mesh);


	

	};
	REGISTERCOMPONENTCLASS(UIMeshComponent)

}

