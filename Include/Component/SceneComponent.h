#pragma once



#include"BaseComponent.h"
#include"Transform.h"


#include<unordered_map>
#include<Core/CoreDllExport.h>
namespace Quad
{
	class CORE_API_LIB SceneComponent:public BaseComponent
	{
	public:
		SceneComponent();
		virtual ~SceneComponent();
		virtual void Start();//게임플레이 시작시한번호출
		virtual void Update(float delta);//게임플레이 매프레임 호출
			

		Transform & GetTransform() const;
		DirectX::XMFLOAT4X4 GetTransformWorldMatrixF() const;
		void SetTransformWorldMatrixF(const DirectX::XMFLOAT4X4& worldMatirx);
		void SetTransformWorld(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& rotation, const DirectX::XMFLOAT3 & translation);

		void AttachToComponent(SceneComponent* parentSceneComponent);
		

		SceneComponent* GetParentSceneComponent() const;


		void SetScaleWorld(const DirectX::XMFLOAT3& scale);
		void SetRotationWorld(const DirectX::XMFLOAT4& rotation);
		void SetPositionWorld(const DirectX::XMFLOAT3& position);

		
		DirectX::XMFLOAT3 GetScaleWorld() const;
		DirectX::XMFLOAT4 GetRotationWorld() const;
		DirectX::XMFLOAT3 GetRotationEulerWorld() const;
		DirectX::XMFLOAT3 GetTranslationWorld() const;

		
		DirectX::XMFLOAT3 GetLookWorld() const;
		DirectX::XMFLOAT3 GetRightWorld() const;
		DirectX::XMFLOAT3 GetUpWorld() const;






		void SetIndependentScaleFlag(bool flag);
		void SetIndependentRotationFlag(bool flag);
		void SetIndependentTranslationFlag(bool flag);


		bool GetIndependentScaleFlag() const;
		bool GetIndependentRotationFlag() const;
		bool GetIndependentTranslationFlag()const;


		void UpdateNewChildComponentLocal(SceneComponent* childComponent);//새로추가된 자식컴포넌트의 local transform을 올바르게 업데이트한다.


		CLASSNAME(SceneComponent)
		CLASSNAMESTATIC(SceneComponent)



	private:

		SceneComponent* AddComponent(const char * componentTypeName, const char* componentName);
		SceneComponent* GetComponent(const char* componentName) const;

		void SetParentSceneComponent(SceneComponent* parentSceneComponent);


		void IncrementWorldMatrixDirtyCount() const;
		unsigned long long GetWorldMatrixDirtyCount() const;

		unsigned long long GetParentWorldMatrixDirtyCount() const;
		void SetParentWorldMatrixDirtyCount(unsigned long long i) const;
		void SetLocalMatrixDirtyCount(unsigned long long i) const;


		void SetWorldMatrix(DirectX::FXMVECTOR scale, DirectX::FXMVECTOR rotation, DirectX::FXMVECTOR translation);

	private:

		void UpdateWorldMatrix(const DirectX::XMFLOAT4X4& parentWorldMatrix, const DirectX::XMFLOAT4X4& localMatrix) const;
		



	private:
		mutable Transform mTransform;
		SceneComponent* mParentSceneComponent;

		mutable DirectX::XMFLOAT4X4 mWorldMatrixF;
		bool mWorldMatrixDirtyFlag;

		mutable unsigned long long mWorldMatrixDirtyCount;		//자신의 월드변환 dirtyCount, 월드변환이 갱신될때마다 증가된다.

		mutable unsigned long long mParentWorldMatrixDirtyCount;//부모의 월드변환을 dirty가 true여서 가져왔을떄의 마지막 count값
		//만약 부모의 dirty flag가 false인데도 자식이 기록하고있는 count보다 크다면 그것은 다른자식 또는 외부로 인해 월드변환값이 다시계산되었다는 의미임으로 부모의 변환행렬을 자식의 월드변환행렬을 계산하는데 적용해야한다.

		mutable unsigned long long mLocalMatrixDirtyCount;	//자신의 로컬변환 dirtyCount, 외부에서 트랜스폼을 얻어 로컬변환행렬을 사용할려고
		//업데이트 되었을수도있느니 정확히 count로 비교해야한다.



		bool mIndependentScaleFlag;
		bool mIndependentRotationFlag;
		bool mIndependentTranslationFlag;





	};
	REGISTERCOMPONENTCLASS(SceneComponent)




}
