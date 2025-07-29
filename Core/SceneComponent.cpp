#include "UIRectTransform.h"
#include "Component/SceneComponent.h"
#include"Object/Object.h"


#include"Utility/Utility.h"
#include"Utility/MathHelper.h"


Quad::SceneComponent::SceneComponent()
    :BaseComponent(EComponentType::ESceneComponentType),
    mWorldMatrixDirtyFlag(false), mWorldMatrixDirtyCount(0), mParentWorldMatrixDirtyCount(0), mLocalMatrixDirtyCount(0), mParentSceneComponent(nullptr),mIndependentScaleFlag(false),mIndependentRotationFlag(false), mIndependentTranslationFlag(false)
    
{
  

}
void Quad::SceneComponent::Start()
{

}

void  Quad::SceneComponent::Update(float delta)
{

}


Quad::SceneComponent::~SceneComponent()
{
}
Quad::Transform& Quad::SceneComponent::GetTransform() const
{
    return mTransform;
}

DirectX::XMFLOAT4X4 Quad::SceneComponent::GetTransformWorldMatrixF() const
{


    //자신의 로컬 더티 카운트를 확인
    // true라면 부모의 dirty와는 상관없이 무조건 update, 

    //false라면 부모의 dirty의 여부로 결정

    bool updateFlag = false;
   

    if (mTransform.GetTransformLocalDirtyCount() != mLocalMatrixDirtyCount)
    {
        updateFlag = true;
    }


    //부모 씬컴포넌트 get
    DirectX::XMFLOAT4X4 parentWorldMatrixF =Utility::GetIdentityMatrixF();
    SceneComponent* parentSceneComponent = GetParentSceneComponent();
    if (!parentSceneComponent)
    {
        //부모씬컴포넌트가없는 씬컴포넌트이다. 즉 루트씬컴포넌트이다.
        Object* parentObject = GetDestObject()->GetParentObject();  //그경우 소유오브젝트의 부모오브젝트의 월드행렬을 얻을것이다.
        if (parentObject)
        {
            parentSceneComponent  = parentObject->GetRootSceneComponent();
        }
    }


    if (parentSceneComponent)
    {
        parentWorldMatrixF=  parentSceneComponent->GetTransformWorldMatrixF();
        if (parentSceneComponent->GetWorldMatrixDirtyCount() != GetParentWorldMatrixDirtyCount())   //부모에서 worldMatrix를얻어온후 dirtyCount확인
            updateFlag = true;
    }



    if (updateFlag)
    {
        //월드 계산수행
        DirectX::XMFLOAT4X4 transformLocalMatrix = mTransform.GetTransformLocal();
        UpdateWorldMatrix(parentWorldMatrixF, transformLocalMatrix);

        if (parentSceneComponent)
            SetParentWorldMatrixDirtyCount(parentSceneComponent->GetWorldMatrixDirtyCount());
        SetLocalMatrixDirtyCount(mTransform.GetTransformLocalDirtyCount());
     
    }


    return mWorldMatrixF;


    

}

void Quad::SceneComponent::SetTransformWorldMatrixF(const DirectX::XMFLOAT4X4& worldMatirx)
{

    if (mIndependentRotationFlag && mIndependentScaleFlag && mIndependentTranslationFlag)
    {
        mTransform.SetTransformLocal(worldMatirx);
        return;
    }
    
    DirectX::XMVECTOR scale;
    DirectX::XMVECTOR rotation;
    DirectX::XMVECTOR translation;

  
    DirectX::XMFLOAT3 scaleF;
    DirectX::XMFLOAT4 rotationF;
    DirectX::XMFLOAT3 translationF;


    DirectX::XMMatrixDecompose(&scale, &rotation, &translation,DirectX::XMLoadFloat4x4(&worldMatirx));

 
    SetWorldMatrix(scale, rotation, translation);




    

}

void Quad::SceneComponent::SetTransformWorld(const DirectX::XMFLOAT3& scaleF, const DirectX::XMFLOAT4& rotationF, const DirectX::XMFLOAT3 & translationF)
{

    if (mIndependentRotationFlag && mIndependentScaleFlag && mIndependentTranslationFlag)
    {
        mTransform.SetTransformLocal(scaleF, rotationF, translationF);
        return;
    }

    SetWorldMatrix(DirectX::XMLoadFloat3(&scaleF), DirectX::XMLoadFloat4(&rotationF), DirectX::XMLoadFloat3(&translationF));
}

void Quad::SceneComponent::AttachToComponent(SceneComponent* parentSceneComponent)
{
    if (parentSceneComponent == nullptr)
        return;

    mParentSceneComponent = parentSceneComponent;

    mParentWorldMatrixDirtyCount = 0;
    GetTransformWorldMatrixF();     //부착된 맨처음 world matrix와 dirtyCount update

}

Quad::SceneComponent* Quad::SceneComponent::GetParentSceneComponent() const
{
    return mParentSceneComponent;
}

void Quad::SceneComponent::SetScaleWorld(const DirectX::XMFLOAT3& scale)
{
    if (mIndependentScaleFlag)
    {
        mTransform.SetScaleLocal(scale);
        return;
    }

    // 부모 월드 get
    SceneComponent* parentSceneComponent = mParentSceneComponent;
    if (parentSceneComponent == nullptr)
    {
        parentSceneComponent = GetDestObject()->GetParentObject()->GetRootSceneComponent();
    }

   DirectX::XMFLOAT4X4 parentWorldMatrix =  parentSceneComponent->GetTransformWorldMatrixF();
    
   DirectX::XMVECTOR parentScaleWorld;
   DirectX::XMVECTOR parentRotationWorld;
   DirectX::XMVECTOR parentTranslationWorld;
   DirectX::XMMatrixDecompose(&parentScaleWorld, &parentRotationWorld, &parentTranslationWorld, DirectX::XMLoadFloat4x4(&parentWorldMatrix));


    //나의 local get
   DirectX::XMFLOAT3 scaleLocal;

    
    DirectX::XMStoreFloat3(&scaleLocal,DirectX::XMVectorDivide(DirectX::XMLoadFloat3(&scale), parentScaleWorld));


    mTransform.SetScaleLocal(scaleLocal);


}

void  Quad::SceneComponent::SetRotationWorld(const DirectX::XMFLOAT4& rotation)
{
    if (mIndependentRotationFlag)
    {
        mTransform.SetQuaternionLocal(rotation);
        return;
    }

    // 부모 월드 get
    SceneComponent* parentSceneComponent = mParentSceneComponent;
    if (parentSceneComponent == nullptr)
    {
        parentSceneComponent = GetDestObject()->GetParentObject()->GetRootSceneComponent();
    }

    DirectX::XMFLOAT4X4 parentWorldMatrix = parentSceneComponent->GetTransformWorldMatrixF();

    DirectX::XMVECTOR parentScaleWorld;
    DirectX::XMVECTOR parentRotationWorld;
    DirectX::XMVECTOR parentTranslationWorld;
    DirectX::XMMatrixDecompose(&parentScaleWorld, &parentRotationWorld, &parentTranslationWorld, DirectX::XMLoadFloat4x4(&parentWorldMatrix));


    //켤레 단위 쿼터니언 
    DirectX::XMVECTOR parentRotationInverse = DirectX::XMQuaternionInverse(parentRotationWorld);

    //나의 local get
    DirectX::XMFLOAT4 rotationScale;

   DirectX::XMStoreFloat4(&rotationScale , DirectX::XMQuaternionMultiply(parentRotationInverse, DirectX::XMLoadFloat4(&rotation)));

 
    mTransform.SetQuaternionLocal(rotationScale);





}

void Quad::SceneComponent::SetPositionWorld(const DirectX::XMFLOAT3& position)
{

    if (mIndependentTranslationFlag)
    {
        mTransform.SetPositionLocal(position);
        return;
    }

    // 부모 월드 get
    SceneComponent* parentSceneComponent = mParentSceneComponent;
    if (parentSceneComponent == nullptr)
    {
        parentSceneComponent = GetDestObject()->GetParentObject()->GetRootSceneComponent();
    }

    DirectX::XMFLOAT4X4 parentWorldMatrix = parentSceneComponent->GetTransformWorldMatrixF();

    DirectX::XMVECTOR parentScaleWorld;
    DirectX::XMVECTOR parentRotationWorld;
    DirectX::XMVECTOR parentTranslationWorld;
    DirectX::XMMatrixDecompose(&parentScaleWorld, &parentRotationWorld, &parentTranslationWorld, DirectX::XMLoadFloat4x4(&parentWorldMatrix));


    //나의 local get
    DirectX::XMFLOAT3 translationLocal;


    DirectX::XMStoreFloat3(&translationLocal,DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&position), parentTranslationWorld));


    mTransform.SetPositionLocal(translationLocal);


}


DirectX::XMFLOAT3 Quad::SceneComponent::GetScaleWorld() const
{

    DirectX::XMFLOAT4X4 worldMatrixF =   GetTransformWorldMatrixF();


    DirectX::XMVECTOR scaleWorld;
    DirectX::XMVECTOR rotationWorld;
    DirectX::XMVECTOR translationWorld;
    DirectX::XMMatrixDecompose(&scaleWorld, &rotationWorld, &translationWorld,DirectX::XMLoadFloat4x4(&worldMatrixF));


    DirectX::XMFLOAT3 worldF;
    DirectX::XMStoreFloat3(&worldF, scaleWorld);
    
    return worldF;

}

DirectX::XMFLOAT4 Quad::SceneComponent::GetRotationWorld() const
{
   
    DirectX::XMFLOAT4X4 worldMatrixF = GetTransformWorldMatrixF();
    DirectX::XMVECTOR scaleWorld;
    DirectX::XMVECTOR rotationWorld;
    DirectX::XMVECTOR translationWorld;
    DirectX::XMMatrixDecompose(&scaleWorld, &rotationWorld, &translationWorld, DirectX::XMLoadFloat4x4(&worldMatrixF));


    DirectX::XMFLOAT4 worldF;
    DirectX::XMStoreFloat4(&worldF, rotationWorld);

    return worldF;

}

DirectX::XMFLOAT3 Quad::SceneComponent::GetRotationEulerWorld() const
{

   DirectX::XMFLOAT4 quaternion =  GetRotationWorld();
   return  MathHelper::ConvertQuaternionToEulerAngle(quaternion);

}

DirectX::XMFLOAT3 Quad::SceneComponent::GetTranslationWorld() const
{
    

    DirectX::XMFLOAT4X4 worldMatrixF = GetTransformWorldMatrixF();
    DirectX::XMVECTOR scaleWorld;
    DirectX::XMVECTOR rotationWorld;
    DirectX::XMVECTOR translationWorld;
    DirectX::XMMatrixDecompose(&scaleWorld, &rotationWorld, &translationWorld, DirectX::XMLoadFloat4x4(&worldMatrixF));


    DirectX::XMFLOAT3 worldF;
    DirectX::XMStoreFloat3(&worldF, translationWorld);

    return worldF;



}

DirectX::XMFLOAT3 Quad::SceneComponent::GetLookWorld() const
{
    DirectX::XMFLOAT4X4 worldTransformMatrixF= GetTransformWorldMatrixF();

    DirectX::XMVECTOR look = 
        DirectX::XMVectorSet(worldTransformMatrixF._31, worldTransformMatrixF._32, worldTransformMatrixF._33,0.0f);

    DirectX::XMFLOAT3 lookF;
    DirectX::XMStoreFloat3(&lookF,DirectX::XMVector3Normalize(look));



    return lookF;


}

DirectX::XMFLOAT3 Quad::SceneComponent::GetRightWorld() const
{
  
    DirectX::XMFLOAT4X4 worldTransformMatrixF = GetTransformWorldMatrixF();

    DirectX::XMVECTOR right =
        DirectX::XMVectorSet(worldTransformMatrixF._11, worldTransformMatrixF._12, worldTransformMatrixF._13, 0.0f);

    DirectX::XMFLOAT3 rightF;
    DirectX::XMStoreFloat3(&rightF, DirectX::XMVector3Normalize(right));



    return rightF;






}

DirectX::XMFLOAT3 Quad::SceneComponent::GetUpWorld() const
{
  
    DirectX::XMFLOAT4X4 worldTransformMatrixF = GetTransformWorldMatrixF();

    DirectX::XMVECTOR up =
        DirectX::XMVectorSet(worldTransformMatrixF._21, worldTransformMatrixF._22, worldTransformMatrixF._23, 0.0f);

    DirectX::XMFLOAT3 upF;
    DirectX::XMStoreFloat3(&upF, DirectX::XMVector3Normalize(up));



    return upF;



}



void Quad::SceneComponent::SetIndependentScaleFlag(bool flag)
{
    mIndependentScaleFlag = flag;
}

void Quad::SceneComponent::SetIndependentRotationFlag(bool flag)
{
    mIndependentRotationFlag = flag;
}

void Quad::SceneComponent::SetIndependentTranslationFlag(bool flag)
{
    mIndependentTranslationFlag = flag;
}

bool Quad::SceneComponent::GetIndependentScaleFlag() const
{
    return mIndependentScaleFlag;
}

bool Quad::SceneComponent::GetIndependentRotationFlag() const
{
    return mIndependentRotationFlag;
}

bool Quad::SceneComponent::GetIndependentTranslationFlag() const
{
    return mIndependentTranslationFlag;
}




void Quad::SceneComponent::SetParentSceneComponent(SceneComponent* parentSceneComponent)
{

    mParentSceneComponent = parentSceneComponent;
}

void Quad::SceneComponent::IncrementWorldMatrixDirtyCount() const
{
    mWorldMatrixDirtyCount++;
}

unsigned long long Quad::SceneComponent::GetWorldMatrixDirtyCount() const
{
    return mWorldMatrixDirtyCount;
}

unsigned long long Quad::SceneComponent::GetParentWorldMatrixDirtyCount() const
{
    return mParentWorldMatrixDirtyCount;
}

void Quad::SceneComponent::SetParentWorldMatrixDirtyCount(unsigned long long i) const
{
    mParentWorldMatrixDirtyCount = i;
}

void Quad::SceneComponent::SetLocalMatrixDirtyCount(unsigned long long i) const
{
    mLocalMatrixDirtyCount = i;
}

void Quad::SceneComponent::SetWorldMatrix(DirectX::FXMVECTOR newScaleWorld, DirectX::FXMVECTOR newRotationWorld, DirectX::FXMVECTOR newTranslationWorld)
{



    SceneComponent* parentSceneComponent = mParentSceneComponent;
    if (parentSceneComponent == nullptr)
    {
        parentSceneComponent = GetDestObject()->GetParentObject()->GetRootSceneComponent();
    }

    DirectX::XMFLOAT4X4 parentWorldMatrix = parentSceneComponent->GetTransformWorldMatrixF();

   
    DirectX::XMVECTOR parentScaleWorld;
    DirectX::XMVECTOR parentRotationWorld;
    DirectX::XMVECTOR parentTranslationWorld;
   
    DirectX::XMMatrixDecompose(&parentScaleWorld, &parentRotationWorld, &parentTranslationWorld, DirectX::XMLoadFloat4x4(&parentWorldMatrix));
        

    DirectX::XMVECTOR newScaleLocal = DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::XMVECTOR newRotationLocal = DirectX::XMQuaternionIdentity();
    DirectX::XMVECTOR newTranslationLocal = DirectX::XMVectorZero();

    if (mIndependentScaleFlag == false)
    {
        newScaleLocal = DirectX::XMVectorDivide(newScaleWorld, parentScaleWorld);
    }

    if (mIndependentRotationFlag == false)
    {
        newRotationLocal = DirectX::XMQuaternionMultiply(DirectX::XMQuaternionInverse(parentRotationWorld), newRotationWorld);
    }

    if (mIndependentTranslationFlag == false)
    {
        newTranslationLocal = DirectX::XMVectorSubtract(newTranslationWorld, parentTranslationWorld);
    }


    DirectX::XMMATRIX newLocalMatrix = DirectX::XMMatrixTransformation(DirectX::XMVectorSet(1, 1, 1, 1), DirectX::XMQuaternionIdentity(), newScaleLocal, DirectX::XMVectorZero(), newRotationLocal, newTranslationLocal);

    mTransform.SetTransformLocal(newLocalMatrix);



}

void Quad::SceneComponent::UpdateWorldMatrix(const DirectX::XMFLOAT4X4& parentWorldMatrix,const DirectX::XMFLOAT4X4& localMatrix) const
{



   

    // 이것을 IndependentFlag들을 고려하는 방식으로 전환해야한다.

    //만약 flag들이 모두 false라면 추가작업 없이 부모행렬과 로컬행렬 곱 끝
     
    bool independentScaleFlag = GetIndependentScaleFlag();
    bool independentRotationFlag = GetIndependentRotationFlag();
    bool independentTranslationFlag = GetIndependentTranslationFlag();

    
    if (independentScaleFlag || independentRotationFlag || independentTranslationFlag)
    {
        //하나의 flag라도 true라면 부모행렬을 s,r,t로 분리후, 
         //flag가 false인것만 곱해서 올바른 부모월드행렬 계산후 로컬행렬과 곱 끝
        DirectX::XMVECTOR scale;
        DirectX::XMVECTOR rotation;
        DirectX::XMVECTOR translation;

        DirectX::XMMatrixDecompose(&scale, &rotation, &translation, DirectX::XMLoadFloat4x4(&parentWorldMatrix));
        
        DirectX::XMMATRIX newParentWorldMatrix = DirectX::XMMatrixIdentity();

        if (independentScaleFlag == false)
        {
            newParentWorldMatrix = DirectX::XMMatrixScalingFromVector(scale);
        }
      
        if (independentRotationFlag == false)
        {
            newParentWorldMatrix = DirectX::XMMatrixMultiply(newParentWorldMatrix,DirectX::XMMatrixRotationQuaternion(rotation));

        }

        if (independentTranslationFlag == false)
        {
            newParentWorldMatrix =  DirectX::XMMatrixMultiply(newParentWorldMatrix,DirectX::XMMatrixTranslationFromVector(translation));
        }

        DirectX::XMStoreFloat4x4(&mWorldMatrixF, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&localMatrix),newParentWorldMatrix));
    }else
        DirectX::XMStoreFloat4x4(&mWorldMatrixF, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&localMatrix),DirectX::XMLoadFloat4x4(&parentWorldMatrix)));


    IncrementWorldMatrixDirtyCount();
}

void Quad::SceneComponent::UpdateNewChildComponentLocal(SceneComponent* childComponent)
{
    Transform& childComponentTransform = childComponent->GetTransform();
   // DirectX::XMFLOAT4X4 childTransformLocalMatrix =  childComponent->GetTransform().GetTransformLocal();

    DirectX::XMFLOAT4X4 childTransformWorldMatrixF = childComponent->GetTransformWorldMatrixF();

    DirectX::XMFLOAT4X4 parentTransformWorldMatrixF= GetTransformWorldMatrixF();

    DirectX::XMMATRIX parentTransformWorldInVMatrix = DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&parentTransformWorldMatrixF));


    childComponentTransform.SetTransformLocal(DirectX::XMMatrixMultiply(parentTransformWorldInVMatrix,
        DirectX::XMLoadFloat4x4(&childTransformWorldMatrixF)));



}



