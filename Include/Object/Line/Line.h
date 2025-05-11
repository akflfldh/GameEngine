#pragma once


#include"Predeclare.h"
#include"Object/Line/LineBase.h"
#include<DirectXMath.h>

#include"Core/CoreDllExport.h"

namespace Quad
{
	//Line은 Mesh같은것
	class CORE_API_LIB Line:public LineBase
	{
		friend class LineFactory;
	public:
		Line(const std::string& name = "");
		virtual ~Line() = default;

		virtual void Initialize() override;
		virtual void DefaultCreatingInitialize() override;
		virtual void OnEvent(Event* event) override;
		virtual void Update(float deltaTime) override;
		virtual void EndUpdate(float deltaTime) override;
	//	virtual void UpdateCollider(float deltaTime) override;

		CLASSNAME(Line)
		CLASSSIZE(Line)
		void SetStartVertexPosition(const DirectX::XMFLOAT3& pos);
		void SetEndVertexPosition(const DirectX::XMFLOAT3& pos);

		const DirectX::XMFLOAT3 & GetStartVertexPosition() const;
		const DirectX::XMFLOAT3 & GetEndVertexPosition() const;

		virtual void Serialize() override;
		virtual void DeSerialize() override;
	protected:
		
		
		DirectX::XMFLOAT3 mStartPosition;
		DirectX::XMFLOAT3 mEndPosition;

	};
	
}

