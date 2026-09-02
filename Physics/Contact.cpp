#include "Contact.h"
#include <Physics/Collider.h>

ContactPairKey ContactPairKey::MakeContactPairKey(PhysicsShapeHandle a, PhysicsShapeHandle b)
{

    return {std::max(a, b), std::min(a, b)};
}

void PhysicsContact::CalculateInternals(float deltaTime)
{

    CalculateContactBasic();

    CoreMath::Vector3 relativeVel;
    if (mBodyA)
    {
        mRelativeContactPosition[0] = mPoint - mBodyA->mPosition;
        relativeVel = mBodyA->mVelocity + mBodyA->mAngularVelocity.Cross(mRelativeContactPosition[0]);
    }

    if (mBodyB)
    {

        mRelativeContactPosition[1] = mPoint - mBodyB->mPosition;

        relativeVel -= mBodyB->mVelocity + mBodyB->mAngularVelocity.Cross(mRelativeContactPosition[1]);
    }

    mRelativeVelocity = mContactToWorld.GetTransposed().TransformDirection(relativeVel);

    CalculateVelocity(deltaTime);
}

void PhysicsContact::CalculateContactBasic()
{

    CoreMath::Vector3 vec[2];

    mNormal.Normalize();
    if (mNormal.LengthSquared() < 0.0001)
    {
        mContactToWorld = CoreMath::Matrix4X4::Identity;
        return;
    }

    if (abs(mNormal.X) > abs(mNormal.Y))
    {
        // normal이 x축에 가깝다.
        // 월드의 up벡터를 사용한다. .

        const float s = 1.0 / sqrtf((mNormal.X * mNormal.X + mNormal.Z * mNormal.Z));

        vec[1].X = -mNormal.Z * s;
        vec[1].Y = 0;
        vec[1].Z = mNormal.X * s;

        vec[0].X = -mNormal.Y * vec[1].Z;
        vec[0].Y = -vec[1].X * mNormal.Z + vec[1].Z * mNormal.X;
        vec[0].Z = vec[1].X * mNormal.Y;
    }
    else
    {
        // normal이 월드의 up벡터에 가까우니
        //  월드의 x축을 사용한다 .
        const float s = 1.0f / sqrtf((mNormal.Y * mNormal.Y + mNormal.Z * mNormal.Z));

        vec[1].X = 0;
        vec[1].Y = -mNormal.Z * s;
        vec[1].Z = mNormal.Y * s;

        vec[0].X = mNormal.Z * vec[1].Y - mNormal.Y * vec[1].Z;
        vec[0].Y = mNormal.X * vec[1].Z;
        vec[0].Z = -mNormal.X * vec[1].Y;
    }

    // mContactToWorld.SetColComponet(0, CoreMath::Vector4{mNormal, 0.0f});
    //  mContactToWorld.SetColComponet(1, CoreMath::Vector4{vec[0], 0.0f});
    //   mContactToWorld.SetColComponet(2, CoreMath::Vector4{vec[1], 0.0f});
    mContactToWorld.SetColComponents(CoreMath::Vector4{mNormal, 0.0f}, CoreMath::Vector4{vec[0], 0.0f},
                                     CoreMath::Vector4{vec[1], 0.0f});
}

CoreMath::Matrix4X4 PhysicsContact::CalculateVelocityPerUnitImpluse()
{

    if (mBodyA == nullptr)
        return {};

    float InvMass = mBodyA->GetInverseMass();
    // 물체의 중심으로부터의 접촉지점까지의 상대벡터
    CoreMath::Vector3 r = mPoint - mBodyA->mPosition;

    // 각충격량
    //  단위임펄스 = normal( 법선방향만 현재 고려하고있음으로 )

    CoreMath::Matrix4X4 MatrixR;
    MatrixR.SetSkewSymmetric(r);

    CoreMath::Matrix4X4 u = MatrixR;
    //*mContactToWorld;

    // 각속도의 변화량
    CoreMath::Matrix4X4 t = mBodyA->mInertiaInverseWorld * u;

    // 접촉지점에서의 각속도변화량(회전)에따른 속도 변화량
    CoreMath::Matrix4X4 velWorld = MatrixR * t;
    velWorld *= -1.0f;

    // float angularComponent = mContactToWorld.GetTransposed().TransformDirection(vel).X;

    // float linearComponent = mBodyA->GetInverseMass();

    //// 법선방향으로 속도 변화량 (ShapeA물체만고려)
    // float deltaVelocity = angularComponent + linearComponent;

    if (mBodyB != nullptr)
    {

        // 물체의 중심으로부터의 접촉지점까지의 상대벡터
        CoreMath::Vector3 r = mPoint - mBodyB->mPosition;

        // 각충격량
        //  단위임펄스 = normal( 법선방향만 현재 고려하고있음으로 )

        CoreMath::Matrix4X4 MatrixR;
        MatrixR.SetSkewSymmetric(r);

        CoreMath::Matrix4X4 u = MatrixR;
        //        *mContactToWorld;

        // 각속도의 변화량
        CoreMath::Matrix4X4 t = mBodyB->mInertiaInverseWorld * u;

        // 접촉지점에서의 각속도변화량(회전)에따른 속도 변화량
        CoreMath::Matrix4X4 velWorldB = MatrixR * t;
        velWorldB *= -1.0f;

        velWorld += velWorldB;

        InvMass += mBodyB->GetInverseMass();

        // 물체 b도 움직이는 물체라면
        //  상대속도를 계산해줘야한다.
        //

        // CoreMath::Vector3 r = mPoint - mBodyB->mPosition;

        // CoreMath::Vector3 u = r.Cross(mNormal);
        //// 각속도의 변화량
        // CoreMath::Vector3 t = mBodyB->mInertiaInverseWorld.TransformDirection(u);

        // CoreMath::Vector3 vel = t.Cross(r);

        // float angularComponent = mContactToWorld.GetTransposed().TransformDirection(vel).X;

        // float linearComponent = mBodyB->GetInverseMass();

        // deltaVelocity += (angularComponent + linearComponent);
    }

    // 접촉좌표계 기준 충격에서 속도변화를 구하는 행렬

    CoreMath::Matrix4X4 deltaVelocity = mContactToWorld.GetTransposed();
    deltaVelocity *= velWorld;
    deltaVelocity *= mContactToWorld;

    deltaVelocity.mat[0].X += InvMass;
    deltaVelocity.mat[1].Y += InvMass;
    deltaVelocity.mat[2].Z += InvMass;

    return deltaVelocity;
}

void PhysicsContact::CalculateVelocity(float deltaTime)
{

    CoreMath::Vector3 scaledContact = mNormal * deltaTime;

    float velocityFromAcc = mBodyA->mLastFrameAcceleration.Dot(scaledContact);

    if (mBodyB)
    {
        velocityFromAcc -= mBodyB->mLastFrameAcceleration.Dot(scaledContact);
    }

    float restitution = mRestitution;
    // restitution = 0.0F;

    const static float velocityLimit = 0.5f;

    if (abs(mRelativeVelocity.X) < velocityLimit)
    {
        restitution = 0.0f;
    }

    float deltaVelocity = -mRelativeVelocity.X - restitution * (mRelativeVelocity.X - velocityFromAcc);
    mDesiredVelocity = deltaVelocity;
    mDesiredVelocity = std::max(deltaVelocity, 0.0f);
    // mDesiredVelocity.Y =
}

CoreMath::Vector3 PhysicsContact::CalculateImpulse(float deltaTime)
{

    CoreMath::Matrix4X4 velocityPerUnitImpulse = CalculateVelocityPerUnitImpluse();

    // 요구되는 필요한 속도변화량 계산
    CalculateVelocity(deltaTime);

    // 경사면 접평면방향 속도 추가
    CoreMath::Vector3 velocityFromAcc = mBodyA->mLastFrameAcceleration * deltaTime;

    if (mBodyB)
    {
        velocityFromAcc -= mBodyB->mLastFrameAcceleration * deltaTime;
    }

    velocityFromAcc = mContactToWorld.GetTransposed().TransformDirection(velocityFromAcc);

    // velocityFromAcc.Y = 0;
    // velocityFromAcc.Z = 0;

    // mRelativeVelocity.Y += velocityFromAcc.Y;
    //    mRelativeVelocity.Z += velocityFromAcc.Z;

    CoreMath::Vector3 mVelKill{mDesiredVelocity, -(mRelativeVelocity.Y + velocityFromAcc.Y),
                               -(mRelativeVelocity.Z + velocityFromAcc.Z)};

    CoreMath::Vector3 impulse;

    // 단위속도당 필요한 충격량
    CoreMath::Matrix4X4 impulseMatrix = velocityPerUnitImpulse.GetInversed();

    // 필요한 충격량
    CoreMath::Vector3 impulseContact = impulseMatrix.TransformDirection(mVelKill);

    // if (impulseContact.X < 0.0f)
    //     return {0, 0, 0};
    // 마찰 고려

    float planarImpulseSqMagnitude =
        std::sqrt(impulseContact.Y * impulseContact.Y + impulseContact.Z * impulseContact.Z);

    float friction = 1.0f;
    constexpr float impulseEpsilon = 0.000001f;
    if (planarImpulseSqMagnitude > impulseEpsilon && planarImpulseSqMagnitude > (impulseContact.X * friction))
    {
        // 동적마찰을 적용한다.
        impulseContact.Y /= planarImpulseSqMagnitude;
        impulseContact.Z /= planarImpulseSqMagnitude;

        impulseContact.X = velocityPerUnitImpulse.mat[0][0] +
                           velocityPerUnitImpulse.mat[1][0] * impulseContact.Y * friction +
                           velocityPerUnitImpulse.mat[2][0] * impulseContact.Z * friction;

        if (std::abs(impulseContact.X) <= impulseEpsilon)
            return {0, 0, 0};

        impulseContact.X = mDesiredVelocity / impulseContact.X;
        impulseContact.Y *= friction * impulseContact.X;
        impulseContact.Z *= friction * impulseContact.X;
    }

    //// 그리고 velocityPerUnitImpulse.X가 0 또는 매우 작은 경우에는 나눗셈을 막아야 한다.   ??
    //// 엄청 큰 임펄스가 필요하다는뜻인데 일단 실행해서 문제를 직접보고 처리하자
    // impulse.X = mDesiredVelocity.X / velocityPerUnitImpulse.X;
    // impulse.Y = 0;
    // impulse.Z = 0;

    impulseContact = mContactToWorld.TransformDirection(impulseContact);

    return impulseContact;
}

void PhysicsContact::ResolvePenetration(CoreMath::Vector3 linearChange[2], CoreMath::Vector3 angularChange[2])
{

    // 선형 투영
    // 역질량에 비례

    // float total = 0.0f;

    // if (mBodyA->mBodyType == EPhysicsBodyType::eDynamic)
    //{
    //     total += 1.0f / mBodyA->mMass;
    // }

    // if (mBodyB != nullptr || mBodyB->mBodyType == EPhysicsBodyType::eDynamic)
    //{
    //     total += 1.0f / mBodyB->mMass;
    // }

    //// a의 이동거리
    // float dA = mPenetration * (1.0f / mBodyA->mMass) / total;

    // float dB = mPenetration * (1.0f / mBodyB->mMass) / total;

    // if (mBodyA->mBodyType == EPhysicsBodyType::eDynamic)
    //{
    //     mBodyA->mPosition += mNormal * dA;
    // }

    // if (mBodyB->mBodyType == EPhysicsBodyType::eDynamic)
    //{

    //    mBodyB->mPosition += -1.0f * mNormal * dB;
    //}

    // 비선형 투영
    // 1. 각 물체의 선운동,각운동의 이동량 계산

    float linearUnitMove[2] = {0, 0};
    float angularUnitMove[2] = {0, 0};

    float linearMove[2] = {0, 0};
    float angularMove[2] = {0, 0};

    CoreMath::Vector3 angularVelocityPerUnitImpulse[2] = {};

    float totalUnitMove = 0.0f;

    PhysicsBody *body[2] = {mBodyA, mBodyB};

    for (int i = 0; i < 2; ++i)
    {
        // 단위충격 기준
        // 선운동 위치변화량은 역질량에 비례한다 .
        if (body[i] == nullptr || body[i]->mBodyType != EPhysicsBodyType::eDynamic)
            continue;

        linearUnitMove[i] = body[i]->GetInverseMass();

        CoreMath::Matrix4X4 inverseInertiaTensor = body[i]->mInertiaInverseWorld;

        CoreMath::Vector3 r = mPoint - body[i]->mPosition;

        // 단위 충격에 따른 각충격량
        CoreMath::Vector3 u = r.Cross(mNormal);

        // 각속도변화량
        CoreMath::Vector3 s = inverseInertiaTensor * u;
        angularVelocityPerUnitImpulse[i] = s;

        // 접촉지점에서의 각속도변화에 따른 속도변화량
        s = s.Cross(r);

        // 접촉 법선방향 속도변화량

        angularUnitMove[i] = s.Dot(mNormal);

        totalUnitMove += linearUnitMove[i] + angularUnitMove[i];
    }

    if (std::abs(totalUnitMove) < 0.00001f)
        return;

    float invTotalUnitMove = 1.0f / totalUnitMove;
    linearMove[0] = linearUnitMove[0] * invTotalUnitMove * mPenetration;
    linearMove[1] = -linearUnitMove[1] * invTotalUnitMove * mPenetration;

    angularMove[0] = angularUnitMove[0] * invTotalUnitMove * mPenetration;
    angularMove[1] = -angularUnitMove[1] * invTotalUnitMove * mPenetration;

    // 2. 선성분과 각성분을 계산하여 위치와 회전 반영

    for (int i = 0; i < 2; ++i)
    {
        if (body[i] && body[i]->mBodyType == EPhysicsBodyType::eDynamic)
        {
            float totalMove = linearMove[i] + angularMove[i];
            CoreMath::Vector3 r = mRelativeContactPosition[i]; //  body[i]->mPosition - mPoint;
                                                               /* float length = r.Length() - r.Dot(mNormal);
                                                   
                                                                float limit = length * 0.2f;*/

            CoreMath::Vector3 projection = r - mNormal * r.Dot(mNormal);

            float limit = projection.Length() * 0.2f;

            if (abs(angularMove[i]) > limit)
            {

                if (angularMove[i] > 0)
                {
                    angularMove[i] = limit;
                }
                else
                {
                    angularMove[i] = -limit;
                }
            }

            linearMove[i] = totalMove - angularMove[i];
            linearChange[i] = linearMove[i] * mNormal;
            body[i]->mPosition += linearChange[i];

            if (abs(angularUnitMove[i]) > 0.0001f)
            {
                angularChange[i] = angularVelocityPerUnitImpulse[i] / angularUnitMove[i] * angularMove[i];
                body[i]->mRotation.ApplyAngularFromVector(angularChange[i]);
            }
            body[i]->mTransformSyncDirty = true;
        }
    }
}

void PhysicsContact::ApplyImpulse(float deltaTime, CoreMath::Vector3 linearVelChange[2],
                                  CoreMath::Vector3 angularVelChange[2])
{

    CoreMath::Vector3 impulse = CalculateImpulse(deltaTime);

    float invMassA = mBodyA->GetInverseMass();

    if (invMassA > 0.0f)
    {

        linearVelChange[0] = mBodyA->GetInverseMass() * impulse;
        mBodyA->mVelocity += linearVelChange[0];
        //        mBodyA->GetInverseMass() * impulse;

        CoreMath::Vector3 r = mPoint - mBodyA->mPosition;
        CoreMath::Vector3 u = r.Cross(impulse);

        angularVelChange[0] += mBodyA->mInertiaInverseWorld.TransformDirection(u);

        mBodyA->mAngularVelocity += angularVelChange[0];
        //       mBodyA->mInertiaInverseWorld.TransformDirection(u);
    }

    if (mBodyB != nullptr)
    {
        float invMassB = mBodyB->GetInverseMass();

        if (invMassB > 0.0f)
        {

            impulse *= -1.0f;
            linearVelChange[1] = mBodyB->GetInverseMass() * (impulse);
            mBodyB->mVelocity += linearVelChange[1];
            //            mBodyB->GetInverseMass() * (impulse);

            CoreMath::Vector3 r = mPoint - mBodyB->mPosition;
            CoreMath::Vector3 u = 1.0f * r.Cross(impulse);

            angularVelChange[1] = mBodyB->mInertiaInverseWorld.TransformDirection(u);
            mBodyB->mAngularVelocity += angularVelChange[1];
            //    mBodyB->mInertiaInverseWorld.TransformDirection(u);
        }
    }
}

void PhysicsContact::matchAwakeState()
{

    if (mBodyB == nullptr)
        return;

    if (mBodyA->mBodyType == EPhysicsBodyType::eStatic || mBodyB->mBodyType == EPhysicsBodyType::eStatic)
    {
        return;
    }

    /*   if (mBodyA->mBodyType == EPhysicsBodyType::eKinematic)
       {
           if (!mBodyB->mIsAwake)
           {
               mBodyB->SetAwake();
           }
       }
       else if (mBodyB->mBodyType == EPhysicsBodyType::eKinematic)
       {
           if (!mBodyA->mIsAwake)
           {
               mBodyA->SetAwake();
           }
       }
       else if (mBodyA->mBodyType == EPhysicsBodyType::eDynamic && mBodyB->mBodyType == EPhysicsBodyType::eDynamic)
       {
       }*/

    // if (mBodyA->mBodyType != EPhysicsBodyType::eDynamic || mBodyB->mBodyType != EPhysicsBodyType::eDynamic)
    //{
    //     return;
    // }
    bool bodyAAwake = mBodyA->mIsAwake;
    bool bodyBAwake = mBodyB->mIsAwake;

    // 하나는 자고있어 다른하나는 꺠어있을떄만  다른한쪽을 깨운다.
    if (bodyAAwake ^ bodyBAwake)
    {
        if (bodyAAwake)
        {

            mBodyB->SetAwake();
        }
        else
        {

            mBodyA->SetAwake();
        }
    }
}

void KinematicContact::ResolvePenetration()
{

    if (mKinematicBody == nullptr)
    {
        mPenetration = 0.0f;
        return;
    }
    mNormalTowardKinematic.Normalize();

    mKinematicBody->mPosition += mNormalTowardKinematic * (mPenetration + 0.001f);

    // normal 방향 ( Kinematic을 향하는방향)
    float normalVelocity = mKinematicBody->mVelocity.Dot(mNormalTowardKinematic);

    // 음수라면 파고드는것이니 제거한다.

    if (normalVelocity < 0.0f)
        mKinematicBody->mVelocity -= mNormalTowardKinematic * normalVelocity;
}