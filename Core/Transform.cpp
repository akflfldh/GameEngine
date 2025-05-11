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
    Transform::Transform(SceneElement* destSceneElementt)
        :mDestSceneElement(destSceneElementt),mTransformChangeCallbackTable(100,nullptr),mDirtyFlag(false), mTransformChangeEventFlag(true)
    {
        
        //if(mDestSceneElement)


    }
    void Transform::Initialize()
    {
        DirectX::XMStoreFloat4x4(&mLocalMatrixF, DirectX::XMMatrixIdentity());
        DirectX::XMStoreFloat4x4(&mWorldMatrixF, DirectX::XMMatrixIdentity());
       //DirectX::XMStoreFloat4x4(&mPreParentWorldMatrixF, DirectX::XMMatrixIdentity());


        mPositionLocal = { 0,0,0 };
        mPositionWorld = mPositionLocal;

        mLookLocal = { 0,0,1.0f };
        mLookWorld = mLookLocal;

        mRightLocal = { 1.0f,0,0 };
        mRightWorld = mRightLocal;

        mUpLocal = { 0,1.0f,0 };
        mUpWorld = mUpLocal;

        mScaleLocal = { 1.0f,1.0f,1.0f };
        mScaleWorld = { 1.0f,1.0f,1.0f };
        mQuaternionLocal = { 0.0f,0.0f,0.0f,1.0f };
        mQuaternionWorld = { 0.0f,0.0f,0.0f,1.0f };



        mIndependentTransformFlag = false;
        mIndependentScaleFlag = false;
        mIndependentRotationFlag = false;
        mIndependentTransitionFlag = false;

         mDefaultWidth=1.0f;
         mDefaultHeight=1.0f;
         mDefaultDepth=1.0f;

         mWidthWorld=1.0f;
         mHeightWorld=1.0f;
         mDepthWorld=1.0f;

         mDefaultLook = { 0.0f,0.0f,1.0f };
         mDefaultUp = { 0.0f,1.0f,0.0f };
         mDefaultRight = { 1.0f,0.0f,0.0f };


    }

    void Transform::Update()
    {
        




    }

    DirectX::XMFLOAT3 Transform::GetPositionLocal() const
    {
        return mPositionLocal;
    }

    DirectX::XMFLOAT3 Transform::GetPositionWorld() const
    {
        return mPositionWorld;
    }

    void Transform::SetPositionLocal(const DirectX::XMFLOAT3  & positionF)
    {
        //부모변환행렬을 먼저얻는다
        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();
        //새로운 로컬변환행렬을 계산한다.
        mPositionLocal = positionF;
        CalculateNewWorldMatrix(parentWorldMatrix);


        if(GetTransformChangeEventFlag())
            InvokeChangeEventCallback();

    }

    void Transform::TranslateLocal(const DirectX::XMFLOAT3& translate)
    {
        DirectX::XMFLOAT3 pos = GetPositionLocal();
        DirectX::XMStoreFloat3(&pos, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&pos), DirectX::XMLoadFloat3(&translate)));
        SetPositionLocal(pos);
    }



    void Transform::SetScaleLocal(const DirectX::XMFLOAT3 & scaleF)
    {

        //부모변환행렬을 먼저얻는다
        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();
        //새로운 로컬변환행렬을 계산한다.
        mScaleLocal = scaleF;
        CalculateNewWorldMatrix(parentWorldMatrix);
        if (GetTransformChangeEventFlag())
        InvokeChangeEventCallback();
    }



    void Transform::SetRotationLocal(const DirectX::XMFLOAT3& rotation)
    {
        DirectX::XMFLOAT3 tempRotation = rotation;


        DirectX::XMVECTOR rotationRadian = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&tempRotation), DirectX::XM_PI / 180.0F);

        


        DirectX::XMFLOAT4 quaternion;
        DirectX::XMStoreFloat4(&quaternion, DirectX::XMQuaternionRotationRollPitchYawFromVector(rotationRadian));
        SetQuaternionLocal(quaternion);
        if (GetTransformChangeEventFlag())
        InvokeChangeEventCallback();
    }

    void Transform::SetQuaternionLocal(const DirectX::XMFLOAT4& quaternion)
    {

        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();
        //새로운 로컬변환행렬을 계산한다.

        DirectX::XMStoreFloat4(&mQuaternionLocal, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&quaternion)));
      //  mQuaternionLocal = quaternion;//쿼터니언정규화가필요할수도?

        CalculateNewWorldMatrix(parentWorldMatrix);


        if (GetTransformChangeEventFlag())
             InvokeChangeEventCallback();
        UpdateDirectionVector();


    }

    void Transform::SetTransformLocal(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT4& quaternion, const DirectX::XMFLOAT3& position)
    {
         DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();
        //새로운 로컬변환행렬을 계산한다.
        mScaleLocal = scale;
        mQuaternionLocal = quaternion;//쿼터니언정규화가필요할수도?
        mPositionLocal = position;
        CalculateNewWorldMatrix(parentWorldMatrix);
       
        if (GetTransformChangeEventFlag())
            InvokeChangeEventCallback();
        UpdateDirectionVector();



    }

    void Transform::SetTransformLocal(const DirectX::XMFLOAT3& scale, const DirectX::XMFLOAT3& rotation, const DirectX::XMFLOAT3& position)
    {
        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();
        mScaleLocal = scale;
        DirectX::XMVECTOR rotationRadian = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rotation), DirectX::XM_PI / 180.0F);
        DirectX::XMStoreFloat4(&mQuaternionLocal, DirectX::XMQuaternionRotationRollPitchYawFromVector(rotationRadian));
        mPositionLocal = position;
     
       
        CalculateNewWorldMatrix(parentWorldMatrix);

        if (GetTransformChangeEventFlag())
          InvokeChangeEventCallback();
        UpdateDirectionVector();
    }

    void XM_CALLCONV Transform::SetTransformLocal(DirectX::FXMMATRIX localMatrix)
    {
      
        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();

        DirectX::XMVECTOR scaleLocal;
        DirectX::XMVECTOR quaternionLocal;
        DirectX::XMVECTOR translationLocal;

        DirectX::XMMatrixDecompose(&scaleLocal, &quaternionLocal, &translationLocal, localMatrix);

        DirectX::XMStoreFloat3(&mScaleLocal, scaleLocal);
        DirectX::XMStoreFloat4(&mQuaternionLocal, quaternionLocal);
        DirectX::XMStoreFloat3(&mPositionLocal, translationLocal);

  
        CalculateNewWorldMatrix(parentWorldMatrix);
        if (GetTransformChangeEventFlag())
          InvokeChangeEventCallback();
        UpdateDirectionVector();
        
    }

    void Transform::SetScaleWorld(const DirectX::XMFLOAT3& scale)
    {


        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();
        mScaleWorld = scale;
        CalculateNewLocalMatrix(parentWorldMatrix);
      
        if (GetTransformChangeEventFlag())
        InvokeChangeEventCallback();

    }

    void Transform::SetPositionWorld(const DirectX::XMFLOAT3 & position)
    {
        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();
        mPositionWorld = position;
        CalculateNewLocalMatrix(parentWorldMatrix);

        if (GetTransformChangeEventFlag())
        InvokeChangeEventCallback();
    }

    void Transform::SetRotationWorld(const DirectX::XMFLOAT3& rotation)
    {
        DirectX::XMVECTOR rotationRadian = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&rotation), DirectX::XM_PI / 180.0F);
        DirectX::XMFLOAT4 quaternion;



        DirectX::XMStoreFloat4(&quaternion, DirectX::XMQuaternionRotationRollPitchYawFromVector(rotationRadian));
        SetQuaternionWorld(quaternion);

        if (GetTransformChangeEventFlag())
        InvokeChangeEventCallback();
    }

    void Transform::SetQuaternionWorld(const DirectX::XMFLOAT4& quaternion)
    {
        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();
        DirectX::XMStoreFloat4(&mQuaternionWorld, DirectX::XMQuaternionNormalize(DirectX::XMLoadFloat4(&quaternion)));
        CalculateNewLocalMatrix(parentWorldMatrix);

        if (GetTransformChangeEventFlag())
           InvokeChangeEventCallback();

        UpdateDirectionVector();
    
      
    }

    void Transform::SetTransformWorld(const DirectX::XMFLOAT4X4& worldMatrixF)
    {
        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();

        DirectX::XMVECTOR scaleWorld;
        DirectX::XMVECTOR quaternionWorld;
        DirectX::XMVECTOR translationWorld;

        DirectX::XMMatrixDecompose(&scaleWorld, &quaternionWorld, &translationWorld, DirectX::XMLoadFloat4x4(&worldMatrixF));

        DirectX::XMStoreFloat3(&mScaleWorld, scaleWorld);
        DirectX::XMStoreFloat4(&mQuaternionWorld, quaternionWorld);
        DirectX::XMStoreFloat3(&mPositionWorld, translationWorld);
     
        CalculateNewLocalMatrix(parentWorldMatrix);

        if (GetTransformChangeEventFlag())
            InvokeChangeEventCallback();
        UpdateDirectionVector();
    

    }

    void XM_CALLCONV Transform::SetTransformWorld(DirectX::FXMMATRIX worldMatrix)
    {
        DirectX::XMMATRIX parentWorldMatrix = CalculateParentMatrix();

        DirectX::XMVECTOR scaleWorld;
        DirectX::XMVECTOR quaternionWorld;
        DirectX::XMVECTOR translationWorld;

        DirectX::XMMatrixDecompose(&scaleWorld, &quaternionWorld, &translationWorld, worldMatrix);

        DirectX::XMStoreFloat3(&mScaleWorld, scaleWorld);
        DirectX::XMStoreFloat4(&mQuaternionWorld, quaternionWorld);
        DirectX::XMStoreFloat3(&mPositionWorld, translationWorld);

        CalculateNewLocalMatrix(parentWorldMatrix);
        UpdateDirectionVector();

        if (GetTransformChangeEventFlag())
        InvokeChangeEventCallback();
    }

    DirectX::XMFLOAT3 Transform::GetScaleLocal() const
    {
        return mScaleLocal;
    }

    DirectX::XMFLOAT3 Transform::GetScaleWorld() const
    {
        return mScaleWorld;
   }

    DirectX::XMFLOAT3 Transform::GetRoataiontLocal() const
    {
        //쿼터니언을 오일러각으로 변환한다.
        return  MathHelper::ConvertQuaternionToEulerAngle(mQuaternionLocal);
    }

    DirectX::XMFLOAT3 Transform::GetRotationWorld() const
    {
        return  MathHelper::ConvertQuaternionToEulerAngle(mQuaternionWorld);
    }

    const DirectX::XMFLOAT4 & Transform::GetQuaternionLocal() const
    {
        return mQuaternionLocal;
    }

    const DirectX::XMFLOAT4& Transform::GetQuaternionWorld() const
    {
        // TODO: 여기에 return 문을 삽입합니다.
        return mQuaternionWorld;
    }

    DirectX::XMFLOAT3 Transform::GetLookLocal() const
    {
        return mLookLocal;
    }

    DirectX::XMFLOAT3 Transform::GetLookWorld() const
    {

        //이런방향 벡터들은 월드변환후에 정규화를 해주어야한다.
        return mLookWorld;
    }

    DirectX::XMFLOAT3 Transform::GetRightLocal() const
    {
        return mRightLocal;
    }

    DirectX::XMFLOAT3 Transform::GetRightWorld() const
    {
        return mRightWorld;
    }

    DirectX::XMFLOAT3 Transform::GetUpLocal() const
    {
        return mUpLocal;
    }

    DirectX::XMFLOAT3 Transform::GetUpWorld() const
    {
        return mUpWorld;
    }

    DirectX::XMFLOAT4X4 Transform::GetLocalMatrix() const
    {
        return mLocalMatrixF;
    }

    DirectX::XMFLOAT4X4 Transform::GetWorldMatrix() const
    {
        return mWorldMatrixF;
    }

  
    void Transform::UpdateWorldMatrix(const DirectX::XMFLOAT4X4 & parentWorldMatrixF)
    {


        if (GetIndependentTransformFlag())
        {
            //이부분 다시봐야한다.제대로
            mWorldMatrixF = mLocalMatrixF;
            return;
        }
        
        DirectX::XMMATRIX parentWorldMatrix = DirectX::XMLoadFloat4x4(&parentWorldMatrixF);
        if (GetIndependentScaleFlag() || GetIndependentRotationFlag() || GetIndependentTransitionFlag())
        {
            DirectX::XMVECTOR parentScaleWorld;
            DirectX::XMVECTOR parentQuaternionWorld;
            DirectX::XMVECTOR parentTranslationWorld;

            DirectX::XMMatrixDecompose(&parentScaleWorld, &parentQuaternionWorld, &parentTranslationWorld, parentWorldMatrix);

            ///parentWorldMatrix = DirectX::XMMatrixIdentity();
            if (!GetIndependentScaleFlag())
            {
                //parentWorldMatrix=DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixScalingFromVector(parentScaleWorld));
                parentWorldMatrix = DirectX::XMMatrixScalingFromVector(parentScaleWorld);
            }
            else
            {
                parentWorldMatrix = DirectX::XMMatrixIdentity();
            }

            if (!GetIndependentRotationFlag())
            {
                parentWorldMatrix = DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixRotationQuaternion(parentQuaternionWorld));
            }

            if (!GetIndependentTransitionFlag())
            {
                parentWorldMatrix = DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixTranslationFromVector(parentTranslationWorld));
            }

        }
       
        //새로운 월드변환행렬을 계산한다.
        DirectX::XMMATRIX childWorldMatrix = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&mLocalMatrixF), parentWorldMatrix);
        DirectX::XMStoreFloat4x4(&mWorldMatrixF, childWorldMatrix);

        //성분을 추출하여 월드속성값을 설정한다.
        DirectX::XMVECTOR childScaleWorldVector;
        DirectX::XMVECTOR childQuaternionWorldVector;
        DirectX::XMVECTOR childTranslationWorldVector;

        DirectX::XMMatrixDecompose(&childScaleWorldVector, &childQuaternionWorldVector, &childTranslationWorldVector, childWorldMatrix);
        DirectX::XMStoreFloat3(&mScaleWorld, childScaleWorldVector);
        DirectX::XMStoreFloat4(&mQuaternionWorld, childQuaternionWorldVector);
        DirectX::XMStoreFloat3(&mPositionWorld, childTranslationWorldVector);
      
       //방향
       //update방향
       UpdateDirectionVector();
    


      // InvokeChangeEventCallback();
       SetDirtyFlag(true);


    }



    void Transform::SetIndependentTransformFlag(bool flag)
    {
        mIndependentTransformFlag = flag;
    }

    bool Transform::GetIndependentTransformFlag() const
    {
        return mIndependentTransformFlag;
    }

    void Transform::SetIndependentTransitionFlag(bool flag)
    {
        mIndependentTransitionFlag = flag;

    }

    bool Transform::GetIndependentTransitionFlag() const
    {
        return mIndependentTransitionFlag;
    }

    void Transform::SetIndependentRotationFlag(bool flag)
    {
        mIndependentRotationFlag = flag;
    }

    bool Transform::GetIndependentRotationFlag() const
    {
        return mIndependentRotationFlag;
    }

    void Transform::SetIndependentScaleFlag(bool flag)
    {
        //이설정을 on, 할때마다 부모의 world scale과 비교하여 local sacle을 계산해내야한다.
        //역으로 off할때도 마찬가지 width world /default width = local scale 
        mIndependentScaleFlag = flag;

    }

    bool Transform::GetIndependentScaleFlag() const
    {
        return mIndependentScaleFlag;
    }

    void Transform::ResetChangeValue()
    {
      /*  mScaleChangeRate = { 1.0F,1.0F,1.0F };
        mRotationChangeRate = { 0.0f,0.0f,0.0f };
        mTransitionChangeRate = { 0.0f,0.0f,0.0f };*/
    }

  

    void Transform::SetDefaultWidth(float width)
    {
        mDefaultWidth = width;
        mWidthWorld = mDefaultWidth * mScaleWorld.x;

        //widthWorld를 계산

    }

    void Transform::SetDefaultHeight(float height)
    {
        mDefaultHeight = height;
        mHeightWorld = mDefaultHeight * mScaleWorld.y;
    }

    void Transform::SetDefaultDepth(float depth)
    {
        mDefaultDepth = depth;
        mDepthWorld = mDefaultDepth * mScaleWorld.z;
    }

    void Transform::SetWidthWorld(float width)
    {
        float scaleFactor = width / mWidthWorld;
        DirectX::XMFLOAT3 scaleLocalF = GetScaleLocal();
        scaleLocalF.x *= scaleFactor;
        SetScaleLocal(scaleLocalF);
       
        
       // mWidthWorld = width;

    }

    void Transform::SetHeightWorld(float height)
    {
        float scaleFactor = height / mHeightWorld;
        DirectX::XMFLOAT3 scaleLocalF = GetScaleLocal();
        if (scaleFactor != 1.0f) {
            int a = 2;
        }
        scaleLocalF.y *= scaleFactor;
        SetScaleLocal(scaleLocalF);
       // mHeightWorld = height;


    }

    void Transform::SetDepthWorld(float depth)
    {
        float scaleFactor = depth / mDepthWorld;
        DirectX::XMFLOAT3 scaleLocalF = GetScaleLocal();
        scaleLocalF.z *= scaleFactor;
        SetScaleLocal(scaleLocalF);
        //mDepthWorld = depth;

    }

    float Transform::GetWidthWorld() const
    {
        return mWidthWorld;
    }

    float Transform::GetHeightWorld() const
    {
        return mHeightWorld;
    }

    float Transform::GetDepthWorld() const
    {
        return mDepthWorld;
    }

  /*  void Transform::RegisterChangeEventDst(Object* object, void(*fun)(Object *))
    {
       
        for (int i = 0; i < mTransformChangeCallbackTable.size(); ++i)
        {
            if (mTransformChangeCallbackTable[i].first == object)
                return;
        }

        mTransformChangeCallbackTable.push_back({ object,fun });
        return;
    }*/

    QHANDLE Transform::RegisterTransformChangeCallback(const std::function<void()>& callback)
    {
        return  mTransformChangeCallbackTable.Register(callback);
    }

    void Transform::InvokeChangeEventCallback()
    {
        const std::vector<QHANDLE> usingHandleVector = mTransformChangeCallbackTable.GetUsingHandleVector();
        for (auto callbackHandle : usingHandleVector)
        {
            std::function<void()>* pCallback = mTransformChangeCallbackTable.GetValue(callbackHandle);
            (*pCallback)();
        }
    }

    bool Transform::GetDirtyFlag() const
    {
        return mDirtyFlag;
    }

    DirectX::XMMATRIX XM_CALLCONV Transform::CalculateLocalMatrix()
    {
      /*  DirectX::XMMATRIX scaleLocalMatrix = DirectX::XMMatrixScaling(mScaleLocal.x, mScaleLocal.y, mScaleLocal.z);
        DirectX::XMMATRIX quaternionLocalMatrix= DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&mQuaternionLocal));
        DirectX::XMMATRIX translationLocalMatrix = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&mPositionLocal));*/


        //DirectX::XMMATRIX localMatrix =DirectX::XMMatrixTransformation(DirectX::XMVectorSet(0, 0, 0, 0), DirectX::XMVectorSet(0, 0, 0, 1),
        //    DirectX::XMLoadFloat3(&mScaleLocal), DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMLoadFloat4(&mQuaternionLocal), DirectX::XMLoadFloat3(&mPositionLocal));

        DirectX::XMMATRIX localMatrix=  DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&mScaleLocal)),
        DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&mQuaternionLocal))),
        DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&mPositionLocal)));



        DirectX::XMStoreFloat4x4(&mLocalMatrixF, localMatrix);

        return localMatrix;

    }

    DirectX::XMMATRIX XM_CALLCONV Transform::CalculateWorldMatrix()
    {
     /*   DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTransformation(DirectX::XMVectorSet(0, 0, 0, 0), DirectX::XMVectorSet(0, 0, 0, 1),
            DirectX::XMLoadFloat3(&mScaleWorld), DirectX::XMVectorSet(0, 0, 0, 1), DirectX::XMLoadFloat4(&mQuaternionWorld), DirectX::XMLoadFloat3(&mPositionWorld));*/

        DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&mScaleWorld)),
            DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&mQuaternionWorld))),
            DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&mPositionWorld)));
        DirectX::XMStoreFloat4x4(&mWorldMatrixF, worldMatrix);

        return worldMatrix;
    }

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
        updateDirectionVectorlamda(mQuaternionWorld, mLookWorld, mUpWorld, mRightWorld);




    }

    DirectX::XMMATRIX XM_CALLCONV Transform::CalculateParentMatrix()
    {
       return  DirectX::XMMatrixMultiply(DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&mLocalMatrixF)), DirectX::XMLoadFloat4x4(&mWorldMatrixF));

    }

    void Transform::CalculateNewLocalMatrix(DirectX::FXMMATRIX parentWorldMatrix)
    {


        //월드변환행렬을 계산한다.
         //월드속성값을 이용해서
        DirectX::XMMATRIX childWorldMatrix = CalculateWorldMatrix();


        //로컬변환행렬을 계산한다.
        DirectX::XMMATRIX childLocalMatrix = DirectX::XMMatrixMultiply(childWorldMatrix, DirectX::XMMatrixInverse(nullptr, parentWorldMatrix));
        DirectX::XMStoreFloat4x4(&mLocalMatrixF, childLocalMatrix);

        DirectX::XMVECTOR childScaleLocal;
        DirectX::XMVECTOR childQuaternionLocal;
        DirectX::XMVECTOR childTranslationLocal;

        //속성값 설정
        DirectX::XMMatrixDecompose(&childScaleLocal, &childQuaternionLocal, &childTranslationLocal, childLocalMatrix);
        DirectX::XMStoreFloat3(&mScaleLocal, childScaleLocal);
        DirectX::XMStoreFloat4(&mQuaternionLocal, childQuaternionLocal);
        DirectX::XMStoreFloat3(&mPositionLocal, childTranslationLocal);
        SetDirtyFlag(true);
     //   CallTransformChangeCallback();

    }

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
        JsonParser::Write("Transform_PositionWorld"+ tag, mPositionWorld);

        JsonParser::Write("Transform_QuaternionLocal"+ tag, mQuaternionLocal);
        JsonParser::Write("Transform_QuaternionWorld"+ tag, mQuaternionWorld);

        JsonParser::Write("Transform_ScaleLocal"+ tag, mScaleLocal);
        JsonParser::Write("Transform_ScaleWorld"+ tag, mScaleWorld);


        JsonParser::Write("Transform_LookLocal"+ tag, mLookLocal);
        JsonParser::Write("Transform_LookWorld"+ tag, mLookWorld);

        JsonParser::Write("Transform_RightLocal"+ tag, mRightLocal);
        JsonParser::Write("Transform_RightWorld"+ tag, mRightWorld);

        JsonParser::Write("Transform_UpLocal"+tag, mUpLocal);
        JsonParser::Write("Transform_UpWorld"+ tag, mUpWorld);

        



    }

    void Transform::DeSerialize(const std::string& tag)
    {
        JsonParser::Read("Transform_PositionLocal"+ tag, mPositionLocal);
        JsonParser::Read("Transform_PositionWorld"+ tag, mPositionWorld);

        JsonParser::Read("Transform_QuaternionLocal"+ tag, mQuaternionLocal);
        JsonParser::Read("Transform_QuaternionWorld"+ tag, mQuaternionWorld);

        JsonParser::Read("Transform_ScaleLocal"+ tag, mScaleLocal);
        JsonParser::Read("Transform_ScaleWorld"+ tag, mScaleWorld);


        JsonParser::Read("Transform_LookLocal"+ tag, mLookLocal);
        JsonParser::Read("Transform_LookWorld"+ tag, mLookWorld);

        JsonParser::Read("Transform_RightLocal"+ tag, mRightLocal);
        JsonParser::Read("Transform_RightWorld"+ tag, mRightWorld);

        JsonParser::Read("Transform_UpLocal"+ tag, mUpLocal);
        JsonParser::Read("Transform_UpWorld"+ tag, mUpWorld);



        //local , world matrix 구축

        CalculateLocalMatrix();
        CalculateWorldMatrix();







    }

    void Transform::SetDirtyFlag(bool flag)
    {

        //        mDestObject


        if (mDestSceneElement->GetSceneElementType() == ESceneElementType::eObject)
        {
            if (mDirtyFlag == false && flag == true)
            {

                mDestSceneElement->GetSystem()->AddTransformDirtyObject(mDestSceneElement);
            }
        }

        mDirtyFlag = flag;





    }

    void Transform::SetTransformChangeEventFlag(bool onOff)
    {
        mTransformChangeEventFlag = onOff;
    }

    bool Transform::GetTransformChangeEventFlag() const
    {
        return mTransformChangeEventFlag;
    }

    

    void Transform::RemoveTransformChangeCallback(QHANDLE handle)
    {
        mTransformChangeCallbackTable.Release(handle);
    }

    void Transform::CalculateNewWorldMatrix(DirectX::FXMMATRIX parentWorldMatrx)
    {

        DirectX::XMMATRIX childLocalMatrix = CalculateLocalMatrix();

        //새로운 월드변환행렬을 계산한다.
        DirectX::XMMATRIX childWorldMatrix = DirectX::XMMatrixMultiply(childLocalMatrix, parentWorldMatrx);
        DirectX::XMStoreFloat4x4(&mWorldMatrixF, childWorldMatrix);

        //성분을 추출하여 월드속성값을 설정한다.
        DirectX::XMVECTOR childScaleWorldVector;
        DirectX::XMVECTOR childQuaternionWorldVector;
        DirectX::XMVECTOR childTranslationWorldVector;

        DirectX::XMMatrixDecompose(&childScaleWorldVector, &childQuaternionWorldVector, &childTranslationWorldVector, childWorldMatrix);
        DirectX::XMStoreFloat3(&mScaleWorld, childScaleWorldVector);
        DirectX::XMStoreFloat4(&mQuaternionWorld, childQuaternionWorldVector);
        DirectX::XMStoreFloat3(&mPositionWorld, childTranslationWorldVector);
        SetDirtyFlag(true);
       // CallTransformChangeCallback();
    }






}