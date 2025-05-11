#include "Object/Line/Spline.h"

#include"Core/Curve.h"
#include"Component/UiColliderComponent.h"
#include"Collision/BoxCollider.h"

#include<algorithm>
#include"CurveFactory.h"
#include"ObjectFactory/LineFactory.h"


Quad::Spline::Spline(const std::string& name)
	:LineBase(name,EObjectType::eSpline),mSplineRenderStyle(ESplineRenderStyle::eThinLine), mThickness(3.0F)
{
	SetEffectName("Spline.effect");


	GetModel()->AddComponernt(Quad::EComponentType::eUiColliderComponent);

}

Quad::Spline::~Spline()
{
	for (auto element : mCurveVector)
	{
		CurveFactory::ReleaseCurve(element);
	}





}

void Quad::Spline::Initialize()
{




}

void Quad::Spline::DefaultCreatingInitialize()
{




}

void Quad::Spline::OnEvent(Event* event)
{





}

void Quad::Spline::Start()
{
}

void Quad::Spline::Update(float deltaTime)
{
}

void Quad::Spline::EndUpdate(float deltaTime)
{

}

void Quad::Spline::AddCurve(Curve* curve, int num)
{
	
	mCurveVector.insert(mCurveVector.begin() + num, curve);

}

void Quad::Spline::AddCurve(Curve* curve)
{
	mCurveVector.push_back(curve);
}

Quad::Curve* Quad::Spline::GetCurve(int index)
{
	if (mCurveVector.size() <= index)
		return nullptr;

	return mCurveVector[index];

}

inline void MinVertex(DirectX::XMFLOAT3& oMin, DirectX::XMFLOAT3& other)
{

	if (oMin.x > other.x)
		oMin.x = other.x;

	if (oMin.y > other.y)
		oMin.y = other.y;

	if (oMin.z > other.z)
		oMin.z = other.z;


}

inline void MaxVertex(DirectX::XMFLOAT3& oMax, DirectX::XMFLOAT3& other)
{

	if (oMax.x < other.x)
		oMax.x = other.x;

	if (oMax.y < other.y)
		oMax.y = other.y;

	if (oMax.z < other.z)
		oMax.z = other.z;


}

void Quad::Spline::Evaulate()
{
	std::vector<DirectX::XMFLOAT3> vertexVector;
	for (auto curve : mCurveVector)
	{
		std::vector<DirectX::XMFLOAT3> tempVector;
		curve->Evaulate(tempVector);
		vertexVector.insert(vertexVector.end(), tempVector.begin(), tempVector.end());
	}

	if (mSplineRenderStyle == ESplineRenderStyle::eThickLine)
	{
		EvaulateThickLine(vertexVector);
	}


	LineModelSubMesh* modelSubMesh = GetModelSubMesh();
	modelSubMesh->mIndexRange = { 0,6 };
	modelSubMesh->mVertexOffset = 0;

	if (modelSubMesh->mVertexNum < vertexVector.size())
	{
		LineFactory::ChangeVertexBuffer(this, vertexVector.size());
	}
	modelSubMesh->mVertexNum = vertexVector.size();

	DirectX::XMFLOAT3 minPos = vertexVector[0];
	DirectX::XMFLOAT3 maxPos = vertexVector[0];



	for(int i=0; i<vertexVector.size(); ++i)
	{
		GetVertexBuffer()->CopyData(i, &vertexVector[i], sizeof(DirectX::XMFLOAT3));

		
		MinVertex(minPos, vertexVector[i]);
		MaxVertex(maxPos, vertexVector[i]);



	}
	



	//box collider의 min, max 설정
	//그래야 렌더링이된다.





	//ColliderComponent * colliderComponent =	GetModel()->GetColliderComponent();
//	BoxCollider* boxCollider = static_cast<BoxCollider*>(colliderComponent->GetCollider());
	////boxCollider->SetMinMaxVertex(minPos, maxPos);
	GetTransform().SetDirtyFlag(true);


}

void Quad::Spline::SetThickness(float value)
{
	mThickness= value;

}

float Quad::Spline::GetThickness() const
{
	return mThickness;
}

void Quad::Spline::Serialize()
{
}

void Quad::Spline::DeSerialize()
{
}

void Quad::Spline::SetSplineRenderStyle(Quad::Spline::ESplineRenderStyle renderStyle)
{

	mSplineRenderStyle = renderStyle;


	switch (mSplineRenderStyle)
	{
	case ESplineRenderStyle::eThinLine:
		SetEffectName("Spline.effect");
		break;

	case ESplineRenderStyle::eThickLine:
		SetEffectName("Thick2DSpline.effect");
		break;

	}

}

void Quad::Spline::EvaulateThickLine(std::vector<DirectX::XMFLOAT3>& vertexVector)
{
	float thickness = GetThickness();
	DirectX::XMVECTOR zVector = DirectX::XMVectorSet(0, 0, 1, 0);

	std::vector<DirectX::XMFLOAT3> newVertexVector(vertexVector.size()*2-2);

	int currNewVertexIndex = 0;
	for (int i = 1; i < vertexVector.size()-1; ++i)
	{

		DirectX::XMVECTOR prevVertex = DirectX::XMLoadFloat3(&vertexVector[i - 1]);
		DirectX::XMVECTOR nextVertex = DirectX::XMLoadFloat3(&vertexVector[i + 1]);
		DirectX::XMVECTOR currVertex = DirectX::XMLoadFloat3(&vertexVector[i]);

		DirectX::XMVECTOR normalVector = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(nextVertex, prevVertex), zVector));

		DirectX::XMFLOAT3 newVertex;

		DirectX::XMStoreFloat3(&newVertex,DirectX::XMVectorAdd(currVertex, DirectX::XMVectorScale(normalVector, thickness)));

		newVertexVector[currNewVertexIndex] = vertexVector[i - 1];
		newVertexVector[currNewVertexIndex + 1] = newVertex;
		currNewVertexIndex += 2;
	}
	
	newVertexVector[currNewVertexIndex] = vertexVector[vertexVector.size() - 2];
	newVertexVector[currNewVertexIndex+1] = vertexVector[vertexVector.size() - 1];
	
	vertexVector = std::move(newVertexVector);

}
