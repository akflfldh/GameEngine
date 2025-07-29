#pragma once

#include<CoreMath.h>
#include<stdint.h>
#include<vector>
namespace UI
{
	class UIRectTransform
	{
		friend class UIElement;

	public:
		UIRectTransform(UIElement * owner);
		~UIRectTransform();

		void SetPositionLocal(const CoreMath::Vector3& posLocal);
		void SetScaleLocal(const CoreMath::Vector3& scaleLocal);

		CoreMath::Vector3 GetPositionLocal() const;
		CoreMath::Vector3 GetScaleLocal() const;

		CoreMath::Vector3 GetPositionWorld() const;
		CoreMath::Vector3 GetScaleWorld() const;

		CoreMath::Vector3 GetSizeWorld() const;


		CoreMath::Matrix4X4 GetWorldMatrix() const;
		//world transform정보를 get할때만 dirty플래그를 보고 계산해서 리턴하자.



				//4개의 꼭짓점을 돌려준다.
		//x축 오른쪽  y축이 위쪾으로 향하는 좌표계
		//0 (x -, y + ) , 1 (x + , y + ) 2 ( x- , y-) , 3(x + , y -)
		void GetRectPointWorld(std::vector<CoreMath::Vector3> oPoints) const;


	private:

		uint64_t GetParentTransformVersionNum() const;
		uint64_t GetCurrentTransformVersionNum() const;
		
		void UpdateWorldMatrix(const CoreMath::Matrix4X4 & parentWorldMatrix) const;	// worldMatrix 재계산;

	private:
		UIElement* mOwner;

		//pos
		//scale
		CoreMath::Vector3 mPositionLocal; //z성분은 깊이 
		CoreMath::Vector3 mScaleLocal;	//항상 모든성분 1.0f
		CoreMath::Vector3 mSizeWorld;	//z성분은 1.0f

		mutable bool mLocalDirtyFlag;

		///mutable CoreMath::Vector3 mPositionWorld;
		//mutable CoreMath::Vector3 mScaleWorld;
		CoreMath::Matrix4X4 mWorldMatrix;
		

		//이것이 부모트랜스폼의 현재 versionNum와다르면(아마 작은경우) 
		//부모의 월드변환행렬을 가져와 갱신해야한다.
		mutable uint64_t mParentTrasnformVersionNum;	

		//이 transform의 versionNum
		mutable uint64_t mCurrentTransformVersionNum;
	
	};


}