#include "PhysicsScene.h"
#include <Physics/Collider.h>
#include <Physics/Contact.h>
#include <algorithm>

PhysicsScene::PhysicsScene() : mNextPhysicsBodyHandle(1), mNextPhysicsShapeHandle(1) {}

PhysicsScene::~PhysicsScene()
{

    ClearAll();
}

void PhysicsScene::Update(float deltaTime)
{

    // 연결된 외부로부터 물리물체들의 속성 업데이트(외부에서 -> 물리 동기화)
    // 1. 생성, 삭제된 physicsBody 반영
    // 2. static , kinematic body의 transform 갱신
    //
    //
    //
    //

    ClearPhysicsFrameResult();

    SyncBodiesFromExternal();

    // 시뮬레이션 업데이트 ( 고정시간 간격으로 step 수행
    Step(deltaTime);

    // 외부로 동기화(물리-> 외부 동기화)
    // 1 .Dynamic body는 물리 시뮬레이션이 Transform을 외부로 동기화 (요청등등)
    SyncBodiesToExternal();

    MakeCollisionResponseData();
}

void PhysicsScene::SetID(PhysicsSceneID id)
{

    mID = id;
}
void PhysicsScene::SetName(const std::string &name)
{
    mName = name;
}
PhysicsSceneID PhysicsScene::GetID() const
{

    return mID;
}
std::string PhysicsScene::GetName() const
{

    return mName;
}

PhysicsBodyHandle PhysicsScene::CreatePhysicsBody(const PhysicsBodyDesc &bodyDesc)
{

    PhysicsBody *physicsBody = AllocPhysicsBody();

    if (physicsBody == nullptr)
        return PhysicsBodyHandleInValid;

    physicsBody->mHandle = mNextPhysicsBodyHandle;
    IncreaseNextPhysicsBodyHandle();

    BuildPhysicsBody(bodyDesc, *physicsBody);
    RegisterPhysicsBody(physicsBody);

    return physicsBody->mHandle;
}

PhysicsShapeHandle PhysicsScene::CreatePhysicsShape(const PhysicsShapeDesc &shapeDesc)
{
    if (shapeDesc.mBodyHandle == PhysicsBodyHandleInValid)
    {
        return PhysicsShapeHandleInValid;
    }

    PhysicsBody *body = GetPhysicsBody(shapeDesc.mBodyHandle);
    if (body == nullptr)
    {
        return PhysicsShapeHandleInValid;
    }

    PhysicsShape *physicsShape = AllocPhysicsShape(shapeDesc.mShapeType);
    if (physicsShape == nullptr)
    {
        return PhysicsShapeHandleInValid;
    }

    physicsShape->mHandle = mNextPhysicsShapeHandle;
    IncreaseNextPhysicsShapeHandle();

    BuildPhysicsShape(body, shapeDesc, *physicsShape);
    RegisterPhysicsShape(physicsShape);

    body->mShapeHandleList.push_back(physicsShape->mHandle);

    SyncBodyFromShape(body);

    return physicsShape->mHandle;
}

void PhysicsScene::DestroyPhysicsBody(PhysicsBodyHandle handle)
{

    PhysicsBody *physicsBody = GetPhysicsBody(handle);
    if (physicsBody == nullptr)
        return;

    for (auto shapeHandle : physicsBody->mShapeHandleList)
    {
        DestroyPhysicsShape(shapeHandle);
    }
    physicsBody->mShapeHandleList.clear();

    UnRegisterPhysicsBody(physicsBody);
    ReleasePhysicsBodyMemory(physicsBody);
}

void PhysicsScene::DestroyPhysicsShape(PhysicsShapeHandle handle)
{

    PhysicsShape *shape = GetPhysicsShape(handle);

    if (shape == nullptr)
        return;

    UnRegisterPhysicsShape(shape);
    ReleasePhysicsShapeMemory(shape);
}

bool PhysicsScene::SetBodyTransform(PhysicsBodyHandle handle, const CoreMath::Vector3 &pos,
                                    const CoreMath::Quaternion &rot)
{
    PhysicsBody *body = GetPhysicsBody(handle);
    if (body == nullptr)
        return false;

    body->mPosition = pos;
    body->mRotation = rot;

    return true;
}
bool PhysicsScene::GetBodyTransform(PhysicsBodyHandle handle, CoreMath::Vector3 &oPos, CoreMath::Quaternion &oRot) const
{
    PhysicsBody *body = GetPhysicsBody(handle);
    if (body == nullptr)
        return false;

    oPos = body->mPosition;
    oRot = body->mRotation;

    return true;
}
void PhysicsScene::EnqueueTransformCommand(const PhysicsTransformCommand &command)
{
    mPhysicsCommandContext.mTransformCommandList.push_back(command);
}
void PhysicsScene::EnqueueSizeCommand(const PhysicsSizeCommand &command)
{
    mPhysicsCommandContext.mSizeCommnadList.push_back(command);
}
void PhysicsScene::EnqueueAddForceCommand(const PhysicsAddForceCommand &command)
{
    mPhysicsCommandContext.mAddForceCommandList.push_back(command);
}

void PhysicsScene::EnqueueSetKinematicVelocityCommand(const PhysicsSetVelocityCommmand &command)
{
    mPhysicsCommandContext.mSetVelocityCommandList.push_back(command);
}

const PhysicsFrameResult &PhysicsScene::GetPhysicsFrameResult() const
{
    return mPhysicsFrameResult;
}

void PhysicsScene::ClearPhysicsFrameResult()
{
    mPhysicsFrameResult.mTransformResults.clear();
}

CoreMath::Vector3 PhysicsScene::GetVelocity(PhysicsBodyHandle handle) const
{

    PhysicsBody *body = GetPhysicsBody(handle);
    if (body == nullptr)
        return {0, 0, 0};

    return body->mVelocity;
}

float PhysicsScene::GetMass(PhysicsBodyHandle handle) const
{

    PhysicsBody *body = GetPhysicsBody(handle);
    if (body == nullptr)
        return 0.0f;

    return body->mMass;
}

PhysicsContact PhysicsScene::BuildPhysicsContact(PhysicsBody &bodyA, PhysicsBody &bodyB, PhysicsShape &shapeA,
                                                 PhysicsShape &shapeB, const CollisionContact &source)
{

    PhysicsContact contact;

    contact.mBodyA = &bodyA;
    contact.mBodyB = &bodyB;

    contact.mShapeA = &shapeA;
    contact.mShapeB = &shapeB;

    contact.mShapeAHandle = shapeA.mHandle;
    contact.mShapeBHandle = shapeB.mHandle;

    contact.mPoint = source.mPoint;
    contact.mNormal = source.mNormal;
    contact.mPenetration = source.mPenetration;
    contact.mContactFeature = source.mFeature;

    return contact;
}

KinematicContact PhysicsScene::BuildKinematicContact(PhysicsBody &bodyA, PhysicsBody &bodyB, PhysicsShape &shapeA,
                                                     PhysicsShape &shapeB, const CollisionContact &source)
{

    KinematicContact contact;

    if (bodyA.mBodyType == EPhysicsBodyType::eKinematic)
    {

        contact.mKinematicBody = &bodyA;
        contact.mOtherBody = &bodyB;
        contact.mKinematicBodyHandle = bodyA.mHandle;
        contact.mKinematicShapeHandle = shapeA.mHandle;

        contact.mOtherBodyHandle = bodyB.mHandle;
        contact.mOtherShapeHandle = shapeB.mHandle;

        // B - >A
        contact.mNormalTowardKinematic = source.mNormal;
    }
    else
    {
        contact.mKinematicBody = &bodyB;
        contact.mOtherBody = &bodyA;
        contact.mKinematicBodyHandle = bodyB.mHandle;
        contact.mKinematicShapeHandle = shapeB.mHandle;

        contact.mOtherBodyHandle = bodyA.mHandle;
        contact.mOtherShapeHandle = shapeA.mHandle;

        // B - >A
        contact.mNormalTowardKinematic = -1.0f * source.mNormal;
    }

    contact.mPoint = source.mPoint;
    contact.mPenetration = source.mPenetration;

    return contact;
}

void PhysicsScene::SetCollisionChannelResponseTable(const PhysicsCollisionChannelResponseTable *pTable)
{

    mCollisionChannelResponseTable = pTable;
}

void PhysicsScene::ConsumeCollisionEvents(std::vector<PhysicsCollisionResponseData> &outEvents)
{

    outEvents = std::move(mCollisionResponseEventList);
}

PhysicsBody *PhysicsScene::GetPhysicsBody(PhysicsBodyHandle handle) const
{

    auto it = mPhysicsBodyHandleTable.find(handle);

    if (it == mPhysicsBodyHandleTable.end())
        return nullptr;

    return it->second;
}

PhysicsShape *PhysicsScene::GetPhysicsShape(PhysicsShapeHandle handle) const
{

    auto it = mPhysicsShapeHandleTable.find(handle);

    if (it == mPhysicsShapeHandleTable.end())
    {
        return nullptr;
    }

    return it->second;
}

void PhysicsScene::IncreaseNextPhysicsBodyHandle()
{

    mNextPhysicsBodyHandle++;
}

void PhysicsScene::IncreaseNextPhysicsShapeHandle()
{

    mNextPhysicsShapeHandle++;
}

PhysicsBody *PhysicsScene::AllocPhysicsBody()
{

    PhysicsBody *body = new PhysicsBody;
    return body;
}

void PhysicsScene::BuildPhysicsBody(const PhysicsBodyDesc &bodyDesc, PhysicsBody &oBody)
{

    oBody.mMass = bodyDesc.mMass;
    oBody.mPosition = bodyDesc.mPosition;
    oBody.mRotation = bodyDesc.mRotation;
    oBody.mVelocity = bodyDesc.mVelocity;
    oBody.mBodyType = bodyDesc.mBodyType;
    oBody.mForce = bodyDesc.mForce;
    oBody.mGravity = bodyDesc.mGravity;
    oBody.mCollisionChannelResponseID = bodyDesc.mCollisionChannelResponseID;

    CalcInertia(oBody);
    if (oBody.mBodyType == EPhysicsBodyType::eStatic)
    {
        oBody.SetAwake(false);
    }
    else if (oBody.mBodyType == EPhysicsBodyType::eDynamic)
    {
        oBody.SetAwake(true);
    }
    else if (oBody.mBodyType == EPhysicsBodyType::eKinematic)
    {
        oBody.SetAwake(true);
    }
}

void PhysicsScene::RegisterPhysicsBody(PhysicsBody *body)
{

    if (body == nullptr)
        return;

    mPhysicsBodyHandleTable[body->mHandle] = body;
    mPhysicsBodyList.push_back(body);
}

void PhysicsScene::UnRegisterPhysicsBody(PhysicsBody *body)
{

    if (body == nullptr)
        return;

    mPhysicsBodyHandleTable.erase(body->mHandle);

    auto it = std::find(mPhysicsBodyList.begin(), mPhysicsBodyList.end(), body);

    if (it != mPhysicsBodyList.end())
    {
        std::iter_swap(it, mPhysicsBodyList.end() - 1);
        mPhysicsBodyList.pop_back();
    }
}

void PhysicsScene::ReleasePhysicsBodyMemory(PhysicsBody *body)
{
    delete body;
}

PhysicsShape *PhysicsScene::AllocPhysicsShape(EPhysicsCollisionShapeType shapeType)
{

    PhysicsShape *shape = nullptr;

    switch (shapeType)
    {

    case EPhysicsCollisionShapeType::eNone:

        break;
    case EPhysicsCollisionShapeType::eBox:
        shape = new PhysicsBoxShape;
        break;
    }

    return shape;
}

void PhysicsScene::BuildPhysicsShape(PhysicsBody *body, const PhysicsShapeDesc &shapeDesc, PhysicsShape &oShape)
{

    oShape.mAttachedBodyHandle = shapeDesc.mBodyHandle;
    oShape.mPhysicsShapeType = shapeDesc.mShapeType;

    oShape.mLocalPosition = shapeDesc.mLocalPosition;
    oShape.mLocalRotation = shapeDesc.mLocalRotation;
    oShape.mLocalScale = shapeDesc.mLocalScale;

    oShape.mWorldPosition = shapeDesc.mWorldPosition;
    oShape.mWorldRotation = shapeDesc.mWorldRotation;
    oShape.mWorldScale = shapeDesc.mWorldScale;

    // 상대좌표 계산
    GetShapeLocalTransformFromBody(*body, oShape);

    oShape.mWorldTransform =
        CoreMath::Matrix4X4::MakeTransform(oShape.mWorldPosition, oShape.mWorldRotation, {1, 1, 1});

    // oShape.mLocalOffset = shapeDesc.mLocalOffset;
    // oShape.mLocalRotation = shapeDesc.mLocalRotation;

    switch (oShape.mPhysicsShapeType)
    {
    case EPhysicsCollisionShapeType::eNone:

        break;
    case EPhysicsCollisionShapeType::eBox:
    {
        PhysicsBoxShape &boxShape = static_cast<PhysicsBoxShape &>(oShape);
        boxShape.mLocalHalfExtent = shapeDesc.mBoxData.mLocalHalfExtent;
        boxShape.UpdateShapeSpecificData();
    }
    break;
    }

    return;
}

void PhysicsScene::RegisterPhysicsShape(PhysicsShape *shape)
{

    if (shape == nullptr)
        return;

    mPhysicsShapeHandleTable[shape->mHandle] = shape;
    mPhysicsShapeList.push_back(shape);
}

void PhysicsScene::UnRegisterPhysicsShape(PhysicsShape *shape)
{

    if (shape == nullptr)
        return;

    mPhysicsShapeHandleTable.erase(shape->mHandle);

    auto it = std::find(mPhysicsShapeList.begin(), mPhysicsShapeList.end(), shape);
    if (it != mPhysicsShapeList.end())
    {

        std::iter_swap(mPhysicsShapeList.end() - 1, it);
        mPhysicsShapeList.pop_back();
    }
}

void PhysicsScene::ReleasePhysicsShapeMemory(PhysicsShape *shape)
{

    delete shape;
}

void PhysicsScene::SyncBodyFromShape(PhysicsBody *pBody)
{

    if (pBody == nullptr)
        return;

    if (pBody->mShapeHandleList.empty())
        return;

    PhysicsShape *pShape = GetPhysicsShape(pBody->mShapeHandleList[0]);

    pBody->mMass = pShape->mMass;
    RecalculateBodyInertia(*pBody);

    //    pBody->mInertiaInverse = pShape->mInverseInertiaTensor;
}

void PhysicsScene::ClearAll()
{

    std::vector<PhysicsBody *> tempBodyList = mPhysicsBodyList;

    for (auto body : tempBodyList)
    {
        if (body)
            DestroyPhysicsBody(body->mHandle);
    }
}

void PhysicsScene::Step(float deltaTime)
{

    // 외부 알짜힘 , 토크 계산

    // 수치적분
    // 속도 적분

    // 위치 적분

    for (int i = 0; i < mPhysicsBodyList.size(); ++i)
    {

        if (mPhysicsBodyList[i])
        {

            // 물리 시물레이션의 전역적인 모든 힘,  토크  + 물리모델의 힘, 토크 모두 계산
            //  CalcForce(mPhysicsBodyList[i]);
            CalcForces(*mPhysicsBodyList[i]);
        }
    }

    // 수치적분
    for (int i = 0; i < mPhysicsBodyList.size(); ++i)
    {

        if (mPhysicsBodyList[i])
        {
            // 수치적분
            // 오일러적분
            //    UpdateBodyEuler(deltaTime, mPhysicsBodyList[i]);

            EPhysicsBodyType bodyType = mPhysicsBodyList[i]->mBodyType;

            if (bodyType == EPhysicsBodyType::eDynamic)
            {
                UpdateBodyEuler(deltaTime, *mPhysicsBodyList[i]);
            }
            else if (bodyType == EPhysicsBodyType::eKinematic)
            {

                UpdateKinematicBodyEuler(deltaTime, *mPhysicsBodyList[i]);
            }
        }
    }

    // 충돌처리

    // manifold - contact list에서 이전충돌들을 갱신
    RefreshManifold();

    // 충돌반응
    //    // 1 .충돌처리 데이터 준비
    //    // 2.관통처리  - 반복
    //    // 3. 속도처리  - 반복
    // kinematic-static
    CheckPreviousGroundResult();

    CheckBodyKinematicStaticCollision();
    KinematicCollisionResolution(deltaTime);

    // dynamic
    CheckBodyDynamicCollision();
    CollisionResolution(deltaTime);

    for (int i = 0; i < mPhysicsBodyList.size(); ++i)
    {

        if (mPhysicsBodyList[i])
        {

            mPhysicsBodyList[i]->UpdateMotion(deltaTime);
            mPhysicsBodyList[i]->UpdateAwakeState();
        }
    }

    mKinematicContactList.clear();
}

bool PhysicsScene::CheckBodyCollision(PhysicsBody &bodyA, PhysicsBody &bodyB)
{

    EPhysicsCollisionChannelResponseType responseType = mCollisionChannelResponseTable->GetResponseType(
        bodyA.mCollisionChannelResponseID, bodyB.mCollisionChannelResponseID);

    if (responseType == EPhysicsCollisionChannelResponseType::eIgnore)
        return false;

    if (responseType == EPhysicsCollisionChannelResponseType::eOverlap)
    {
        int a = 2;
    }

    // 충돌검출

    // 일단 box - box aabb 로 충돌검사

    // 여러 하위 shape들에서 충돌이 감지될수있는데 관통의 깊이가 가장 작은 경우가 선발되어야한다.

    // 두물체 모두 sleep상태임으로
    if (!bodyA.mIsAwake && !bodyB.mIsAwake && responseType != EPhysicsCollisionChannelResponseType::eOverlap)
    {
        return false;
    }

    bool hasDynamicBody =
        (bodyA.mBodyType == EPhysicsBodyType::eDynamic || bodyB.mBodyType == EPhysicsBodyType::eDynamic);

    ECollisionResponsePath path = ClassifyCollisionPair(bodyA, bodyB);

    for (size_t i = 0; i < bodyA.mShapeHandleList.size(); ++i)
    {

        PhysicsShape *pShapeA = GetPhysicsShape(bodyA.mShapeHandleList[i]);

        for (size_t j = 0; j < bodyB.mShapeHandleList.size();)
        {
            PhysicsShape *pShapeB = GetPhysicsShape(bodyB.mShapeHandleList[j]);

            // DetectCollision
            CollisionContactManifold manifold;

            // PhysicsContact contact;
            bool bodySwap = false;
            bool bRepeat = false;
            if (DectectCollision(*pShapeA, *pShapeB, manifold, bodySwap))
            {
                if (responseType == EPhysicsCollisionChannelResponseType::eOverlap)
                {
                    PushCurrPhysicsCollisionPairState(bodyA.mHandle, bodyB.mHandle,
                                                      EPhysicsCollisionChannelResponseType::eOverlap);
                    return false;
                }

                PhysicsBody *orderedBodyA = &bodyA;
                PhysicsBody *orderedBodyB = &bodyB;

                manifold.mShapeA = pShapeA;
                manifold.mShapeB = pShapeB;

                manifold.mShapeAHandle = pShapeA->mHandle;
                manifold.mShapeBHandle = pShapeB->mHandle;

                if (bodySwap)
                {
                    std::swap(manifold.mShapeA, manifold.mShapeB);
                    std::swap(manifold.mShapeAHandle, manifold.mShapeBHandle);

                    std::swap(manifold.mShapeA, manifold.mShapeB);
                    std::swap(manifold.mShapeAHandle, manifold.mShapeBHandle);

                    std::swap(orderedBodyA, orderedBodyB);
                    /*manifold.mShapeA = pShapeA;
                    manifold.mShapeB = pShapeB;

                    manifold.mShapeAHandle = pShapeA->mHandle;
                    manifold.mShapeBHandle = pShapeB->mHandle;*/
                }

                /*  for (CollisionContact &contact : manifold.mContacts)
                  {
                      contact.mShapeA = pShapeA;
                      contact.mShapeB = pShapeB;

                      contact.mShapeAHandle = pShapeA->mHandle;
                      contact.mShapeBHandle = pShapeB->mHandle;

                      contact.mBodyA = &bodyA;
                      contact.mBodyB = &bodyB;

                      if (bodySwap)
                      {
                          std::swap(contact.mBodyA, contact.mBodyB);
                          std::swap(contact.mShapeA, contact.mShapeB);
                          std::swap(contact.mShapeAHandle, contact.mShapeBHandle);

                          contact.mContactFeature.bUseEdgeB = !contact.mContactFeature.bUseEdgeB;
                      }
                  }*/

                // contact  분류

                // 상세한 contact manifold생성

                switch (path)
                {
                case ECollisionResponsePath::eNone:

                    break;

                case ECollisionResponsePath::eRigidBodySolver:
                    AddRigidBodyManifold(*orderedBodyA, *orderedBodyB, manifold);
                    break;

                case ECollisionResponsePath::eKinematicSolver:
                    AddKinematicContacts(*orderedBodyA, *orderedBodyB, manifold);
                    break;
                }

                // AddManifold(manifold);
            }

            ++j;
        }
    }

    return false;
}

// Kinematic-static 충돌검출
void PhysicsScene::CheckBodyKinematicStaticCollision()
{
    // 새로운 충돌감지
    for (int i = 0; i < mPhysicsBodyList.size(); ++i)
    {
        for (int j = i + 1; j < mPhysicsBodyList.size(); ++j)
        {

            PhysicsBody *bodyA = mPhysicsBodyList[i];
            PhysicsBody *bodyB = mPhysicsBodyList[j];

            ECollisionResponsePath path = ClassifyCollisionPair(*bodyA, *bodyB);

            if (path != ECollisionResponsePath::eKinematicSolver)
                continue;

            CheckBodyCollision(*bodyA, *bodyB);
        }
    }
}
// dynamic 충돌검출
void PhysicsScene::CheckBodyDynamicCollision()
{

    // 새로운 충돌감지
    for (int i = 0; i < mPhysicsBodyList.size(); ++i)
    {
        for (int j = i + 1; j < mPhysicsBodyList.size(); ++j)
        {

            PhysicsBody *bodyA = mPhysicsBodyList[i];
            PhysicsBody *bodyB = mPhysicsBodyList[j];

            ECollisionResponsePath path = ClassifyCollisionPair(*bodyA, *bodyB);

            if (path != ECollisionResponsePath::eRigidBodySolver)
                continue;

            CheckBodyCollision(*bodyA, *bodyB);
        }
    }
}

void PhysicsScene::RefreshManifold()
{

    mContactManifoldTable.clear();

    // for (auto it = mContactManifoldTable.begin(); it != mContactManifoldTable.end(); ++it)
    //{

    //    std::vector<PhysicsContact> &contactList = it->second.mContactList;
    //    std::vector<PhysicsContact> newContactList;
    //    for (int i = 0; i < contactList.size(); ++i)
    //    {

    //        // Shape 들의 유효성 검사

    //        PhysicsContact &contact = contactList[i];

    //        PhysicsShape *pShapeA = GetPhysicsShape(contact.mShapeAHandle);
    //        PhysicsShape *pShapeB = GetPhysicsShape(contact.mShapeBHandle);

    //        if (IsSleep(*contact.mBodyA, *contact.mBodyB))
    //        {
    //            continue;
    //        }

    //        if (pShapeA == nullptr || pShapeB == nullptr)
    //        {
    //            // 이 contact는 제거한다.
    //            continue;
    //        }

    //        PhysicsContact newContact;
    //        ContactManifold contactManifold;
    //        bool bodySwap = false;
    //        bool bRepeat = false;
    //        if (!DectectCollision(*pShapeA, *pShapeB, contactManifold, bodySwap))
    //        {
    //            continue;
    //        }

    //        // 충돌데이터 갱신

    //        // 1. 여전히 해당 접촉의 충돌이 유효한가.
    //        // 2. 유효하다면 충돌데이터를 새롭게 계산한다.

    //        // shape type별로 나누고

    //        if (pShapeA->mPhysicsShapeType == EPhysicsCollisionShapeType::eBox &&
    //            pShapeB->mPhysicsShapeType == EPhysicsCollisionShapeType::eBox)
    //        {

    //            PhysicsBoxShape *boxShapeA = static_cast<PhysicsBoxShape *>(pShapeA);
    //            PhysicsBoxShape *boxShapeB = static_cast<PhysicsBoxShape *>(pShapeB);

    //            CoreMath::Vector3 toCentre = boxShapeB->mWorldPosition - boxShapeA->mWorldPosition;

    //            if (contact.mContactFeature.mType == EContactFeatureType::eFaceA)
    //            {
    //                // 면축의 경우
    //                // 그 면의 법선 벡터가 분리축이되고 최대 사영의 길이의합이 중심점을 잇는 선분의 길이보다 크면
    //                여전히
    //                // 접촉 , 음수이면 더이상 접촉하지않는다.

    //                CoreMath::Vector3 axis = boxShapeA->GetAxis(contact.mContactFeature.mFaceIndex);

    //                float penetration = TryAxis(*boxShapeA, *boxShapeB, axis, toCentre);
    //                if (penetration < 0.0f)
    //                {

    //                    // 충돌하지않는다.
    //                    continue;
    //                }

    //                FillContactDataFaceBoxBox(*boxShapeA, *boxShapeB, contact.mContactFeature.mFaceIndex, toCentre,
    //                                          penetration, contact);
    //            }
    //            else if (contact.mContactFeature.mType == EContactFeatureType::eEdgeEdge)
    //            {
    //                CoreMath::Vector3 edgeA = boxShapeA->GetAxis(contact.mContactFeature.mEdgeAIndex);
    //                CoreMath::Vector3 edgeB = boxShapeB->GetAxis(contact.mContactFeature.mEdgeBIndex);

    //                float penetration = TryAxis(*boxShapeA, *boxShapeB, edgeA.Cross(edgeB), toCentre);

    //                if (penetration < 0.0f)
    //                {
    //                    continue;
    //                }
    //                FillContactDataEdgeBoxBox(*boxShapeA, *boxShapeB, contact.mContactFeature.mEdgeAIndex,
    //                                          contact.mContactFeature.mEdgeBIndex, contact.mContactFeature.bUseEdgeB,
    //                                          toCentre, penetration, contact);
    //            }
    //        }

    //        newContactList.push_back(contact);
    //    }

    //    std::swap(it->second.mContactList, newContactList);
    //}

    //// 비어있는 manifold들을 제거
    // for (auto it = mContactManifoldTable.begin(); it != mContactManifoldTable.end();)
    //{
    //     if (it->second.mContactList.empty())
    //     {
    //         it = mContactManifoldTable.erase(it);
    //     }
    //     else
    //     {
    //         it++;
    //     }
    // }
}

void PhysicsScene::KinematicCollisionResolution(float deltaTime)
{
    int loop = 20;
    for (int i = 0; i < mKinematicContactList.size() * loop; ++i)
    {

        float bestPenetration = 0.000001f;
        int bestContactIndex = -1;

        // 관통깊이가 가장깊은 접촉을 찾는다
        for (int i = 0; i < mKinematicContactList.size(); ++i)
        {

            if (mKinematicContactList[i].mPenetration > bestPenetration)
            {
                bestPenetration = mKinematicContactList[i].mPenetration;
                bestContactIndex = i;
            }
        }

        if (bestContactIndex == -1)
            break;

        CoreMath::Vector3 linearChange;
        CoreMath::Vector3 angularChange;
        mKinematicContactList[bestContactIndex].ResolvePenetration(linearChange, angularChange);

        PhysicsBody *bestcontactBodies[2] = {mKinematicContactList[bestContactIndex].mKinematicBody,
                                             mKinematicContactList[bestContactIndex].mOtherBody};

        for (int i = 0; i < mKinematicContactList.size(); ++i)
        {
            KinematicContact &contact = mKinematicContactList[i];

            if (contact.mKinematicBody == mKinematicContactList[bestContactIndex].mKinematicBody)
            {

                CoreMath::Vector3 r = contact.mPoint - contact.mKinematicBody->mPosition;
                CoreMath::Vector3 deltaPosition = linearChange + angularChange.Cross(r);

                contact.mPenetration -= deltaPosition.Dot(contact.mNormalTowardKinematic);
                // contact.mPoint += deltaPosition;
            }
        }

        mKinematicContactList[bestContactIndex].mKinematicBody->mTransformSyncDirty = true;

        UpdateGroundResult(mKinematicContactList[bestContactIndex]);
    }
}

void PhysicsScene::CollisionResolution(float deltaTime)
{
    std::vector<PhysicsContact> contactList;
    for (auto it = mContactManifoldTable.begin(); it != mContactManifoldTable.end(); ++it)
    {
        ContactManifold &manifold = it->second;

        for (int i = 0; i < manifold.mContactList.size(); ++i)
        {
            PhysicsContact &contact = manifold.mContactList[i];

            contact.CalculateInternals(deltaTime);

            contactList.push_back(contact);

            // 관통처리
            //   contact.ResolvePenetration();
        }
    }

    int loop = 20;

    bool t2 = true;
    if (t2)
    {
        for (int k = 0; k < contactList.size() * loop; ++k)
        {

            float bestPenetration = 0.000001f;
            int bestContactIndex = -1;

            // 관통깊이가 가장깊은 접촉을 찾는다
            for (int i = 0; i < contactList.size(); ++i)
            {

                if (contactList[i].mPenetration > bestPenetration)
                {
                    bestPenetration = contactList[i].mPenetration;
                    bestContactIndex = i;
                }
            }

            if (bestContactIndex == -1)
                break;

            CoreMath::Vector3 linearChange[2] = {};
            CoreMath::Vector3 angularChange[2] = {};
            contactList[bestContactIndex].ResolvePenetration(linearChange, angularChange);

            PhysicsBody *bestcontactBodies[2] = {contactList[bestContactIndex].mBodyA,
                                                 contactList[bestContactIndex].mBodyB};

            for (int i = 0; i < contactList.size(); ++i)
            {
                PhysicsContact &contact = contactList[i];

                PhysicsBody *bodies[2] = {contact.mBodyA, contact.mBodyB};

                for (int a = 0; a < 2; ++a)
                {
                    if (bodies[a])
                    {

                        for (int b = 0; b < 2; ++b)
                        {
                            if (bodies[a] == bestcontactBodies[b])
                            {

                                CoreMath::Vector3 r = contact.mPoint - bodies[a]->mPosition;
                                CoreMath::Vector3 deltaPosition = linearChange[b] + angularChange[b].Cross(r);

                                contact.mPenetration += deltaPosition.Dot(contact.mNormal) * (a ? 1 : -1);
                                // contact.mPoint += deltaPosition;
                            }
                        }
                    }
                }
            }

            //  UpdatePenetrations(contactList);
        }
    }
    else
    {
        CoreMath::Vector3 linearVelChange[2];
        CoreMath::Vector3 angularVelChange[2];
        for (int i = 0; i < contactList.size(); ++i)
        {
            PhysicsContact &contact = contactList[i];
            contact.ResolvePenetration(linearVelChange, angularVelChange);
        }
    }

    bool tt = true;
    int k = 0;
    if (tt)
    {

        for (k = 0; k < contactList.size() * loop; ++k)
        {

            float bestVel = 0.001f;
            int bestContactIndex = -1;

            for (int i = 0; i < contactList.size(); ++i)
            {

                if (contactList[i].mDesiredVelocity > bestVel)
                {
                    bestVel = contactList[i].mDesiredVelocity;
                    bestContactIndex = i;
                }
            }

            if (bestContactIndex == -1)
            {
                int a = 2;
                break;
            }

            CoreMath::Vector3 linearVelChange[2] = {};
            CoreMath::Vector3 angularVelChange[2] = {};

            contactList[bestContactIndex].matchAwakeState();

            contactList[bestContactIndex].ApplyImpulse(deltaTime, linearVelChange, angularVelChange);

            PhysicsBody *bestcontactBodies[2] = {contactList[bestContactIndex].mBodyA,
                                                 contactList[bestContactIndex].mBodyB};

            for (int i = 0; i < contactList.size(); ++i)
            {
                PhysicsContact &contact = contactList[i];

                PhysicsBody *bodies[2] = {contact.mBodyA, contact.mBodyB};

                for (int a = 0; a < 2; ++a)
                {
                    if (bodies[a])
                    {

                        for (int b = 0; b < 2; ++b)
                        {
                            if (bodies[a] == bestcontactBodies[b])
                            {

                                CoreMath::Vector3 deltaVel =
                                    linearVelChange[b] + angularVelChange[b].Cross(contact.mRelativeContactPosition[a]);

                                if (a == 0)
                                {
                                    contact.mRelativeVelocity +=
                                        contact.mContactToWorld.GetTransposed().TransformDirection(deltaVel);
                                }
                                else
                                {
                                    contact.mRelativeVelocity -=
                                        contact.mContactToWorld.GetTransposed().TransformDirection(deltaVel);
                                }
                                contact.CalculateVelocity(deltaTime);
                            }
                        }
                    }
                }
            }
        }

        if (k >= contactList.size() * loop)
        {
            int a = 2;
        }
    }
    else
    {
        CoreMath::Vector3 linearVelChange[2];
        CoreMath::Vector3 angularVelChange[2];
        for (int i = 0; i < contactList.size(); ++i)
        {
            PhysicsContact &contact = contactList[i];

            contact.ApplyImpulse(deltaTime, linearVelChange, angularVelChange);
        }
    }

    for (PhysicsBody *body : mPhysicsBodyList)
    {
        if (body && body->mBodyType == EPhysicsBodyType::eDynamic)
        {
            body->mAngularVelocity *= 0.95f;

            if (body->mAngularVelocity.LengthSquared() < 0.0001f)
                body->mAngularVelocity = {0, 0, 0};
        }
    }
}

void PhysicsScene::UpdatePenetrations(std::vector<PhysicsContact> &contactList) {}

void PhysicsScene::ApplyImpluse(PhysicsContact &contact)
{

    // 충격 계산

    // 각 강제의 속도, 각속도 설정

    // if (contact.mBodyA->mBodyType == EPhysicsBodyType::eDynamic)
    //{

    //    contact.mBodyA->mVelocity =
    //        -1.0f * contact.mBodyA->mVelocity.Dot(contact.mNormal) * contact.mNormal * contact.mRestitution;
    //}
    // else if (contact.mBodyB->mBodyType == EPhysicsBodyType::eDynamic)
    //{
    //    contact.mBodyB->mVelocity =
    //        -1.0f * contact.mBodyB->mVelocity.Dot(contact.mNormal) * contact.mNormal * contact.mRestitution;
    //}
}

bool PhysicsScene::DectectCollision(PhysicsShape &shapeA, PhysicsShape &shapeB, CollisionContactManifold &oManifoold,
                                    bool &oBodySwap) const
{

    // 어떻한 충돌검사를 사용할것인가 .
    // 하드코딩할수있는데
    // 나중에 다른방법을 적용하도록 수정할수도있으니
    // DectionCollisionPolicy

    EPhysicsCollisionShapeType shAType = shapeA.mPhysicsShapeType;
    EPhysicsCollisionShapeType shBType = shapeB.mPhysicsShapeType;

    if (shAType == EPhysicsCollisionShapeType::eBox && shBType == EPhysicsCollisionShapeType::eBox)
    {
        PhysicsBoxShape &boxA = static_cast<PhysicsBoxShape &>(shapeA);
        PhysicsBoxShape &boxB = static_cast<PhysicsBoxShape &>(shapeB);

        PhysicsBoxCollisionQueryData collisionDataA = boxA.GetCollisionData();
        PhysicsBoxCollisionQueryData collisionDataB = boxB.GetCollisionData();

        return DetectBoxAndBox(collisionDataA, collisionDataB, oManifoold, oBodySwap);
    }

    return false;
}

bool PhysicsScene::DectectCollisionWithOffset(PhysicsShape &shapeA, const CoreMath::Vector3 &offset,
                                              PhysicsShape &shapeB, CollisionContactManifold &oManifoold,
                                              bool &oBodySwap) const
{
    // 어떻한 충돌검사를 사용할것인가 .
    // 하드코딩할수있는데
    // 나중에 다른방법을 적용하도록 수정할수도있으니
    // DectionCollisionPolicy

    EPhysicsCollisionShapeType shAType = shapeA.mPhysicsShapeType;
    EPhysicsCollisionShapeType shBType = shapeB.mPhysicsShapeType;

    if (shAType == EPhysicsCollisionShapeType::eBox && shBType == EPhysicsCollisionShapeType::eBox)
    {
        PhysicsBoxShape &boxA = static_cast<PhysicsBoxShape &>(shapeA);
        PhysicsBoxShape &boxB = static_cast<PhysicsBoxShape &>(shapeB);

        PhysicsBoxCollisionQueryData collisionDataA = boxA.GetCollisionData();

        collisionDataA.Translate(offset);

        PhysicsBoxCollisionQueryData collisionDataB = boxB.GetCollisionData();

        return DetectBoxAndBox(collisionDataA, collisionDataB, oManifoold, oBodySwap);
    }

    return false;
}

#define CHECKOVERLAP(axis, index)                                                                                      \
    if (!CheckOverlap(collisionDataA, collisionDataB, (axis), toCentre, (index), penetration, bestIndex))              \
        return false;

bool PhysicsScene::DetectBoxAndBox(PhysicsBoxCollisionQueryData &collisionDataA,
                                   PhysicsBoxCollisionQueryData &collisionDataB, CollisionContactManifold &oManifoold,
                                   bool &oBodySwap) const
{
    oBodySwap = false;

    // 근데 일단은 SAT 방식을 바로 작성

    // 일단 BOX - BOX SAT  - >  15개의 분리축 테스트 수행

    //  PhysicsBoxShape &boxA = static_cast<PhysicsBoxShape &>(shapeA);
    //  PhysicsBoxShape &boxB = static_cast<PhysicsBoxShape &>(shapeB);

    // 15개의 분리축으로 SAT 수행, 가장 낮은 관통깊이를 가지는경우가 선택됨
    // 한번이라도 실패하면 두  박스는 충돌하지 않는다는것이므로 즉시 종료
    // A->B
    CoreMath::Vector3 toCentre = collisionDataB.mPosition - collisionDataA.mPosition;

    unsigned int bestIndex = 0xffffff;
    unsigned int bestSingleIndex = 0;
    float penetration = FLT_MAX;

    CHECKOVERLAP(collisionDataA.GetAxis(0), 0);
    CHECKOVERLAP(collisionDataA.GetAxis(1), 1);
    CHECKOVERLAP(collisionDataA.GetAxis(2), 2);

    CHECKOVERLAP(collisionDataB.GetAxis(0), 3);
    CHECKOVERLAP(collisionDataB.GetAxis(1), 4);
    CHECKOVERLAP(collisionDataB.GetAxis(2), 5);

    bestSingleIndex = bestIndex;

    CHECKOVERLAP(collisionDataA.GetAxis(0).Cross(collisionDataB.GetAxis(0)), 6);
    CHECKOVERLAP(collisionDataA.GetAxis(0).Cross(collisionDataB.GetAxis(1)), 7);
    CHECKOVERLAP(collisionDataA.GetAxis(0).Cross(collisionDataB.GetAxis(2)), 8);

    CHECKOVERLAP(collisionDataA.GetAxis(1).Cross(collisionDataB.GetAxis(0)), 9);
    CHECKOVERLAP(collisionDataA.GetAxis(1).Cross(collisionDataB.GetAxis(1)), 10);
    CHECKOVERLAP(collisionDataA.GetAxis(1).Cross(collisionDataB.GetAxis(2)), 11);

    CHECKOVERLAP(collisionDataA.GetAxis(2).Cross(collisionDataB.GetAxis(0)), 12);
    CHECKOVERLAP(collisionDataA.GetAxis(2).Cross(collisionDataB.GetAxis(1)), 13);
    CHECKOVERLAP(collisionDataA.GetAxis(2).Cross(collisionDataB.GetAxis(2)), 14);

    // 선택된 케이스가 면-면축 경우인지, 선-선축 경우인지에 따라 접촉데이터 생성방법이 다르다.

    if (bestIndex < 3)
    {
        FillContactDataFaceBoxBox(collisionDataA, collisionDataB, bestIndex, toCentre, penetration, oManifoold);

        // oContact를 어떻게 하든가 해야함
    }
    else if (bestIndex < 6)
    {
        FillContactDataFaceBoxBox(collisionDataB, collisionDataA, bestIndex - 3, -1.0f * toCentre, penetration,
                                  oManifoold);
        oBodySwap = true;
    }
    else
    {
        unsigned int edgeAIndex = (bestIndex - 6) / 3;
        unsigned int edgeBIndex = (bestIndex - 6) % 3;

        FillContactDataEdgeBoxBox(collisionDataA, collisionDataB, edgeAIndex, edgeBIndex, bestSingleIndex > 2, toCentre,
                                  penetration, oManifoold);
    }

    return true;
}

bool PhysicsScene::CheckOverlap(const PhysicsBoxCollisionQueryData &boxCollisionDataA,
                                const PhysicsBoxCollisionQueryData &boxCollisionDataB, CoreMath::Vector3 axis,
                                CoreMath::Vector3 toCentre, unsigned int index, float &smallestPenetration,
                                unsigned int &bestIndex) const
{

    // 축에 박스를 사영 (extent를 사영)한 길이의합이  toCentre 의 길이보다 크면 접촉, 그렇지않으면 반환된
    // 관통깊이값이 음수로 실패

    // 그 shape의 외적으로 생성된 축의 길이가 0.001이라는것은 그 사용된 두축이 거의 평행하다는것 이경우는
    // 넘어간다(면축 처리에서  거의 동일한 축을 처리했을것임으로)
    if (axis.LengthSquared() < 0.001)
        return true;
    axis.Normalize();

    float penetration = TryAxis(boxCollisionDataA, boxCollisionDataB, axis, toCentre);

    if (penetration < 0.0f)
        return false;

    if (penetration < smallestPenetration)
    {
        smallestPenetration = penetration;
        bestIndex = index;
    }

    return true;
}

// 사영시 최대값 반환
static float ProjectBoxOnToAxis(const PhysicsBoxCollisionQueryData &boxCollisionData, const CoreMath::Vector3 &axis)
{

    // project  axis
    // 축에 사영시 최대값은 반대각선 총 4방향중 하나
    // 내적을  각 축으로 분리해서 적용 .
    // 내적이  각성분곱의 합 임으로 각성분의 곱이 양수가 나오도록 하는 경우에 사영의 최대값이 나온다.
    float project = boxCollisionData.mHalfExtent.X * (abs(boxCollisionData.GetAxis(0).Dot(axis)));
    project += boxCollisionData.mHalfExtent.Y * (abs(boxCollisionData.GetAxis(1).Dot(axis)));
    project += boxCollisionData.mHalfExtent.Z * (abs(boxCollisionData.GetAxis(2).Dot(axis)));
    return project;
}

float PhysicsScene::TryAxis(const PhysicsBoxCollisionQueryData &boxCollisionDataA,
                            const PhysicsBoxCollisionQueryData &boxCollisionDataB, const CoreMath::Vector3 &axis,
                            const CoreMath::Vector3 &toCentre) const

{

    float projectA = ProjectBoxOnToAxis(boxCollisionDataA, axis);
    float projectB = ProjectBoxOnToAxis(boxCollisionDataB, axis);

    float projectCentre = abs(toCentre.Dot(axis));

    return projectA + projectB - projectCentre;
}
void PhysicsScene::FillContactDataFaceBoxBox(const PhysicsBoxCollisionQueryData &collisionDataA,
                                             const PhysicsBoxCollisionQueryData &collisionDataB, unsigned axisIndex,
                                             const CoreMath::Vector3 &toCentre, float penetration,
                                             CollisionContactManifold &oManifold) const
{

    // cliping 방식으로 개선하자

    // 면축 접촉에서
    // 해당 축방향의 면은 box에서 서로 마주보는 두개의 면이다.
    // 두 면중 다른쪽 박스에 가까운 면을 선택해야한다.

    CoreMath::Vector3 normal = collisionDataA.GetAxis(axisIndex);

    int aAxisSign = -1;
    if (normal.Dot(toCentre) > 0)
    {
        aAxisSign *= -1;
        normal *= -1.0f; //->이러면 반대쪽 박스가 다가오는방향이자 ,기존박스가 튕겨나가는방향(접촉법선)
    }
    // a의 기준면이 z+인 공간좌표
    CoreMath::Vector3 faceABasicPoint[4];
    int otherAxisIndexA0 = (axisIndex + 1) % 3;
    int otherAxisIndexA1 = (axisIndex + 2) % 3;

    // A의 halfExtent ;
    CoreMath::Vector3 eA = collisionDataA.mHalfExtent;

    // a로컬공간 -> 기준면 이 z+ 공간 변환행렬
    CoreMath::Matrix4X4 basicsMatrix = CoreMath::Matrix4X4::Identity;
    if (axisIndex == 0)
    {
        // x축
        if (aAxisSign == 1)
        {
            // Y축 기준 -90도회전
            basicsMatrix.SetColComponet(0, {0, 0, 1, 0});
            basicsMatrix.SetColComponet(2, {-1, 0, 0, 0});
            std::swap(eA.X, eA.Z);
        }
        else
        {
            // Y축 기준 + 90도 회전
            basicsMatrix.SetColComponet(0, {0, 0, -1, 0});
            basicsMatrix.SetColComponet(2, {1, 0, 0, 0});
            std::swap(eA.X, eA.Z);
        }
    }
    else if (axisIndex == 1)
    {
        // y축
        if (aAxisSign == 1)
        {
            // X축 기준 90도 회전
            basicsMatrix.SetColComponet(1, {0, 0, 1, 0});
            basicsMatrix.SetColComponet(2, {0, -1, 0, 0});
            std::swap(eA.Y, eA.Z);
        }
        else
        { // X축 기준 -90도회전
            basicsMatrix.SetColComponet(1, {0, 0, -1, 0});
            basicsMatrix.SetColComponet(2, {0, 1, 0, 0});
            std::swap(eA.Y, eA.Z);
        }
    }
    else if (axisIndex == 2)
    {
        // z축
        if (aAxisSign == 1)
        {
            // 아무것도 할게없다 .
        }
        else
        {
            basicsMatrix.mat[0].X = -1.0F;
            basicsMatrix.mat[2].Z = -1.0F;
        }
    }

    // world- > a 로 변환행렬
    CoreMath::Matrix4X4 toATransformMatrix = basicsMatrix * collisionDataA.mWorldTransform.GetInversed();

    // A로컬 기준 좌표
    int tangentAxisSign[4][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};
    for (int i = 0; i < 4; ++i)
    {
        faceABasicPoint[i][otherAxisIndexA0] = collisionDataA.mHalfExtent[otherAxisIndexA0] * tangentAxisSign[i][0];
        faceABasicPoint[i][otherAxisIndexA1] = collisionDataA.mHalfExtent[otherAxisIndexA1] * tangentAxisSign[i][1];
        faceABasicPoint[i][axisIndex] = collisionDataA.mHalfExtent[axisIndex] * aAxisSign;

        faceABasicPoint[i] = basicsMatrix.TransformPoint(faceABasicPoint[i]);
    }

    // B의 로컬기준 좌표  구하고 - > 월드공간 - > A의 로컬공간으로 변환

    // find shapeB face axis index

    float bestN = 0.0f;
    int shapeBBestIndex = -1;
    for (int i = 0; i < 3; ++i)
    {
        float n = collisionDataB.GetAxis(i).Dot(normal);

        if (std::abs(n) > bestN)
        {
            shapeBBestIndex = i;
            bestN = std::abs(n);
        }
    }
    const float contactTolerance = 0.001f;
    int sign = collisionDataB.GetAxis(shapeBBestIndex).Dot(normal) > 0 ? 1 : -1;

    CoreMath::Vector3 localVertices[4] = {};
    CoreMath::Vector3 worldVertices[4] = {};

    int tangentAxis0 = (shapeBBestIndex + 1) % 3;
    int tangentAxis1 = (shapeBBestIndex + 2) % 3;

    for (int i = 0; i < 4; ++i)
    {
        localVertices[i][tangentAxis0] = collisionDataB.mHalfExtent[tangentAxis0] * tangentAxisSign[i][0];
        localVertices[i][tangentAxis1] = collisionDataB.mHalfExtent[tangentAxis1] * tangentAxisSign[i][1];

        localVertices[i][shapeBBestIndex] = collisionDataB.mHalfExtent[shapeBBestIndex] * sign;

        worldVertices[i] = collisionDataB.mWorldTransform.TransformPoint(localVertices[i]);
    }

    // B 의 충돌면 꼭짓점들의 로컬좌표,월드좌표  구했고
    //  A의 로컬공간으로 변환

    // A로컬공간 기준 b의 입사면의 꼭짓점 좌표
    CoreMath::Vector3 incidentFaceVertices[4];
    for (int i = 0; i < 4; ++i)
    {
        incidentFaceVertices[i] = toATransformMatrix.TransformPoint(worldVertices[i]);
    }

    //+x축평면으로  incidentFace의 점을 clipping
    // 처음에는 그 입사면의 점이 각꼭짓점으로 4개 - >edge가 4개  그 edge들에대해서 기준면의 +x축 사이드평면으로
    // clipping수행
    std::vector<CoreMath::Vector3> inVertices = {incidentFaceVertices[0], incidentFaceVertices[1],
                                                 incidentFaceVertices[2], incidentFaceVertices[3]};
    std::vector<CoreMath::Vector3> outVertices;

    // axis 0 - >x , 1 -> y
    // sgin 1 , -1
    // d - >extent

    BoxBoxFaceClip(1, eA.X, 0, inVertices, outVertices);

    BoxBoxFaceClip(-1, eA.X, 0, outVertices, inVertices);

    BoxBoxFaceClip(1, eA.Y, 1, inVertices, outVertices);

    BoxBoxFaceClip(-1, eA.Y, 1, outVertices, inVertices);

    outVertices.clear();
    for (int i = 0; i < inVertices.size(); ++i)
    {

        if ((inVertices[i].Z - eA.Z) <= 0.0f)
        {
            outVertices.push_back(inVertices[i]);
        }
    }

    // 해당점들에대해서 world 좌표로 이동
    auto toWorldMatrix = toATransformMatrix.GetInversed();

    for (const auto &point : outVertices)
    {
        if (oManifold.mContacts.size() >= 4)
            break;

        float depth = eA.Z - point.Z; // 정렬 공간에서 먼저 계산
        auto worldPoint = toWorldMatrix.TransformPoint(point);

        CollisionContact contact;
        // PhysicsContact contact;
        contact.mPoint = worldPoint;
        contact.mNormal = normal;
        // contact.mPenetration = penetration;
        contact.mPenetration = depth;
        contact.mFeature.mFaceIndex = axisIndex;
        contact.mFeature.mType = EContactFeatureType::eFaceA;

        oManifold.mContacts.push_back(contact);
    }

    // int inCount = 4;
    // float d = eA.X; // x축 사이드평면까지의 거리

    // int a = inCount - 1; // 시작점
    //// edge들에대해 한정
    // for (int i = 0; i < inCount; ++i)
    //{
    //     int b = i; // edge의 끝점

    //    float da = incidentFaceVertices[a].X - d;
    //    float db = incidentFaceVertices[b].X - d;

    //    // a는 밖, b는 안에있을때
    //    if ((da > 0.0f && db <= 0.0f))
    //    {

    //        // clip되어 새로운점 계산
    //        CoreMath::Vector3 cv =
    //            (incidentFaceVertices[b] - incidentFaceVertices[a]) * (da / (da - db)) + incidentFaceVertices[a];

    //        outVertices.push_back(cv);
    //        outVertices.push_back(incidentFaceVertices[b]);
    //    }
    //    else if ((da <= 0.0f && db <= 0.0f))
    //    {
    //        // b만추가 (a,b둘다 추가하면  모서리들이 모두 안쪽일때 중복되어 넣게 되는 문제발생하기에  )
    //        outVertices.push_back(incidentFaceVertices[b]);
    //    }
    //    else if ((da <= 0.0f) && (db > 0.0f))
    //    {
    //        CoreMath::Vector3 cv =
    //            (incidentFaceVertices[b] - incidentFaceVertices[a]) * (da / (da - db)) + incidentFaceVertices[a];
    //        outVertices.push_back(cv);
    //    }

    //    a = b;
    //}

    // CoreMath::Vector3 contactPoints;

    // for (int i = 0; i < 4; ++i)
    //{

    //    // 후보 점들의 접촉 판단
    //    CoreMath::Vector3 r = worldVertices[i] - collisionDataA.mPosition;
    //    bool pass = true;
    //    for (int j = 0; j < 3; ++j)
    //    {
    //        if (collisionDataA.mHalfExtent[j] + contactTolerance < std::abs(collisionDataA.GetAxis(j).Dot(r)))
    //        {
    //            pass = false;
    //            break;
    //        }
    //    }

    //    if (pass)
    //    {
    //        CollisionContact contact;
    //        // PhysicsContact contact;
    //        contact.mPoint = worldVertices[i];
    //        contact.mNormal = normal;
    //        contact.mPenetration = penetration;
    //        // contact.mShapeAHandle = shapeA.mHandle;
    //        // contact.mShapeBHandle = shapeB.mHandle;
    //        contact.mFeature.mFaceIndex = axisIndex;
    //        contact.mFeature.mType = EContactFeatureType::eFaceA;

    //        oManifold.mContacts.push_back(contact);
    //    }
    //}

    //// 접촉점이 4개미만이면 A의 꼭짓점도 검사를 수행한다.
    // if (oManifold.mContacts.size() < 4)
    //{
    //     int aFaceSign = collisionDataA.GetAxis(axisIndex).Dot(normal) > 0 ? -1 : 1;

    //    CoreMath::Vector3 localVertices[4] = {};
    //    CoreMath::Vector3 worldVertices[4] = {};

    //    int tangentAxis0 = (axisIndex + 1) % 3;
    //    int tangentAxis1 = (axisIndex + 2) % 3;
    //    int tangentAxisSign[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
    //    for (int i = 0; i < 4; ++i)
    //    {
    //        localVertices[i][tangentAxis0] = collisionDataA.mHalfExtent[tangentAxis0] * tangentAxisSign[i][0];
    //        localVertices[i][tangentAxis1] = collisionDataA.mHalfExtent[tangentAxis1] * tangentAxisSign[i][1];

    //        localVertices[i][axisIndex] = collisionDataA.mHalfExtent[axisIndex] * aFaceSign;

    //        worldVertices[i] = collisionDataA.mWorldTransform.TransformPoint(localVertices[i]);
    //    }

    //    for (int i = 0; i < 4; ++i)
    //    {

    //        // 후보 점들의 접촉 판단
    //        CoreMath::Vector3 r = worldVertices[i] - collisionDataB.mPosition;
    //        bool pass = true;
    //        for (int j = 0; j < 3; ++j)
    //        {

    //            if (collisionDataB.mHalfExtent[j] + contactTolerance < std::abs(collisionDataB.GetAxis(j).Dot(r)))
    //            {
    //                pass = false;
    //                break;
    //            }
    //        }

    //        if (pass)
    //        {
    //            // 중복체크
    //            bool duplicate = std::find_if(oManifold.mContacts.begin(), oManifold.mContacts.end(),
    //                                          [pos = worldVertices[i]](const CollisionContact &contact)
    //                                          {
    //                                              const float dis = 0.000001f;
    //                                              if ((contact.mPoint - pos).LengthSquared() < dis)
    //                                                  return true;

    //                                              return false;
    //                                          }) != oManifold.mContacts.end();
    //            if (!duplicate)

    //            {
    //                CollisionContact contact;
    //                // PhysicsContact contact;
    //                contact.mPoint = worldVertices[i];
    //                contact.mNormal = normal;
    //                contact.mPenetration = penetration;
    //                // contact.mShapeAHandle = shapeA.mHandle;
    //                // contact.mShapeBHandle = shapeB.mHandle;
    //                contact.mFeature.mFaceIndex = axisIndex;
    //                contact.mFeature.mType = EContactFeatureType::eFaceA;

    //                oManifold.mContacts.push_back(contact);
    //            }
    //        }

    //        if (oManifold.mContacts.size() >= 4)
    //            break;
    //    }
    //}
}

// 선과선 사이의 중앙점을 반환한다 .
static CoreMath::Vector3 GetContactPoint(const CoreMath::Vector3 &edgeA, const CoreMath::Vector3 &edgeB, float aSize,
                                         float bSize, const CoreMath::Vector3 &pointOnEdgeA,
                                         const CoreMath::Vector3 &pointOnEdgeB, bool bUseEdgeB)
{

    CoreMath::Vector3 s = pointOnEdgeA - pointOnEdgeB;

    float A = edgeA.Dot(s);
    float B = edgeB.Dot(s);

    float edgeALengthSq = edgeA.LengthSquared();
    float edgeBLengthSq = edgeB.LengthSquared();

    float dotEdges = edgeA.Dot(edgeB);

    float D = (edgeALengthSq * edgeBLengthSq - dotEdges * dotEdges);

    float a = (dotEdges * B - edgeBLengthSq * A) / D;
    float b = (edgeALengthSq * B - dotEdges * A) / D;

    if (a < -aSize || a > aSize || b < -bSize || b > bSize)
    {
        // a위의 점사용
        if (bUseEdgeB)
        {
            return pointOnEdgeB;
        }
        else
        {

            return pointOnEdgeA;
        }
    }

    CoreMath::Vector3 nearestPointOnEdgeA = pointOnEdgeA + edgeA * a;
    CoreMath::Vector3 nearestPointOnEdegB = pointOnEdgeB + edgeB * b;

    return 0.5f * (nearestPointOnEdgeA + nearestPointOnEdegB);
}

void PhysicsScene::FillContactDataEdgeBoxBox(const PhysicsBoxCollisionQueryData &collisionDataA,
                                             const PhysicsBoxCollisionQueryData &collisionDataB,
                                             unsigned int bestEdgeAIndex, unsigned int bestEdgeBIndex, bool bUseEdgeB,
                                             const CoreMath::Vector3 &toCentre, float penetration,
                                             CollisionContactManifold &oManifold) const
{

    CoreMath::Vector3 edgeA = collisionDataA.GetAxis(bestEdgeAIndex);
    CoreMath::Vector3 edgeB = collisionDataB.GetAxis(bestEdgeBIndex);

    CoreMath::Vector3 axis = edgeA.Cross(edgeB);
    axis.Normalize();

    if (axis.Dot(toCentre) > 0)
        axis *= -1.0f; // b->a 로 가는방향으로

    CoreMath::Vector3 centerPointOnEdgeA = collisionDataA.mHalfExtent;
    CoreMath::Vector3 centerPointOnEdgeB = collisionDataB.mHalfExtent;
    for (int i = 0; i < 3; ++i)
    {
        if (i == bestEdgeAIndex)
        {
            centerPointOnEdgeA[i] = 0;
        }
        else if (collisionDataA.GetAxis(i).Dot(axis) > 0)
            centerPointOnEdgeA[i] *= -1.0f;

        if (i == bestEdgeBIndex)
        {
            centerPointOnEdgeB[i] = 0;
        }
        else if (collisionDataB.GetAxis(i).Dot(axis) < 0)
            centerPointOnEdgeB[i] *= -1.0f;
    }

    centerPointOnEdgeA = collisionDataA.mWorldTransform.TransformPoint(centerPointOnEdgeA);
    centerPointOnEdgeB = collisionDataB.mWorldTransform.TransformPoint(centerPointOnEdgeB);

    CollisionContact contact;

    //    PhysicsContact contact;
    // 두 박스에서 각각 4개의 모서리에서 axis와 가장가까운 모서리와 그 위의 중앙점을 잡는다.
    // 두 모서리사이의 최단거리에 해당하는 두 지점을 계산하고 그 두점 사이의  가운데를  접촉지점으로 잡는다.
    CoreMath::Vector3 point =
        GetContactPoint(edgeA, edgeB, collisionDataA.mHalfExtent[bestEdgeAIndex],
                        collisionDataB.mHalfExtent[bestEdgeBIndex], centerPointOnEdgeA, centerPointOnEdgeB, bUseEdgeB);

    contact.mPoint = point;
    contact.mNormal = axis;
    contact.mPenetration = penetration;
    // contact.mShapeAHandle = shapeA.mHandle;
    //  contact.mShapeBHandle = shapeB.mHandle;

    contact.mFeature.mEdgeAIndex = bestEdgeAIndex;
    contact.mFeature.mEdgeBIndex = bestEdgeBIndex;
    contact.mFeature.mType = EContactFeatureType::eEdgeEdge;
    contact.mFeature.bUseEdgeB = bUseEdgeB;
    oManifold.mContacts.push_back(contact);

    //    oManifold.mContactList.push_back(contact);
}
void PhysicsScene::AddManifold(const ContactManifold &manifold)
{

    if (manifold.mContactList.empty())
        return;

    // 일단 한개의 contact만 들어온다고 가정
    ContactPairKey key = ContactPairKey::MakeContactPairKey(manifold.mContactList[0].mShapeAHandle,
                                                            manifold.mContactList[0].mShapeBHandle);
    // key.mShapeHandleA = manifold.mContactList[0].mShapeAHandle;
    // key.mShapeHandleB = manifold.mContactList[0].mShapeBHandle;

    auto manifoldIt = mContactManifoldTable.find(key);
    if (manifoldIt == mContactManifoldTable.end())
    {
        mContactManifoldTable.insert({key, manifold});
        return;
    }

    const PhysicsContact &contact = manifold.mContactList[0];
    auto duplicateContactIt =
        std::find_if(manifoldIt->second.mContactList.begin(), manifoldIt->second.mContactList.end(),
                     [&contact](const PhysicsContact &ct)
                     {
                         if (contact.mShapeA != ct.mShapeA)
                             return false;
                         if (contact.mShapeB != ct.mShapeB)
                             return false;
                         if (contact.mContactFeature.mType != ct.mContactFeature.mType)
                             return false;

                         if (contact.mContactFeature.mFaceIndex != ct.mContactFeature.mFaceIndex)
                             return false;

                         if (contact.mContactFeature.mEdgeAIndex != ct.mContactFeature.mEdgeAIndex)
                             return false;

                         if (contact.mContactFeature.mEdgeBIndex != ct.mContactFeature.mEdgeBIndex)
                             return false;

                         float lengthSq = (contact.mPoint - ct.mPoint).LengthSquared();

                         if (lengthSq < 0.00001f)
                         {
                             return true;
                         }

                         return false;
                     });

    if (duplicateContactIt == manifoldIt->second.mContactList.end())
    {

        manifoldIt->second.mContactList.push_back(contact);

        if (manifoldIt->second.mContactList.size() > 4) // manifold 개수제한
        {
            std::iter_swap(manifoldIt->second.mContactList.begin(), manifoldIt->second.mContactList.end() - 1);
            manifoldIt->second.mContactList.pop_back();
        }
    }
}

void PhysicsScene::AddRigidBodyManifold(PhysicsBody &bodyA, PhysicsBody &bodyB,
                                        const CollisionContactManifold &manifold)
{

    ContactManifold rigidBodyManifold;

    for (size_t i = 0; i < manifold.mContacts.size(); ++i)
    {
        PhysicsContact physicsContact =
            BuildPhysicsContact(bodyA, bodyB, *manifold.mShapeA, *manifold.mShapeB, manifold.mContacts[i]);
        rigidBodyManifold.mContactList.push_back(physicsContact);
    }

    if (rigidBodyManifold.mContactList.empty())
        return;

    // 일단 한개의 contact만 들어온다고 가정
    ContactPairKey key = ContactPairKey::MakeContactPairKey(rigidBodyManifold.mContactList[0].mShapeAHandle,
                                                            rigidBodyManifold.mContactList[0].mShapeBHandle);
    // key.mShapeHandleA = manifold.mContactList[0].mShapeAHandle;
    // key.mShapeHandleB = manifold.mContactList[0].mShapeBHandle;

    auto manifoldIt = mContactManifoldTable.find(key);
    if (manifoldIt == mContactManifoldTable.end())
    {
        mContactManifoldTable.insert({key, rigidBodyManifold});
        return;
    }

    const PhysicsContact &contact = rigidBodyManifold.mContactList[0];
    auto duplicateContactIt =
        std::find_if(manifoldIt->second.mContactList.begin(), manifoldIt->second.mContactList.end(),
                     [&contact](const PhysicsContact &ct)
                     {
                         if (contact.mShapeA != ct.mShapeA)
                             return false;
                         if (contact.mShapeB != ct.mShapeB)
                             return false;
                         if (contact.mContactFeature.mType != ct.mContactFeature.mType)
                             return false;

                         if (contact.mContactFeature.mFaceIndex != ct.mContactFeature.mFaceIndex)
                             return false;

                         if (contact.mContactFeature.mEdgeAIndex != ct.mContactFeature.mEdgeAIndex)
                             return false;

                         if (contact.mContactFeature.mEdgeBIndex != ct.mContactFeature.mEdgeBIndex)
                             return false;

                         float lengthSq = (contact.mPoint - ct.mPoint).LengthSquared();

                         if (lengthSq < 0.00001f)
                         {
                             return true;
                         }

                         return false;
                     });

    if (duplicateContactIt == manifoldIt->second.mContactList.end())
    {

        manifoldIt->second.mContactList.push_back(contact);

        if (manifoldIt->second.mContactList.size() > 4) // manifold 개수제한
        {
            std::iter_swap(manifoldIt->second.mContactList.begin(), manifoldIt->second.mContactList.end() - 1);
            manifoldIt->second.mContactList.pop_back();
        }
    }
}
void PhysicsScene::AddKinematicContacts(PhysicsBody &bodyA, PhysicsBody &bodyB,
                                        const CollisionContactManifold &manifold)
{

    for (int i = 0; i < manifold.mContacts.size(); ++i)
    {

        KinematicContact contact =
            BuildKinematicContact(bodyA, bodyB, *manifold.mShapeA, *manifold.mShapeB, manifold.mContacts[i]);

        mKinematicContactList.push_back(contact);
    }
}

void PhysicsScene::RecalculateBodyInertia(PhysicsBody &body)
{
    if (body.mBodyType != EPhysicsBodyType::eDynamic)
    {
        body.mInertia = CoreMath::Matrix4X4::Zero;
        body.mInertiaInverse = CoreMath::Matrix4X4::Zero;
        body.mInertiaWorld = CoreMath::Matrix4X4::Zero;
        body.mInertiaInverseWorld = CoreMath::Matrix4X4::Zero;
        return;
    }

    if (body.mShapeHandleList.empty())
    {
        body.mInertia = CoreMath::Matrix4X4::Identity;
        body.mInertiaInverse = CoreMath::Matrix4X4::Identity;
        body.mInertiaWorld = CoreMath::Matrix4X4::Zero;
        body.mInertiaInverseWorld = CoreMath::Matrix4X4::Zero;
        return;
    }

    // 일단 shape 하나 일경우에 로직
    PhysicsShape *shape = GetPhysicsShape(body.mShapeHandleList[0]);
    if (shape == nullptr)
        return;

    body.mInertia = shape->CalculateLocalInertiaTensor();
    body.mInertiaInverse = body.mInertia.GetInversed();
}

void PhysicsScene::CalcForces(PhysicsBody &body)
{

    // 씬의 전역적인 힘 적용

    // 물체 바디에 적용되는힘 계산
    // 중력

    if (body.mForce.LengthSquared() > 0.01f)
    {
        body.SetAwake(true);
    }

    if (!body.mIsAwake)
        return;

    if (body.mBodyType == EPhysicsBodyType::eDynamic && body.mGravity)
    {
        body.mForce += {0, -980.0F * body.mMass, 0};
        body.mLastFrameAcceleration = {0, -9.8F, 0};
    }

    // for (size_t i = 0; i < body.mShapeHandleList.size(); ++i)
    //{

    //    PhysicsShape *shape = GetPhysicsShape(body.mShapeHandleList[i]);
    //    if (shape == nullptr)
    //        continue;

    //    // Shape에 적용되는 힘 계산
    //}
}

void PhysicsScene::UpdateBodyEuler(float deltaTime, PhysicsBody &body)
{

    if (body.mBodyType != EPhysicsBodyType::eDynamic)
    {
        body.mForce = {0.0f, 0.0f, 0.0f};
        body.mTorque = {0.0f, 0.0f, 0.0f};
        return;
    }

    if (!body.mIsAwake)
    {
        return;
    }
    // 여기도 body의 shape 단위로 처리해야함. 여기서부터 시작
    // body에 중력(힘)이 적용되고나서 body의 위치,방향만 수치적분으로 바뀌고 shape의 트랜스폼은 그대로인게 문제
    // shape들의 local 트랜스폼을바탕으로 다시 월드 트랜스폼을 계산해줘야함.

    // 수치적분
    // 오일러적분

    // 선운동방정식 적분

    // 가속도
    CoreMath::Vector3 a = body.mForce / body.mMass;

    //  body.mLastFrameAcceleration = a;

    CoreMath::Vector3 dv = a * deltaTime;

    body.mVelocity += dv;

    // 각운동방정식 적분
    body.mAngularVelocity += body.mInertiaInverseWorld *
                             (body.mTorque - body.mAngularVelocity.Cross(body.mInertiaWorld * body.mAngularVelocity)) *
                             deltaTime;

    CoreMath::Vector3 ds = body.mVelocity * deltaTime;

    body.mPosition += ds;

    if (ds.LengthSquared() > 0.00001f || body.mAngularVelocity.LengthSquared() > 0.00000001f)
    {
        body.mTransformSyncDirty = true;
    }

    // 각속도 damping
    body.mAngularVelocity *= 0.98f;
    if (body.mAngularVelocity.Length() < 0.0000001f)
    {
        body.mAngularVelocity = {0, 0, 0};
    }
    // CoreMath::Quaternion angularVelocity(body.mAngularVelocity);
    body.mRotation.ApplyAngularFromVector(body.mAngularVelocity * deltaTime);
    // body.mRotation += 0.5f * angularVelocity * body.mRotation * deltaTime;

    // body.mRotation += 0.5f * body.mRotation * body.mAngularVelocity * deltaTime;

    body.mRotation.Normalize();

    body.mForce = {0, 0, 0};
    body.mTorque = {0, 0, 0};

    body.UpdateDerivedData();
    RebuildShapesWorldTransformFromBody(body);
}

void PhysicsScene::UpdateKinematicBodyEuler(float deltaTime, PhysicsBody &body)
{

    if (!body.mIsAwake)
    {
        return;
    }

    CoreMath::Vector3 ds = body.mVelocity * deltaTime;
    body.mPosition += ds;
    if (ds.LengthSquared() > 0.00001f /*|| body.mAngularVelocity.LengthSquared() > 0.00000001f*/)
    {
        body.mTransformSyncDirty = true;
    }

    RebuildShapesWorldTransformFromBody(body);
}

void PhysicsScene::CalcInertia(PhysicsBody &body)
{
    body.mInertia = CoreMath::Matrix4X4::Identity;
    body.mInertiaInverse = CoreMath::Matrix4X4::Identity;
    // switch (body.mCollisionShape.mShapeType)
    //{
    // case EPhysicsBodyShapeType::eNone:

    //    break;

    // case EPhysicsBodyShapeType::eBox:
    //{
    //     float a = body.mCollisionShape.mBoxData.mHalfExtent[0]; // x
    //     float b = body.mCollisionShape.mBoxData.mHalfExtent[1]; // y
    //     float c = body.mCollisionShape.mBoxData.mHalfExtent[2]; // z

    //    body.mInertia = CoreMath::Matrix4X4::Identity;

    //    body.mInertia.mat[0][0] = 1.0f / 12 * body.mMass * (b * b + c * c);
    //    body.mInertia.mat[1][1] = 1.0f / 12 * body.mMass * (a * a + c * c);
    //    body.mInertia.mat[2][2] = 1.0f / 12 * body.mMass * (a * a + b * b);

    //    body.mInertiaInverse = body.mInertia.GetInversed();
    //}
    // break;

    // case EPhysicsBodyShapeType::eConvex:

    //    break;

    // case EPhysicsBodyShapeType::eTriangleMesh:

    //    break;
    //}
}
void PhysicsScene::SyncBodiesFromExternal()
{

    // 외부에서 들어온 command 꺼내서 수행
    std::vector<PhysicsTransformCommand> &transformCommandList = mPhysicsCommandContext.mTransformCommandList;

    for (const auto &command : transformCommandList)
    {

        PhysicsBody *pBody = GetPhysicsBody(command.mBodyHandle);

        if (pBody == nullptr)
            continue;

        pBody->mPosition = command.mWorldPosition;
        pBody->mRotation = command.mWorldRotation;
        pBody->mScale = command.mWorldScale;

        // body 와 연결된 shape 들의 world transform을 업데이트 한다.
        // UpdateShapeTransformFromBody();
        for (auto shapeHandle : pBody->mShapeHandleList)
        {
            auto pShape = GetPhysicsShape(shapeHandle);
            if (pShape)
            {
                UpdateShapeFromBody(*pBody, *pShape);
            }
        }

        RecalculateBodyInertia(*pBody);
    }

    // 힘은 나중에
    std::vector<PhysicsAddForceCommand> &addForceCommandList = mPhysicsCommandContext.mAddForceCommandList;
    for (const auto &command : addForceCommandList)
    {

        PhysicsBody *pBody = GetPhysicsBody(command.mBodyHandle);
        if (pBody == nullptr)
            continue;

        pBody->mForce += command.mForce;
    }

    std::vector<PhysicsSetVelocityCommmand> &setVelocityCommandList = mPhysicsCommandContext.mSetVelocityCommandList;
    for (const auto &command : setVelocityCommandList)
    {

        PhysicsBody *pBody = GetPhysicsBody(command.mBodyHandle);
        if (pBody == nullptr)
            continue;

        pBody->mVelocity = command.mVelocity;
    }

    ClearPhysicsCommand();
}
void PhysicsScene::SyncBodiesToExternal()
{

    // 외부로 동기화 수행

    // 1.
    // 한가지방법은 동기화알림을 큐에 담아놓고외부에서 뽑아갈수있도록?
    // 이러면 이제외부 중앙시스템이 뽑아서 각각 물체들의 동기화를수행하는구조여야하고

    // dirty 인 body들만 push

    for (auto pBody : mPhysicsBodyList)
    {
        if (pBody && pBody->mTransformSyncDirty)
        {

            PhysicsTransformResult transformResult;
            transformResult.mBodyHandle = pBody->mHandle;
            // pBody->mRotation.Normalize();
            transformResult.mRotation = pBody->mRotation;
            transformResult.mPosition = pBody->mPosition;
            mPhysicsFrameResult.mTransformResults.push_back(transformResult);

            pBody->mTransformSyncDirty = false;
        }
    }
}

void PhysicsScene::ClearPhysicsCommand()
{

    mPhysicsCommandContext.mTransformCommandList.clear();
    mPhysicsCommandContext.mSizeCommnadList.clear();
    mPhysicsCommandContext.mAddForceCommandList.clear();
    mPhysicsCommandContext.mSetVelocityCommandList.clear();
}

void PhysicsScene::GetShapeLocalTransformFromBody(const PhysicsBody &body, PhysicsShape &shape)
{

    // shape의 월드를 body의 로컬로 이동

    CoreMath::Quaternion invBodyRotation = body.mRotation.GetConjugate();

    shape.mLocalPosition = invBodyRotation.RotateVector(shape.mWorldPosition - body.mPosition);
    shape.mLocalRotation = invBodyRotation * shape.mWorldRotation;
    shape.mLocalRotation.Normalize();
}
void PhysicsScene::UpdateShapeFromBody(const PhysicsBody &body, PhysicsShape &shape)
{

    UpdateShapeWorldTransformFromBody(body, shape);

    shape.UpdateShapeSpecificData();
}

void PhysicsScene::UpdateShapeWorldTransformFromBody(const PhysicsBody &body, PhysicsShape &shape)
{
    CoreMath::Quaternion bodyRotation = body.mRotation;

    CoreMath::Vector3 scaledLocalPosition = shape.mLocalPosition * body.mScale;

    shape.mWorldPosition = bodyRotation.RotateVector(scaledLocalPosition) + body.mPosition;
    shape.mWorldRotation = bodyRotation * shape.mLocalRotation;
    shape.mWorldRotation.Normalize();
    shape.mWorldScale = body.mScale * shape.mLocalScale;

    shape.mWorldTransform = CoreMath::Matrix4X4::MakeTransform(shape.mWorldPosition, shape.mWorldRotation, {1, 1, 1});
}

void PhysicsScene::RebuildShapesWorldTransformFromBody(const PhysicsBody &body)
{

    for (PhysicsShapeHandle shapeHandle : body.mShapeHandleList)
    {
        if (shapeHandle == PhysicsShapeHandleInValid)
            continue;

        PhysicsShape *pShape = GetPhysicsShape(shapeHandle);
        if (pShape == nullptr)
            continue;

        UpdateShapeFromBody(body, *pShape);
        // UpdateShapeWorldTransformFromBody(body, *pShape);
    }
}

bool PhysicsScene::IsSleep(const PhysicsBody &bodyA, const PhysicsBody &bodyB) const
{

    if (bodyA.mIsAwake || bodyB.mIsAwake)
        return false;

    return true;
}

ECollisionResponsePath PhysicsScene::ClassifyCollisionPair(const PhysicsBody &bodyA, const PhysicsBody &bodyB) const
{

    EPhysicsBodyType bodyTypeA = bodyA.mBodyType;
    EPhysicsBodyType bodyTypeB = bodyB.mBodyType;

    if (bodyTypeA == EPhysicsBodyType::eDynamic || bodyTypeB == EPhysicsBodyType::eDynamic)
    {
        return ECollisionResponsePath::eRigidBodySolver;
    }

    if (bodyTypeA == EPhysicsBodyType::eStatic && bodyTypeB == EPhysicsBodyType::eStatic)
    {
        return ECollisionResponsePath::eNone;
    }

    // 일단 둘다 kinematic 인경우는 고려안함
    if (bodyTypeA == EPhysicsBodyType::eKinematic && bodyTypeB == EPhysicsBodyType::eKinematic)
    {
        return ECollisionResponsePath::eNone;
    }

    // 각각 kinematic , static 인경우
    return ECollisionResponsePath::eKinematicSolver;
}

PhysicsGroundResult &PhysicsScene::GetOrCreateGroundResult(PhysicsBodyHandle handle, PhysicsShapeHandle shapeHandle)
{

    auto it = std::find_if(mPhysicsFrameResult.mGroundResults.begin(), mPhysicsFrameResult.mGroundResults.end(),
                           [handle, shapeHandle](const PhysicsGroundResult &groundResult)
                           {
                               if (groundResult.mBodyHandle == handle && groundResult.mShapeHandle == shapeHandle)
                               {
                                   return true;
                               }
                               return false;
                           });

    if (it != mPhysicsFrameResult.mGroundResults.end())
    {
        return *it;
    }
    else
    {
        PhysicsGroundResult result;
        result.mBodyHandle = handle;
        result.mShapeHandle = shapeHandle;
        result.mIsGrounded = false;
        result.mGroundBodyHandle = PhysicsBodyHandleInValid;
        mPhysicsFrameResult.mGroundResults.push_back(result);
        return mPhysicsFrameResult.mGroundResults.back();
    }
}

void PhysicsScene::UpdateGroundResult(const KinematicContact &contact)
{

    if (CheckGroundNormal(contact.mNormalTowardKinematic))
    {
        PhysicsGroundResult &result =
            GetOrCreateGroundResult(contact.mKinematicBodyHandle, contact.mKinematicShapeHandle);
        FillGroundResult(true, contact.mNormalTowardKinematic, contact.mOtherBodyHandle, result);
    }
}

void PhysicsScene::CheckPreviousGroundResult()
{

    auto &groundResults = mPhysicsFrameResult.mGroundResults;

    for (size_t i = 0; i < groundResults.size(); ++i)
    {
        if (groundResults[i].mIsGrounded == false)
            continue;

        PhysicsBodyHandle bodyHandle = groundResults[i].mBodyHandle;

        PhysicsBody *body = GetPhysicsBody(bodyHandle);
        if (body == nullptr)
            continue;

        PhysicsShape *shape = GetPhysicsShape(groundResults[i].mShapeHandle);
        if (shape == nullptr)
            continue;

        // copy

        // 다시 충돌감지
        bool bGrounded = false;
        CoreMath::Vector3 probOffset = {0, -1, 0};
        float offsetDistance = 0.2f;
        probOffset *= offsetDistance;

        for (size_t j = 0; j < mPhysicsShapeList.size(); ++j)
        {
            PhysicsShape *otherShape = mPhysicsShapeList[j];

            if (otherShape == nullptr)
                continue;

            if (otherShape->mAttachedBodyHandle == bodyHandle)
                continue;

            CollisionContactManifold manifold;
            bool bodySwap = false;
            bool collided = DectectCollisionWithOffset(*shape, probOffset, *otherShape, manifold, bodySwap);

            // 임시처리 contact.empty(), - 충돌감지가 완전히 정확하기 전까지는
            if (collided && !manifold.mContacts.empty())
            {
                if (CheckGroundNormal(manifold.mContacts[0].mNormal))
                {
                    bGrounded = true;
                    FillGroundResult(true, {0, 1, 0}, otherShape->mAttachedBodyHandle, groundResults[i]);
                    break;
                }
            }
        }

        if (bGrounded == false)
        {
            groundResults[i].mIsGrounded = false;
        }
    }
}

bool PhysicsScene::CheckGroundNormal(const CoreMath::Vector3 &normal)
{

    CoreMath::Vector3 up(0, 1, 0);
    float threshold = 0.707f;

    float cos = normal.GetNormalize().Y;

    if (cos > threshold)
    {
        return true;
    }
    return false;
}

void PhysicsScene::MakeCollisionResponseData()
{

    // curr list

    // pre list
    std::vector<bool> preCheckList(mCollisionPreFrameStateList.size(), false);

    for (size_t i = 0; i < mCollisionCurrFrameStateList.size(); ++i)
    {
        bool bCurrStateBegin = true; // 처음 시작의 경우인가? 즉 currFrame에만존재하는가.
        for (size_t j = 0; j < mCollisionPreFrameStateList.size(); ++j)
        {
            if (mCollisionCurrFrameStateList[i].isSame(mCollisionPreFrameStateList[j]))
            {

                if (mCollisionCurrFrameStateList[i].mResponseType == EPhysicsCollisionChannelResponseType::eOverlap)
                {
                    // overlap 유지
                }
                preCheckList[j] = true;
                bCurrStateBegin = false;
                break;
            }
        }

        if (bCurrStateBegin)
        {
            if (mCollisionCurrFrameStateList[i].mResponseType == EPhysicsCollisionChannelResponseType::eOverlap)
            {
                // overlap begin
                // Body 쌍당 하나만 저장하며 양쪽 컴포넌트 전달은 Bridge가 담당한다.
                // 현재 Body 기준 이벤트에는 Shape 식별자를 제공하지 않는다.
                const PhysicsCollisionPairState &state = mCollisionCurrFrameStateList[i];
                PushCollisionResponseData(state.mResponseType, EPhysicsCollisionResponseEventType::eBegin,
                                          state.mHandleA, state.mHandleB, PhysicsShapeHandleInValid,
                                          PhysicsShapeHandleInValid);
            }
        }
    }

    for (int i = 0; i < preCheckList.size(); ++i)
    {
        if (preCheckList[i] == false)
        { // curr에는 없던 state들 - >end

            if (mCollisionPreFrameStateList[i].mResponseType == EPhysicsCollisionChannelResponseType::eOverlap)
            {
                // overlap end
                // 사라진 관계의 식별자는 이전 상태에서 얻는다. Body가 이미 제거됐어도
                // 포인터 대신 Handle을 전달하고 생존 여부는 Bridge에서 확인한다.
                const PhysicsCollisionPairState &state = mCollisionPreFrameStateList[i];
                PushCollisionResponseData(state.mResponseType, EPhysicsCollisionResponseEventType::eEnd, state.mHandleA,
                                          state.mHandleB, PhysicsShapeHandleInValid, PhysicsShapeHandleInValid);
            }
        }
    }
    // 비교 완료 후 현재 관계를 다음 갱신의 기준으로 보관한다. 현재 목록을 비워
    // 다음 수집에서 이전 관계가 누적되지 않도록 한다.
    mCollisionPreFrameStateList.swap(mCollisionCurrFrameStateList);
    mCollisionCurrFrameStateList.clear();
}

void PhysicsScene::FillGroundResult(bool bIsGrounded, const CoreMath::Vector3 &groundNormal,
                                    PhysicsBodyHandle groundHandle, PhysicsGroundResult &oResult)
{
    oResult.mIsGrounded = bIsGrounded;
    oResult.mGroundNormal = groundNormal;
    oResult.mGroundBodyHandle = groundHandle;
}

void PhysicsScene::PushCollisionResponseData(EPhysicsCollisionChannelResponseType type,
                                             EPhysicsCollisionResponseEventType event, PhysicsBodyHandle bodyHandle,
                                             PhysicsBodyHandle otherBodyHandle, PhysicsShapeHandle shapeHandle,
                                             PhysicsShapeHandle otherShapeHandle)
{

    PhysicsCollisionResponseData data;
    data.mResponseType = type;
    data.mEventType = event;
    data.mBodyHandle = bodyHandle;
    data.mOtherBodyHandle = otherBodyHandle;
    data.mShapeHandle = shapeHandle;
    data.mOhterShapeHandle = otherShapeHandle;

    mCollisionResponseEventList.push_back(data);
}

void PhysicsScene::PushCurrPhysicsCollisionPairState(PhysicsBodyHandle handleA, PhysicsBodyHandle handleB,
                                                     EPhysicsCollisionChannelResponseType responseType)
{

    PhysicsCollisionPairState state;
    state.SetHandles(handleA, handleB);
    state.mResponseType = responseType;
    mCollisionCurrFrameStateList.push_back(state);
}

void PhysicsScene::BoxBoxFaceClip(int sign, float d, int axisIndex, const std::vector<CoreMath::Vector3> &inVertices,
                                  std::vector<CoreMath::Vector3> &oOutVertices) const
{
    oOutVertices.clear();
    int inCount = inVertices.size();

    int a = inCount - 1; // 시작점
    // edge들에대해 한정
    for (int i = 0; i < inCount; ++i)
    {
        int b = i; // edge의 끝점

        float da = sign * inVertices[a][axisIndex] - d;
        float db = sign * inVertices[b][axisIndex] - d;

        // a는 밖, b는 안에있을때
        if ((da > 0.0f && db <= 0.0f))
        {

            // clip되어 새로운점 계산
            CoreMath::Vector3 cv = (inVertices[b] - inVertices[a]) * (da / (da - db)) + inVertices[a];

            oOutVertices.push_back(cv);
            oOutVertices.push_back(inVertices[b]);
        }
        else if ((da <= 0.0f && db <= 0.0f))
        {
            // b만추가 (a,b둘다 추가하면  모서리들이 모두 안쪽일때 중복되어 넣게 되는 문제발생하기에  )
            oOutVertices.push_back(inVertices[b]);
        }
        else if ((da <= 0.0f) && (db > 0.0f))
        {
            CoreMath::Vector3 cv = (inVertices[b] - inVertices[a]) * (da / (da - db)) + inVertices[a];
            oOutVertices.push_back(cv);
        }

        a = b;
    }
}

//
//// 충돌 처리
// void PhysicsScene::ResolveContact(const PhysicsContact &contact) {}
