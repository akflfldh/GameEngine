#pragma once


#include"header.h"
#include"Predeclare.h"
#include"Utility/HandleTable.h"


#include"Core/CoreDllExport.h"


namespace Quad
{

	class CORE_API_LIB Transform
	{
	public:
		Transform();
		~Transform() = default;

		DirectX::XMFLOAT3 GetPositionLocal() const;
		void SetPositionLocal(const DirectX::XMFLOAT3 & position);
		void SetPositionLocal(float x, float y, float z);

		void TranslateLocal(const DirectX::XMFLOAT3& translate);


		void SetScaleLocal(const DirectX::XMFLOAT3 & scale);
		void SetRotationLocal(const DirectX::XMFLOAT3&  rotation);
		void SetQuaternionLocal(const DirectX::XMFLOAT4 & quaternion);	
		void SetTransformLocal(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& quaternion, const DirectX::XMFLOAT3& position);
		void SetTransformLocal(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& position);
		void XM_CALLCONV SetTransformLocal(DirectX::FXMMATRIX localMatrix);
		void SetTransformLocal(const DirectX::XMFLOAT4X4& localMatrix);


		DirectX::XMFLOAT4X4 GetTransformLocal() const;

		DirectX::XMFLOAT3 GetScaleLocal() const;

		DirectX::XMFLOAT3 GetRoataiontLocal() const;
	
		const DirectX::XMFLOAT4 & GetQuaternionLocal()const;


		DirectX::XMFLOAT3 GetLookLocal()const;

		DirectX::XMFLOAT3 GetRightLocal()const;

		DirectX::XMFLOAT3 GetUpLocal()const;


		//void SetDefaultWidth(float width);
		//void SetDefaultHeight(float height);
		//void SetDefaultDepth(float depth);



		bool GetDirtyFlag() const;
		
		

		void Serialize(const std::string& tag="");
		void DeSerialize(const std::string& tag="");


		unsigned long long GetTransformLocalDirtyCount() const;

	private:

		void UpdateDirectionVector();
	
		void SetDirtyFlag(bool flag);

	private:
		//local matrix와 local속성들은 바뀔때마다 다시 계산해줘야하는거지
		//이 엔티티의 업데이트가 완료된이후에도 다른 엔티티들이 이 엔티티의 속성을 바꿔도 계속 최신정보가 유지된다.

		DirectX::XMFLOAT3 mPositionLocal;
	
		DirectX::XMFLOAT3 mScaleLocal;
		DirectX::XMFLOAT4 mQuaternionLocal;
		
		mutable DirectX::XMFLOAT4X4 mTransformLocalMatrixF;

		//오일러
		DirectX::XMFLOAT3 mLookLocal;
		

		DirectX::XMFLOAT3 mRightLocal;
		

		DirectX::XMFLOAT3 mUpLocal;
		

		//쿼터니언


		////부모의 변환정보에 영향을 받는지에대한 플래그
		//bool mIndependentTransformFlag;

		////부모의 월드변환에서 스케일,회전,이동의 영향을 받는지에대한 개별 플래그  
		//bool mIndependentScaleFlag;
		//bool mIndependentRotationFlag;
		//bool mIndependentTransitionFlag;
		//더티플래그
		bool mDirtyFlag;


		//float mDefaultWidth;
		//float mDefaultHeight;
		//float mDefaultDepth;

		DirectX::XMFLOAT3 mDefaultLook;
		DirectX::XMFLOAT3 mDefaultUp;
		DirectX::XMFLOAT3 mDefaultRight;

		mutable unsigned long long  mTransformLocalDirtyCount = 0;	//새로운,스케일,회전,위치가 설정될때마다 증가한다.
	};

}