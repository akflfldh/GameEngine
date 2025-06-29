#include "Core/Transform.h"
#include"Utility/Utility.h"
#include<cmath>
#include"Utility/MathHelper.h"
#include"Core/System.h"
//#include"TextCharacterPanelEntity.h"
#include"Parser/JsonParser.h"
#include"Core/SceneElement.h"




namespace Quad
{
    Transform::Transform()
        :mDirtyFlag(false), mTransformLocalDirtyCount(0)
    {
       
        DirectX::XMStoreFloat4x4(&mTransformLocalMatrixF, DirectX::XMMatrixIdentity());

        mPositionLocal = { 0,0,0 };

        mLookLocal = { 0,0,1.0f };

        mRightLocal = { 1.0f,0,0 };

        mUpLocal = { 0,1.0f,0 };

        mScaleLocal = { 1.0f,1.0f,1.0f };

        mQuaternionLocal = { 0.0f,0.0f,0.0f,1.0f };

        mDefaultLook = { 0.0f,0.0f,1.0f };
        mDefaultUp = { 0.0f,1.0f,0.0f };
        mDefaultRight = { 1.0f,0.0f,0.0f };

    }
  

    DirectX::XMFLOAT3 Transform::GetPositionLocal() const
    {
        return mPositionLocal;
    }



    void Transform::SetPositionLocal(const DirectX::XMFLOAT3  & positionF)
    {
        //새로운 로컬변환행렬을 계산한다.
        mPositionLocal = positionF;
        SetDirtyFlag(true);
        mTransformLocalDirtyCount++;
    }

    void Transform::SetPositionLocal(float x, float y, float z)
    {
        SetPositionLocal({ x,y,z });
    }

    void Transform::TranslateLocal(const DirectX::XMFLOAT3& translate)
    {
        DirectX::XMFLOAT3 pos = GetPositionLocal();
        DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&translate)));
        SetPositionLocal(pos);
        SetDirtyFlag(true);
        mTransformLocalDirtyCount++;
    }



    void Transform::SetScaleLocal(const DirectX::XMFLOAT3 & scaleF)
    {  
        mScaleLocal = scaleF;
        SetDirtyFlag(true);
        mTransformLocalDirtyCount++;
    }



    void Transform::SetRotationLocal(const DirectX::XMFLOAT3& rotation)
    {
        DirectX::XMFLOAT3 tempRotation = rotation;


        DirectX::XMVECTOR rotationRadian = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&tempRotation), DirectX::XM_PI / 180.0F);

        
        DirectX::XMFLOAT4 quaternion;
        DirectX::XMStoreFloat4(&quaternion, DirectX::XMQuaternionRotationRollPitchYawFromVector(rotationRadian));
        SetQuaternionLocal(quaternion);

    }

    void Transform::SetQuaternionLocal(const DirectX::XMFLOAT4& quaternion)
    {
        DirectX::XMStoreFloat4(&mQuaternionLocal, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&quaternion)));

        SetDirtyFlag(true);
        mTransformLocalDirtyCount++;

    }

    void Transform::SetTransformLocal(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& quaternion, const DirectX::XMFLOAT3& position)
    {
        //새로운 로컬변환행렬을 계산한다.
        mScaleLocal = scale;
        mQuaternionLocal = quaternion;//쿼터니언정규화가필요할수도?
        mPositionLocal = position;
        SetDirtyFlag(true);
        mTransformLocalDirtyCount++;
    }

    void Transform::SetTransformLocal(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& position)
    {
        mScaleLocal = scale;
        mPositionLocal = position;
        SetRotationLocal(rotation);  
    }

    void XM_CALLCONV Transform::SetTransformLocal(DirectX::FXMMATRIX localMatrix)
    {
      
        DirectX::XMVECTOR scaleLocal;
        DirectX::XMVECTOR quaternionLocal;
        DirectX::XMVECTOR translationLocal;

        DirectX::XMMatrixDecompose(&scaleLocal, &quaternionLocal, &translationLocal, localMatrix);

        DirectX::XMStoreFloat3(&mScaleLocal, scaleLocal);
        DirectX::XMStoreFloat4(&mQuaternionLocal, quaternionLocal);
        DirectX::XMStoreFloat3(&mPositionLocal, translationLocal);

        DirectX::XMStoreFloat4x4(&mTransformLocalMatrixF, localMatrix);

        SetDirtyFlag(true);
        mTransformLocalDirtyCount++;

        
    }

    void Transform::SetTransformLocal(const DirectX::XMFLOAT4X4& localMatrix)
    {
        SetTransformLocal(DirectX::XMLoadFloat4x4(&localMatrix));

    }

    DirectX::XMFLOAT4X4 Transform::GetTransformLocal() const
    {
        if (GetDirtyFlag())
        {
            DirectX::XMMATRIX localMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&mScaleLocal)),
                DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&mQuaternionLocal))),
                DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&mPositionLocal)));

            DirectX::XMStoreFloat4x4(&mTransformLocalMatrixF, localMatrix);
        }

        return mTransformLocalMatrixF;
    }





    



    DirectX::XMFLOAT3 Transform::GetScaleLocal() const
    {
        return mScaleLocal;
    }

    
    DirectX::XMFLOAT3 Transform::GetRoataiontLocal() const
    {
        //쿼터니언을 오일러각으로 변환한다.
        return  MathHelper::ConvertQuaternionToEulerAngle(mQuaternionLocal);
    }

    

    const DirectX::XMFLOAT4 & Transform::GetQuaternionLocal() const
    {
        return mQuaternionLocal;
    }

   

    DirectX::XMFLOAT3 Transform::GetLookLocal() const
    {
        
        DirectX::XMFLOAT3 defaultLook;

        DirectX::XMStoreFloat3(&defaultLook,DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 0, 1.0f, 0), DirectX::XMLoadFloat4(&mQuaternionLocal)));

        return defaultLook;

    }

 

    DirectX::XMFLOAT3 Transform::GetRightLocal() const
    {
        DirectX::XMFLOAT3 defaultRight;

        DirectX::XMStoreFloat3(&defaultRight, DirectX::XMVector3Rotate(DirectX::XMVectorSet(1, 0, 0.0f, 0), DirectX::XMLoadFloat4(&mQuaternionLocal)));

        return defaultRight;
    }

  

    DirectX::XMFLOAT3 Transform::GetUpLocal() const
    {
        DirectX::XMFLOAT3 defaultUp;

        DirectX::XMStoreFloat3(&defaultUp, DirectX::XMVector3Rotate(DirectX::XMVectorSet(0, 1, 0.0f, 0), DirectX::XMLoadFloat4(&mQuaternionLocal)));

        return defaultUp;
    }

   

    //void Transform::UpdateWorldMatrix(const DirectX::XMFLOAT4X4 & parentWorldMatrixF)
    //{


    //    if (GetIndependentTransformFlag())
    //    {
    //        //이부분 다시봐야한다.제대로
    //        mWorldMatrixF = mLocalMatrixF;
    //        return;
    //    }
    //    
    //    DirectX::XMMATRIX parentWorldMatrix = DirectX::XMLoadFloat4x4(&parentWorldMatrixF);
    //    if (GetIndependentScaleFlag() || GetIndependentRotationFlag() || GetIndependentTransitionFlag())
    //    {
    //        DirectX::XMVECTOR parentScaleWorld;
    //        DirectX::XMVECTOR parentQuaternionWorld;
    //        DirectX::XMVECTOR parentTranslationWorld;

    //        DirectX::XMMatrixDecompose(&parentScaleWorld, &parentQuaternionWorld, &parentTranslationWorld, parentWorldMatrix);

    //        ///parentWorldMatrix = DirectX::XMMatrixIdentity();
    //        if (!GetIndependentScaleFlag())
    //        {
    //            //parentWorldMatrix=DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixScalingFromVector(parentScaleWorld));
    //            parentWorldMatrix = DirectX::XMMatrixScalingFromVector(parentScaleWorld);
    //        }
    //        else
    //        {
    //            parentWorldMatrix = DirectX::XMMatrixIdentity();
    //        }

    //        if (!GetIndependentRotationFlag())
    //        {
    //            parentWorldMatrix = DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixRotationQuaternion(parentQuaternionWorld));
    //        }

    //        if (!GetIndependentTransitionFlag())
    //        {
    //            parentWorldMatrix = DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixTranslationFromVector(parentTranslationWorld));
    //        }

    //    }
    //   
    //    //새로운 월드변환행렬을 계산한다.
    //    DirectX::XMMATRIX childWorldMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&mLocalMatrixF), parentWorldMatrix);
    //    DirectX::XMStoreFloat4x4(&mWorldMatrixF, childWorldMatrix);

    //    //성분을 추출하여 월드속성값을 설정한다.
    //    DirectX::XMVECTOR childScaleWorldVector;
    //    DirectX::XMVECTOR childQuaternionWorldVector;
    //    DirectX::XMVECTOR childTranslationWorldVector;

    //    DirectX::XMMatrixDecompose(&childScaleWorldVector, &childQuaternionWorldVector, &childTranslationWorldVector, childWorldMatrix);
    //    DirectX::XMStoreFloat3(&mScaleWorld, childScaleWorldVector);
    //    DirectX::XMStoreFloat4(&mQuaternionWorld, childQuaternionWorldVector);
    //    DirectX::XMStoreFloat3(&mPositionWorld, childTranslationWorldVector);
    //  
    //   //방향
    //   //update방향
    //   UpdateDirectionVector();
    //


    //  // InvokeChangeEventCallback();
    //   SetDirtyFlag(true);


    //}


    bool Transform::GetDirtyFlag() const
    {
        return mDirtyFlag;
    }


    //DirectX::XMMATRIX XM_CALLCONV Transform::CalculateWorldMatrix()
    //{
    // /*   DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTransformation(DirectX::XMVectorSet(0, 0, 0, 0), DirectX::XMVectorSet(0, 0, 0, 1),
    //        DirectX::XMLoadFloat3(&mScaleWorld), DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMLoadFloat4(&mQuaternionWorld), DirectX::XMLoadFloat3(&mPositionWorld));*/

    //    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&mScaleWorld)),
    //        DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&mQuaternionWorld))),
    //        DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&mPositionWorld)));
    //    DirectX::XMStoreFloat4x4(&mWorldMatrixF, worldMatrix);

    //    return worldMatrix;
    //}

    void Transform::UpdateDirectionVector()
    {
        auto updateDirectionVectorlamda = [](const DirectX::XMFLOAT4& quaternion, DirectX::XMFLOAT3& oLook,
            DirectX::XMFLOAT3& oUp, DirectX::XMFLOAT3& oRight) {

                DirectX::XMVECTOR look = DirectX::XMVectorSet(0, 0, 1, 0);
                DirectX::XMVECTOR right = DirectX::XMVectorSet(1, 0, 0, 0);
                DirectX::XMVECTOR up = DirectX::XMVectorSet(0, 1, 0, 0);
                DirectX::XMMATRIX quaternionLocalMatrix = DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&quaternion));

                look = DirectX::XMVector3Transform(look, quaternionLocalMatrix);
                up = DirectX::XMVector3Transform(up, quaternionLocalMatrix);
                right = DirectX::XMVector3Cross(up, look);

                look = DirectX::XMVector3Normalize(look);
                up = DirectX::XMVector3Normalize(up);
                right = DirectX::XMVector3Normalize(right);


                DirectX::XMStoreFloat3(&oLook, look);
                DirectX::XMStoreFloat3(&oRight, right);
                DirectX::XMStoreFloat3(&oUp, up);
        };

        updateDirectionVectorlamda(mQuaternionLocal, mLookLocal, mUpLocal, mRightLocal);
        //updateDirectionVectorlamda(mQuaternionWorld, mLookWorld, mUpWorld, mRightWorld);




    }


    //void Transform::CalculateNewLocalMatrix(DirectX::FXMMATRIX parentWorldMatrix)
    //{


    //    //월드변환행렬을 계산한다.
    //     //월드속성값을 이용해서
    //    DirectX::XMMATRIX childWorldMatrix = CalculateWorldMatrix();


    //    //로컬변환행렬을 계산한다.
    //    DirectX::XMMATRIX childLocalMatrix = DirectX::XMMatrixMultiply(childWorldMatrix, DirectX::XMMatrixInverse(nullptr, parentWorldMatrix));
    //    DirectX::XMStoreFloat4x4(&mLocalMatrixF, childLocalMatrix);

    //    DirectX::XMVECTOR childScaleLocal;
    //    DirectX::XMVECTOR childQuaternionLocal;
    //    DirectX::XMVECTOR childTranslationLocal;

    //    //속성값 설정
    //    DirectX::XMMatrixDecompose(&childScaleLocal, &childQuaternionLocal, &childTranslationLocal, childLocalMatrix);
    //    DirectX::XMStoreFloat3(&mScaleLocal, childScaleLocal);
    //    DirectX::XMStoreFloat4(&mQuaternionLocal, childQuaternionLocal);
    //    DirectX::XMStoreFloat3(&mPositionLocal, childTranslationLocal);
    //    SetDirtyFlag(true);
    // //   CallTransformChangeCallback();

    //}

  /*  void Transform::CallTransformChangeCallback()
    {
        const std::vector<QHANDLE> handleVector = mTransformChangeCallbackHandleTable.GetUsingHandleVector();


        for (int i = 0; i < handleVector.size(); ++i)
        {

            const auto& callback = mTransformChangeCallbackHandleTable.GetValue(handleVector[i]);
            (*callback)();
        }


    }*/

    void Transform::Serialize(const std::string & tag)
    {
        JsonParser::Write("Transform_PositionLocal"+ tag, mPositionLocal);
       
        JsonParser::Write("Transform_QuaternionLocal"+ tag, mQuaternionLocal);
      
        JsonParser::Write("Transform_ScaleLocal"+ tag, mScaleLocal);
       


        JsonParser::Write("Transform_LookLocal"+ tag, mLookLocal);
       
        JsonParser::Write("Transform_RightLocal"+ tag, mRightLocal);
      

        JsonParser::Write("Transform_UpLocal"+tag, mUpLocal);
       
    }

    void Transform::DeSerialize(const std::string& tag)
    {
        JsonParser::Read("Transform_PositionLocal"+ tag, mPositionLocal);
      

        JsonParser::Read("Transform_QuaternionLocal"+ tag, mQuaternionLocal);
     

        JsonParser::Read("Transform_ScaleLocal"+ tag, mScaleLocal);
      


        JsonParser::Read("Transform_LookLocal"+ tag, mLookLocal);
     

        JsonParser::Read("Transform_RightLocal"+ tag, mRightLocal);
      

        JsonParser::Read("Transform_UpLocal"+ tag, mUpLocal);

    }

    unsigned long long Transform::GetTransformLocalDirtyCount() const
    {
        return mTransformLocalDirtyCount;
    }

    void Transform::SetDirtyFlag(bool flag)
    {
        mDirtyFlag = flag;
 
    }

    //void Transform::CalculateNewWorldMatrix(DirectX::FXMMATRIX parentWorldMatrx)
    //{

    //    DirectX::XMMATRIX childLocalMatrix = CalculateLocalMatrix();

    //    //새로운 월드변환행렬을 계산한다.
    //    DirectX::XMMATRIX childWorldMatrix = DirectX::XMMatrixMultiply(childLocalMatrix, parentWorldMatrx);
    //    DirectX::XMStoreFloat4x4(&mWorldMatrixF, childWorldMatrix);

    //    //성분을 추출하여 월드속성값을 설정한다.
    //    DirectX::XMVECTOR childScaleWorldVector;
    //    DirectX::XMVECTOR childQuaternionWorldVector;
    //    DirectX::XMVECTOR childTranslationWorldVector;

    //    DirectX::XMMatrixDecompose(&childScaleWorldVector, &childQuaternionWorldVector, &childTranslationWorldVector, childWorldMatrix);
    //    DirectX::XMStoreFloat3(&mScaleWorld, childScaleWorldVector);
    //    DirectX::XMStoreFloat4(&mQuaternionWorld, childQuaternionWorldVector);
    //    DirectX::XMStoreFloat3(&mPositionWorld, childTranslationWorldVector);
    //    SetDirtyFlag(true);
    //   // CallTransformChangeCallback();
    //}






}