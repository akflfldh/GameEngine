#include "UIRectTransform.h"
#include"UIElement.h"
UI::UIRectTransform::UIRectTransform(UIElement* owner)
	:mOwner(owner),mLocalDirtyFlag(false), mParentTrasnformVersionNum(0),mCurrentTransformVersionNum(0)
{
}


UI::UIRectTransform::~UIRectTransform()
{
}

void UI::UIRectTransform::SetPositionLocal(const CoreMath::Vector3& posLocal)
{
	mPositionLocal = posLocal;

	mLocalDirtyFlag = true;
}

void UI::UIRectTransform::SetScaleLocal(const CoreMath::Vector3& scaleLocal)
{
	mScaleLocal = scaleLocal;
	mScaleLocal.Z = 1.0f;

	mLocalDirtyFlag = true;

}
CoreMath::Vector3 UI::UIRectTransform::GetPositionLocal() const
{
	return mPositionLocal;
}
CoreMath::Vector3 UI::UIRectTransform::GetScaleLocal() const
{
	return mScaleLocal;
}


CoreMath::Vector3 UI::UIRectTransform::GetPositionWorld() const
{

	//worldTransform과 동일하게 
	//localDirty와 부모와의 버전일치를 확인하고 적절히 갱신후 
	//분해하여 리턴한다.

	GetWorldMatrix();

	CoreMath::Vector3 positionWorld;
	CoreMath::Quaternion quaternion;
	CoreMath::Vector3 scaleWorld;
	CoreMath::Matrix4X4::MatrixDecompose(positionWorld, quaternion, scaleWorld, mWorldMatrix);

	return positionWorld;
}



CoreMath::Vector3 UI::UIRectTransform::GetScaleWorld() const
{

	GetWorldMatrix();
	CoreMath::Vector3 positionWorld;
	CoreMath::Quaternion quaternion;
	CoreMath::Vector3 scaleWorld;
	CoreMath::Matrix4X4::MatrixDecompose(positionWorld, quaternion, scaleWorld, mWorldMatrix);

	return scaleWorld;

}

CoreMath::Vector3 UI::UIRectTransform::GetSizeWorld() const
{
	//dirty플래그가 켜져있으면 조상이 변한것임으로 업데이트 후 리턴
	return mSizeWorld;
}



CoreMath::Matrix4X4 UI::UIRectTransform::GetWorldMatrix() const
{

	//먼저 부모의 변환행렬을 가져와야한다
	//GetWorldMatrix 그래야 부모도 자신의 부모들과의 버전비교후 업데이트를 올바르게 먼저수행(재귀적으로)
	//만약 완전히 갱신된상태였다면 빠르게 거슬러올라가면서 버전확인후 돌아온다.

	UIElement * parentUIElement =	mOwner->GetParent();	//최상위는 UICanvas일것이다.
	if (parentUIElement != nullptr)
	{
		CoreMath::Matrix4X4 parentWorldMatrix = parentUIElement->GetRectTransform()->GetWorldMatrix();
		//그렇지않은경우 적절한 부모의 월드행렬,버전갱신 발생한다.

		if (GetParentTransformVersionNum() != parentUIElement->GetRectTransform()->GetCurrentTransformVersionNum())
		{
			//업데이트 필요
			UpdateWorldMatrix(parentWorldMatrix);
			mParentTrasnformVersionNum = parentUIElement->GetRectTransform()->GetCurrentTransformVersionNum();
			mCurrentTransformVersionNum++;
		}
	}
	else if (mLocalDirtyFlag)
	{
		//역시 업데이트필요 // 
		UpdateWorldMatrix(CoreMath::Matrix4X4::Identity);
		mLocalDirtyFlag = false;
		mCurrentTransformVersionNum++;
	}
	else
	{
		//업데이트 불필요 최신상태

	}

	return mWorldMatrix;
}

void UI::UIRectTransform::GetRectPointWorld(std::vector<CoreMath::Vector3> oPoints) const
{

	oPoints.resize(4);

	CoreMath::Vector3 pos = GetPositionWorld();
	CoreMath::Vector3 size = GetSizeWorld();



	CoreMath::Vector3 sizeDivide2 = size / 2;
	//0   x  -  , y + 
	oPoints[0] = pos;
	oPoints[0].X -= sizeDivide2.X;
	oPoints[0].Y += sizeDivide2.Y;

	//1  x + , y +
	oPoints[1] = pos;
	oPoints[1].X += sizeDivide2.X;
	oPoints[1].Y += sizeDivide2.Y;

	//2   x - , y - 
	oPoints[2] = pos;
	oPoints[2].X -= sizeDivide2.X;
	oPoints[2].Y -= sizeDivide2.Y;

	//3  x +  , y -
	oPoints[3] = pos;
	oPoints[3].X += sizeDivide2.X;
	oPoints[3].Y -= sizeDivide2.Y;

}

uint64_t UI::UIRectTransform::GetParentTransformVersionNum() const
{
	return mParentTrasnformVersionNum;
}

uint64_t UI::UIRectTransform::GetCurrentTransformVersionNum() const
{
	return mCurrentTransformVersionNum;
}

void UI::UIRectTransform::UpdateWorldMatrix(const CoreMath::Matrix4X4 & parentWorldMatrix) const
{
}
