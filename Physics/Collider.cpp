#include "Collider.h"

static preal gSleepEpsilon = 0.3f;

preal GetSleepEpsilon()
{
    return gSleepEpsilon;
}

void SetSleepEpsilon(preal value)
{
    gSleepEpsilon = value;
}

static CoreMath::Matrix4X4 CaculateInertiaeWorld(const CoreMath::Matrix4X4 &inertialInverse,
                                                 const CoreMath::Quaternion &worldRotation)
{

    CoreMath::Matrix4X4 R = CoreMath::Matrix4X4::MakeTransform({0, 0, 0}, worldRotation, {1, 1, 1});
    CoreMath::Matrix4X4 invR = R.GetTransposed();

    return R * inertialInverse * invR;
}

CoreMath::Vector3 PhysicsShape::GetAxis(int index) const
{
    //    return mWorldTransform.GetColumn(index).XYZ();

    static CoreMath::Vector3 axis[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    if (index < 0 || index >= 3)
        return {0, 0, 0};

    return mWorldRotation.RotateVector(axis[index]).GetNormalize();
}

float PhysicsBody::GetInverseMass() const
{
    if (mBodyType != EPhysicsBodyType::eDynamic)
    {
        return 0.0f;
    }

    return 1.0f / mMass;
}

void PhysicsBody::UpdateDerivedData()
{

    UpdateInertiaIWorld();
}

void PhysicsBody::SetAwake(bool awake)
{

    if (awake)
    {
        mIsAwake = true;
        mMotion = gSleepEpsilon * 2.0f;
    }
    else
    {
        mIsAwake = false;
        mVelocity.Clear();
        mAngularVelocity.Clear();
    }
}

void PhysicsBody::UpdateAwakeState()
{
    if (mBodyType != EPhysicsBodyType::eDynamic || !mCanSleep)
        return;

    if (mMotion < gSleepEpsilon)
    {

        // 이미 자고있다면
        if (!mIsAwake)
            return;

        SetAwake(false);
    }
    else
    {
        // 이미 꺠어있다면
        if (mIsAwake)
            return;

        SetAwake(true);
    }
}

void PhysicsBody::UpdateMotion(float deltaTime)
{

    preal currentMotion = mVelocity.LengthSquared() + mAngularVelocity.LengthSquared();

    preal bias = pow(mBaseBias, deltaTime);
    mMotion = bias * mMotion + (1 - bias) * currentMotion;

    if (mMotion > 10 * gSleepEpsilon)
        mMotion = 10 * gSleepEpsilon;
}

void PhysicsBody::UpdateInertiaIWorld()
{
    mInertiaWorld = CaculateInertiaeWorld(mInertia, mRotation);
    mInertiaInverseWorld = CaculateInertiaeWorld(mInertiaInverse, mRotation);
}

void PhysicsBoxShape::UpdateShapeSpecificData()
{

    mHalfExtent = {abs(mWorldScale.X) * mLocalHalfExtent.X, abs(mWorldScale.Y) * mLocalHalfExtent.Y,
                   abs(mWorldScale.Z) * mLocalHalfExtent.Z};

    //
}

CoreMath::Matrix4X4 PhysicsBoxShape::CalculateLocalInertiaTensor()
{

    CoreMath::Vector3 size = mHalfExtent * 2.0f;

    float x2 = size.X * size.X;
    float y2 = size.Y * size.Y;
    float z2 = size.Z * size.Z;

    CoreMath::Matrix4X4 inertia = CoreMath::Matrix4X4::Zero;
    inertia.mat[0][0] = (1.0f / 12.0f) * mMass * (y2 + z2);
    inertia.mat[1][1] = (1.0f / 12.0f) * mMass * (x2 + z2);
    inertia.mat[2][2] = (1.0f / 12.0f) * mMass * (x2 + y2);
    inertia.mat[3][3] = 1.0f;

    mInverseInertiaTensor = inertia.GetInversed();

    return inertia;
}

PhysicsBoxCollisionQueryData PhysicsBoxShape::GetCollisionData() const
{
    PhysicsBoxCollisionQueryData data;
    data.mHalfExtent = mHalfExtent;
    data.mPosition = mWorldPosition;
    data.mRotation = mWorldRotation;
    data.mWorldTransform = mWorldTransform;

    return data;
}

CoreMath::Vector3 PhysicsBoxCollisionQueryData::GetAxis(int index) const
{

    static CoreMath::Vector3 axis[3] = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};

    if (index < 0 || index >= 3)
        return {0, 0, 0};

    return mRotation.RotateVector(axis[index]).GetNormalize();
}

void PhysicsBoxCollisionQueryData::Translate(const CoreMath::Vector3 &offset)
{

    mPosition += offset;
    mWorldTransform = CoreMath::Matrix4X4::MakeTransform(mPosition, mRotation, {1, 1, 1});
}
