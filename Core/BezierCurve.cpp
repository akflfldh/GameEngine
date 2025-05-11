#include "BezierCurve.h"
#include<Queue>
Quad::BezierCurve::BezierCurve()
{
	std::vector<DirectX::XMFLOAT3> defaultControlVector(4);
	SetControlPointVector(std::move(defaultControlVector));

}

Quad::BezierCurve::~BezierCurve()
{


}

void Quad::BezierCurve::SetControlPoint(const DirectX::XMFLOAT3& controlPoint, unsigned int index)
{
	if (index >= 4)
		return;

	Curve::SetControlPoint(controlPoint, index);


}

bool Quad::BezierCurve::GetControlPoint(int index, DirectX::XMFLOAT3 & oControlPoint)const
{
	if (index >= 4)
		return false;

	oControlPoint = GetControlPointVector()[index];

	return true;

}

void Quad::BezierCurve::Evaulate(std::vector<DirectX::XMFLOAT3>& oVertexVector)
{

	//oVertexVector =	GetControlPointVector();
	const std::vector<DirectX::XMFLOAT3> & controlPointVector =	GetControlPointVector();
	
	
	


	std::queue<DirectX::XMFLOAT3> controlPointQueue;
	controlPointQueue.push(controlPointVector[0]);
	controlPointQueue.push(controlPointVector[1]);
	controlPointQueue.push(controlPointVector[2]);
	controlPointQueue.push(controlPointVector[3]);

	int divisionNum = 5;
	int loopNum = 2;				;//실제 loop횟수는 2^5 번 //깊이 하나씩내려갈수록 2의배수로 횟수가늘어난다.
	
	loopNum = (loopNum << (divisionNum - 1))-1;
	


	std::vector<DirectX::XMFLOAT3>  oVertexSubVector(7);
	for(int i=0; i<loopNum; ++i)
	{


		DirectX::XMFLOAT3 p0F = controlPointQueue.front();
		controlPointQueue.pop();
		DirectX::XMFLOAT3 p1F = controlPointQueue.front();
		controlPointQueue.pop();
		DirectX::XMFLOAT3 p2F = controlPointQueue.front();
		controlPointQueue.pop();
		DirectX::XMFLOAT3 p3F = controlPointQueue.front();
		controlPointQueue.pop();



		DirectX::XMVECTOR p0, p1, p2, p3;
		p0 = DirectX::XMLoadFloat3(&p0F);
		p1 = DirectX::XMLoadFloat3(&p1F);
		p2 = DirectX::XMLoadFloat3(&p2F);
		p3 = DirectX::XMLoadFloat3(&p3F);



		DirectX::XMVECTOR q0 = DirectX::XMVectorLerp(p0, p1, 0.5f);
		DirectX::XMVECTOR q1 = DirectX::XMVectorLerp(p1, p2, 0.5f);
		DirectX::XMVECTOR q2 = DirectX::XMVectorLerp(p2, p3, 0.5f);

		DirectX::XMVECTOR m0 = DirectX::XMVectorLerp(q0, q1, 0.5f);
		DirectX::XMVECTOR m1 = DirectX::XMVectorLerp(q1, q2, 0.5f);


		DirectX::XMVECTOR n0 = DirectX::XMVectorLerp(m0, m1, 0.5f);

	
		DirectX::XMStoreFloat3(&oVertexSubVector[0], p0);
		DirectX::XMStoreFloat3(&oVertexSubVector[1], q0);
		DirectX::XMStoreFloat3(&oVertexSubVector[2], m0);
		DirectX::XMStoreFloat3(&oVertexSubVector[3], n0);
		DirectX::XMStoreFloat3(&oVertexSubVector[4], m1);
		DirectX::XMStoreFloat3(&oVertexSubVector[5], q2);
		DirectX::XMStoreFloat3(&oVertexSubVector[6], p3);

		if (i < loopNum/2)
		{
			controlPointQueue.push(oVertexSubVector[0]);
			controlPointQueue.push(oVertexSubVector[1]);
			controlPointQueue.push(oVertexSubVector[2]);
			controlPointQueue.push(oVertexSubVector[3]);

			controlPointQueue.push(oVertexSubVector[3]);
			controlPointQueue.push(oVertexSubVector[4]);
			controlPointQueue.push(oVertexSubVector[5]);
			controlPointQueue.push(oVertexSubVector[6]);
		}
		else
		{

			//16번부터는 끝이니, 더이상분할 하지않고, 최종출력버텍스벡터로 들어간다.
			//항상 마지막버텍스가 다음번 분할과 중복되니 마지막버텍스는 포함하지않는다.
			//단 가장마지막분할은 제외 ,루프밖에서 마지막 버텍스를 추가한다.
			oVertexVector.insert(oVertexVector.end(), oVertexSubVector.begin(), oVertexSubVector.end()-1);
			
		}


	}
	oVertexVector.push_back(oVertexSubVector[6]);
	
	return;


}
