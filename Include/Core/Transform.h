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
		Transform(SceneElement * destSceneElementt);
		~Transform() = default;

		void Initialize();
		void Update();

		DirectX::XMFLOAT3 GetPositionLocal() const;
		DirectX::XMFLOAT3 GetPositionWorld() const;
		void SetPositionLocal(const DirectX::XMFLOAT3 & position);
		void TranslateLocal(const DirectX::XMFLOAT3& translate);


		void SetScaleLocal(const DirectX::XMFLOAT3 & scale);
		void SetRotationLocal(const DirectX::XMFLOAT3&  rotation);
		void SetQuaternionLocal(const DirectX::XMFLOAT4 & quaternion);	
		void SetTransformLocal(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& quaternion, const DirectX::XMFLOAT3& position);
		void SetTransformLocal(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& position);
		void XM_CALLCONV SetTransformLocal(DirectX::FXMMATRIX localMatrix);


		void SetScaleWorld(const DirectX::XMFLOAT3& scale);
		void SetPositionWorld(const DirectX::XMFLOAT3 &position);
		void SetRotationWorld(const DirectX::XMFLOAT3& rotation);
		void SetQuaternionWorld(const DirectX::XMFLOAT4& quaternion);

		void SetTransformWorld(const DirectX::XMFLOAT4X4& worldMatrixF);
		void XM_CALLCONV SetTransformWorld(DirectX::FXMMATRIX worldMatrix);

		


		DirectX::XMFLOAT3 GetScaleLocal() const;
		DirectX::XMFLOAT3 GetScaleWorld()const;

		DirectX::XMFLOAT3 GetRoataiontLocal() const;
		DirectX::XMFLOAT3 GetRotationWorld() const;

		const DirectX::XMFLOAT4 & GetQuaternionLocal()const;
		const DirectX::XMFLOAT4 & GetQuaternionWorld() const;






		DirectX::XMFLOAT3 GetLookLocal()const;
		DirectX::XMFLOAT3 GetLookWorld()const;

		DirectX::XMFLOAT3 GetRightLocal()const;
		DirectX::XMFLOAT3 GetRightWorld()const;

		DirectX::XMFLOAT3 GetUpLocal()const;
		DirectX::XMFLOAT3 GetUpWorld()const;



		DirectX::XMFLOAT4X4 GetLocalMatrix()const;
		DirectX::XMFLOAT4X4 GetWorldMatrix()const;
		



		//이함수를 호출하고나면 world matrix가 셋팅이되어있을것이다.
		void UpdateWorldMatrix(const DirectX::XMFLOAT4X4& parentWorldMatrixF );





		void SetIndependentTransformFlag(bool flag);
		bool GetIndependentTransformFlag()const;

		void SetIndependentTransitionFlag(bool flag);
		bool GetIndependentTransitionFlag()const;

		void SetIndependentRotationFlag(bool flag);
		bool GetIndependentRotationFlag()const;

		void SetIndependentScaleFlag(bool flag);
		bool GetIndependentScaleFlag()const;

		void ResetChangeValue();





		void SetDefaultWidth(float width);
		void SetDefaultHeight(float height);
		void SetDefaultDepth(float depth);



		void SetWidthWorld(float width);
		void SetHeightWorld(float height);
		void SetDepthWorld(float depth);

		float GetWidthWorld() const;
		float GetHeightWorld() const;
		float GetDepthWorld() const;



	//	void RegisterChangeEventDst(Object* object, void(*fun)(Object*));

		//트랜스폼이 변경되었을때 호출되는 콜백 등록함수
		QHANDLE RegisterTransformChangeCallback(const std::function<void()> & callback);
		void InvokeChangeEventCallback();


		bool GetDirtyFlag() const;
		void SetDirtyFlag(bool flag);
		
		void SetTransformChangeEventFlag(bool onOff);
		bool GetTransformChangeEventFlag() const;

		//QHANDLE RegisterTransformChangeCallback(const std::function<void()>& callback);
		void RemoveTransformChangeCallback(QHANDLE handle);

		void Serialize(const std::string& tag="");
		void DeSerialize(const std::string& tag="");

	private:
		DirectX::XMMATRIX XM_CALLCONV CalculateLocalMatrix();
		DirectX::XMMATRIX XM_CALLCONV CalculateWorldMatrix();
		void UpdateDirectionVector();
		DirectX::XMMATRIX XM_CALLCONV CalculateParentMatrix();

		//local matrix(현재속성값으로새로계산)와 parentWorldMatrix를바탕으로 ,world matrix와 속성값을 다시계산한다(set메서드를 위한것)
		void CalculateNewWorldMatrix(DirectX::FXMMATRIX parentWorldMatrx);
		//worldMatrix(현재속성값으로새로계산)와 parentWorldMatrix를 바탕으로 local matrix와 속성값을 다시계산한다.(set메서드를 위한것)
		void CalculateNewLocalMatrix(DirectX::FXMMATRIX parentWorldMatrix);
		
		

	//	void CallTransformChangeCallback();


	private:
		//local matrix와 local속성들은 바뀔때마다 다시 계산해줘야하는거지
		//이 엔티티의 업데이트가 완료된이후에도 다른 엔티티들이 이 엔티티의 속성을 바꿔도 계속 최신정보가 유지된다.

		DirectX::XMFLOAT3 mPositionLocal;
		DirectX::XMFLOAT3 mPositionWorld;

		DirectX::XMFLOAT3 mScaleLocal;
		DirectX::XMFLOAT3 mScaleWorld;
		//DirectX::XMFLOAT3 mRotationLocal;
		//DirectX::XMFLOAT3 mRotationWorld;
		//

		DirectX::XMFLOAT4 mQuaternionLocal;
		DirectX::XMFLOAT4 mQuaternionWorld;



		
	/*	DirectX::XMFLOAT4X4 mScalegLocalMatrix;
		DirectX::XMFLOAT4X4 mRotationLocalMatrix;
		DirectX::XMFLOAT4X4 mTransitionLocalMstrix;*/

		DirectX::XMFLOAT4X4 mWorldMatrixF;
		DirectX::XMFLOAT4X4 mLocalMatrixF;



		//오일러
		DirectX::XMFLOAT3 mLookLocal;
		DirectX::XMFLOAT3 mLookWorld;

		DirectX::XMFLOAT3 mRightLocal;
		DirectX::XMFLOAT3 mRightWorld;

		DirectX::XMFLOAT3 mUpLocal;
		DirectX::XMFLOAT3 mUpWorld;

		//쿼터니언


		//부모의 변환정보에 영향을 받는지에대한 플래그
		bool mIndependentTransformFlag;

		//부모의 월드변환에서 스케일,회전,이동의 영향을 받는지에대한 개별 플래그  
		bool mIndependentScaleFlag;
		bool mIndependentRotationFlag;
		bool mIndependentTransitionFlag;
		//더티플래그
		bool mDirtyFlag;


		//DirectX::XMFLOAT4X4 mPreParentWorldMatrixF;
	

		float mDefaultWidth;
		float mDefaultHeight;
		float mDefaultDepth;
		//DirectX::XMFLOAT3 mDimension

		float mWidthWorld;
		float mHeightWorld;
		float mDepthWorld;


		DirectX::XMFLOAT3 mDefaultLook;
		DirectX::XMFLOAT3 mDefaultUp;
		DirectX::XMFLOAT3 mDefaultRight;


	//	std::vector <std::pair<Object*, void(*)(Object*)>> mTransformChangeCallbackTable;
		//std::vector <std::function<void()>> mTransformChangeCallbackTable;
		HandleTable<std::function<void()>> mTransformChangeCallbackTable;
		bool mTransformChangeEventFlag = true;		//true이면 등록된 콜백들이 호출된다.


		bool mDirtyFlagOnOff = false; 
		SceneElement* mDestSceneElement;








	};

}