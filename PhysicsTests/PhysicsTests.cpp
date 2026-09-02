#include <Physics/PhysicsScene.h>
#include <Physics/PhysicsWorld.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <string_view>

namespace
{
class TestHarness
{
  public:
    void Check(bool condition, std::string_view message)
    {
        if (condition)
        {
            std::cout << "[PASS] " << message << '\n';
            return;
        }

        std::cerr << "[FAIL] " << message << '\n';
        ++mFailureCount;
    }

    int GetExitCode() const
    {
        return mFailureCount == 0 ? 0 : 1;
    }

  private:
    int mFailureCount = 0;
};

bool IsFinite(const CoreMath::Vector3 &value)
{
    return std::isfinite(value.X) && std::isfinite(value.Y) && std::isfinite(value.Z);
}

bool IsFinite(const CoreMath::Quaternion &value)
{
    return std::isfinite(value.X) && std::isfinite(value.Y) && std::isfinite(value.Z) && std::isfinite(value.W);
}

bool IsNear(const CoreMath::Vector3 &lhs, const CoreMath::Vector3 &rhs, float epsilon)
{
    return std::abs(lhs.X - rhs.X) <= epsilon && std::abs(lhs.Y - rhs.Y) <= epsilon &&
           std::abs(lhs.Z - rhs.Z) <= epsilon;
}

bool IsSameOrientation(const CoreMath::Quaternion &lhs, const CoreMath::Quaternion &rhs, float epsilon)
{
    const float lhsLengthSquared = lhs.X * lhs.X + lhs.Y * lhs.Y + lhs.Z * lhs.Z + lhs.W * lhs.W;
    const float rhsLengthSquared = rhs.X * rhs.X + rhs.Y * rhs.Y + rhs.Z * rhs.Z + rhs.W * rhs.W;

    if (lhsLengthSquared <= 0.0f || rhsLengthSquared <= 0.0f)
        return false;

    const float dot = lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z + lhs.W * rhs.W;
    const float normalizedDot = dot / std::sqrt(lhsLengthSquared * rhsLengthSquared);
    return std::abs(std::abs(normalizedDot) - 1.0f) <= epsilon;
}

void RunGravitySimulationTest(TestHarness &harness, PhysicsWorld &physicsWorld)
{
    constexpr float fixedDeltaTime = 1.0f / 60.0f;
    constexpr int simulationStepCount = 120;

    const PhysicsSceneID sceneID = physicsWorld.CreateNewScene();
    PhysicsScene *physicsScene = physicsWorld.GetScene(sceneID);

    if (physicsScene == nullptr)
    {
        harness.Check(false, "Gravity simulation scene creation");
        return;
    }

    PhysicsBodyDesc bodyDesc{};
    bodyDesc.mBodyType = EPhysicsBodyType::eDynamic;
    bodyDesc.mPosition = {0.0f, 10.0f, 0.0f};
    bodyDesc.mRotation = {0.0f, 0.0f, 0.0f, 1.0f};
    bodyDesc.mMass = 1.0f;
    bodyDesc.mGravity = true;

    const PhysicsBodyHandle bodyHandle = physicsScene->CreatePhysicsBody(bodyDesc);
    if (bodyHandle == PhysicsBodyHandleInValid)
    {
        harness.Check(false, "Dynamic body creation for gravity simulation");
        return;
    }

    CoreMath::Vector3 previousPosition = bodyDesc.mPosition;
    CoreMath::Vector3 finalPosition = bodyDesc.mPosition;
    CoreMath::Quaternion finalRotation{0.0f, 0.0f, 0.0f, 1.0f};
    CoreMath::Vector3 finalVelocity{};
    bool simulationValuesAreFinite = true;

    for (int step = 0; step < simulationStepCount; ++step)
    {
        previousPosition = finalPosition;
        physicsScene->Update(fixedDeltaTime);

        if (!physicsScene->GetBodyTransform(bodyHandle, finalPosition, finalRotation))
        {
            simulationValuesAreFinite = false;
            break;
        }

        finalVelocity = (finalPosition - previousPosition) / fixedDeltaTime;
        simulationValuesAreFinite = simulationValuesAreFinite && IsFinite(finalPosition) && IsFinite(finalVelocity);
    }

    harness.Check(finalPosition.Y < bodyDesc.mPosition.Y, "Gravity lowers the dynamic body's Y position");
    harness.Check(finalVelocity.Y < 0.0f, "Gravity produces a negative final Y velocity");
    harness.Check(simulationValuesAreFinite, "Gravity simulation position and velocity remain finite");
}

void RunStaticBodyInvarianceTest(TestHarness &harness, PhysicsWorld &physicsWorld)
{
    constexpr float fixedDeltaTime = 1.0f / 60.0f;
    constexpr int simulationStepCount = 120;
    constexpr float transformEpsilon = 0.00001f;

    const PhysicsSceneID sceneID = physicsWorld.CreateNewScene();
    PhysicsScene *physicsScene = physicsWorld.GetScene(sceneID);

    if (physicsScene == nullptr)
    {
        harness.Check(false, "Static body simulation scene creation");
        return;
    }

    PhysicsBodyDesc bodyDesc{};
    bodyDesc.mBodyType = EPhysicsBodyType::eStatic;
    bodyDesc.mPosition = {3.0f, 7.0f, -2.0f};
    bodyDesc.mRotation = CoreMath::Quaternion::MakeFromEuler({0.2f, -0.4f, 0.1f});
    bodyDesc.mMass = 1.0f;
    bodyDesc.mGravity = true;

    const PhysicsBodyHandle bodyHandle = physicsScene->CreatePhysicsBody(bodyDesc);
    if (bodyHandle == PhysicsBodyHandleInValid)
    {
        harness.Check(false, "Static body creation for invariance simulation");
        return;
    }

    CoreMath::Vector3 initialPosition;
    CoreMath::Quaternion initialOrientation;
    if (!physicsScene->GetBodyTransform(bodyHandle, initialPosition, initialOrientation))
    {
        harness.Check(false, "Initial static body transform query");
        return;
    }

    CoreMath::Vector3 finalPosition = initialPosition;
    CoreMath::Quaternion finalOrientation = initialOrientation;
    bool valuesAreFinite = IsFinite(initialPosition) && IsFinite(initialOrientation);
    bool transformRemainedUnchanged = true;

    for (int step = 0; step < simulationStepCount; ++step)
    {
        physicsScene->Update(fixedDeltaTime);

        if (!physicsScene->GetBodyTransform(bodyHandle, finalPosition, finalOrientation))
        {
            valuesAreFinite = false;
            transformRemainedUnchanged = false;
            break;
        }

        valuesAreFinite = valuesAreFinite && IsFinite(finalPosition) && IsFinite(finalOrientation);
        transformRemainedUnchanged =
            transformRemainedUnchanged && IsNear(finalPosition, initialPosition, transformEpsilon) &&
            IsSameOrientation(finalOrientation, initialOrientation, transformEpsilon);
    }

    harness.Check(IsNear(finalPosition, initialPosition, transformEpsilon),
                  "Static body position is unchanged after simulation");
    harness.Check(IsSameOrientation(finalOrientation, initialOrientation, transformEpsilon),
                  "Static body orientation is unchanged after simulation");
    harness.Check(valuesAreFinite, "Static body position and orientation remain finite");
    harness.Check(transformRemainedUnchanged, "Static body remains unchanged throughout all simulation steps");
}

void RunBoxBoxCollisionSimulationTest(TestHarness &harness, PhysicsWorld &physicsWorld)
{
    constexpr float fixedDeltaTime = 1.0f / 60.0f;
    constexpr int simulationStepCount = 120;
    constexpr float gravityY = -9.8f;
    constexpr float transformEpsilon = 0.00001f;
    constexpr float penetrationTolerance = 0.25f;
    constexpr float trajectoryDifferenceThreshold = 0.25f;
    constexpr float staticBoxCenterY = 0.0f;
    constexpr float dynamicBoxInitialY = 5.0f;
    constexpr float staticBoxHalfExtentY = 0.5f;
    constexpr float dynamicBoxHalfExtentY = 0.5f;
    constexpr float expectedContactCenterY =
        staticBoxCenterY + staticBoxHalfExtentY + dynamicBoxHalfExtentY;
    constexpr float minimumAllowedDynamicCenterY = expectedContactCenterY - penetrationTolerance;

    const PhysicsSceneID sceneID = physicsWorld.CreateNewScene();
    PhysicsScene *physicsScene = physicsWorld.GetScene(sceneID);

    if (physicsScene == nullptr)
    {
        harness.Check(false, "Box-Box collision simulation scene creation");
        return;
    }

    PhysicsBodyDesc staticBodyDesc{};
    staticBodyDesc.mBodyType = EPhysicsBodyType::eStatic;
    staticBodyDesc.mPosition = {0.0f, staticBoxCenterY, 0.0f};
    staticBodyDesc.mRotation = {0.0f, 0.0f, 0.0f, 1.0f};
    staticBodyDesc.mMass = 1.0f;
    staticBodyDesc.mGravity = true;

    const PhysicsBodyHandle staticBodyHandle = physicsScene->CreatePhysicsBody(staticBodyDesc);

    PhysicsShapeDesc staticShapeDesc{};
    staticShapeDesc.mBodyHandle = staticBodyHandle;
    staticShapeDesc.mShapeType = EPhysicsCollisionShapeType::eBox;
    staticShapeDesc.mLocalRotation = {0.0f, 0.0f, 0.0f, 1.0f};
    staticShapeDesc.mWorldPosition = staticBodyDesc.mPosition;
    staticShapeDesc.mWorldRotation = staticBodyDesc.mRotation;
    staticShapeDesc.mBoxData.mLocalHalfExtent = {0.5f, staticBoxHalfExtentY, 0.5f};
    const PhysicsShapeHandle staticShapeHandle = physicsScene->CreatePhysicsShape(staticShapeDesc);

    PhysicsBodyDesc dynamicBodyDesc{};
    dynamicBodyDesc.mBodyType = EPhysicsBodyType::eDynamic;
    dynamicBodyDesc.mPosition = {0.0f, dynamicBoxInitialY, 0.0f};
    dynamicBodyDesc.mRotation = {0.0f, 0.0f, 0.0f, 1.0f};
    dynamicBodyDesc.mVelocity = {0.0f, 0.0f, 0.0f};
    dynamicBodyDesc.mMass = 1.0f;
    dynamicBodyDesc.mGravity = true;

    const PhysicsBodyHandle dynamicBodyHandle = physicsScene->CreatePhysicsBody(dynamicBodyDesc);

    PhysicsShapeDesc dynamicShapeDesc{};
    dynamicShapeDesc.mBodyHandle = dynamicBodyHandle;
    dynamicShapeDesc.mShapeType = EPhysicsCollisionShapeType::eBox;
    dynamicShapeDesc.mLocalRotation = {0.0f, 0.0f, 0.0f, 1.0f};
    dynamicShapeDesc.mWorldPosition = dynamicBodyDesc.mPosition;
    dynamicShapeDesc.mWorldRotation = dynamicBodyDesc.mRotation;
    dynamicShapeDesc.mBoxData.mLocalHalfExtent = {0.5f, dynamicBoxHalfExtentY, 0.5f};
    const PhysicsShapeHandle dynamicShapeHandle = physicsScene->CreatePhysicsShape(dynamicShapeDesc);

    if (staticBodyHandle == PhysicsBodyHandleInValid || staticShapeHandle == PhysicsShapeHandleInValid ||
        dynamicBodyHandle == PhysicsBodyHandleInValid || dynamicShapeHandle == PhysicsShapeHandleInValid)
    {
        harness.Check(false, "Static and dynamic Box-Box test fixture creation");
        return;
    }

    CoreMath::Vector3 initialStaticPosition;
    CoreMath::Quaternion initialStaticOrientation;
    CoreMath::Vector3 dynamicPosition = dynamicBodyDesc.mPosition;
    CoreMath::Quaternion dynamicOrientation = dynamicBodyDesc.mRotation;

    if (!physicsScene->GetBodyTransform(staticBodyHandle, initialStaticPosition, initialStaticOrientation))
    {
        harness.Check(false, "Initial static Box transform query");
        return;
    }

    float minimumDynamicY = dynamicPosition.Y;
    float freeFallPositionY = dynamicBodyDesc.mPosition.Y;
    float freeFallVelocityY = dynamicBodyDesc.mVelocity.Y;
    bool allTransformsAreFinite = true;
    bool staticTransformRemainedUnchanged = true;
    bool dynamicBoxPassedThroughStaticBox = false;
    bool collisionResponseObserved = false;

    for (int step = 0; step < simulationStepCount; ++step)
    {
        freeFallVelocityY += gravityY * fixedDeltaTime;
        freeFallPositionY += freeFallVelocityY * fixedDeltaTime;

        physicsScene->Update(fixedDeltaTime);

        CoreMath::Vector3 staticPosition;
        CoreMath::Quaternion staticOrientation;
        const bool dynamicTransformRead =
            physicsScene->GetBodyTransform(dynamicBodyHandle, dynamicPosition, dynamicOrientation);
        const bool staticTransformRead =
            physicsScene->GetBodyTransform(staticBodyHandle, staticPosition, staticOrientation);

        if (!dynamicTransformRead || !staticTransformRead)
        {
            allTransformsAreFinite = false;
            staticTransformRemainedUnchanged = false;
            break;
        }

        allTransformsAreFinite = allTransformsAreFinite && IsFinite(dynamicPosition) && IsFinite(dynamicOrientation) &&
                                 IsFinite(staticPosition) && IsFinite(staticOrientation);
        staticTransformRemainedUnchanged =
            staticTransformRemainedUnchanged && IsNear(staticPosition, initialStaticPosition, transformEpsilon) &&
            IsSameOrientation(staticOrientation, initialStaticOrientation, transformEpsilon);

        minimumDynamicY = std::min(minimumDynamicY, dynamicPosition.Y);
        dynamicBoxPassedThroughStaticBox =
            dynamicBoxPassedThroughStaticBox || dynamicPosition.Y < minimumAllowedDynamicCenterY;

        if (freeFallPositionY < expectedContactCenterY &&
            dynamicPosition.Y - freeFallPositionY > trajectoryDifferenceThreshold)
        {
            collisionResponseObserved = true;
        }
    }

    std::cout << "[INFO] Box-Box minimum dynamic Y: " << minimumDynamicY
              << ", final free-fall reference Y: " << freeFallPositionY << '\n';

    harness.Check(minimumDynamicY < dynamicBoxInitialY - transformEpsilon,
                  "Dynamic Box falls before Box-Box collision response");
    harness.Check(!dynamicBoxPassedThroughStaticBox, "Dynamic Box does not pass through the Static Box");
    harness.Check(collisionResponseObserved, "Box-Box collision changes the free-fall trajectory");
    harness.Check(allTransformsAreFinite, "Box-Box simulation transforms remain finite");
    harness.Check(staticTransformRemainedUnchanged, "Static Box remains unchanged during Box-Box collision");
}
} // namespace

int main()
{
    TestHarness harness;

    PhysicsWorld physicsWorld;
    const PhysicsSceneID sceneID = physicsWorld.CreateNewScene();
    PhysicsScene *physicsScene = physicsWorld.GetScene(sceneID);

    harness.Check(sceneID != PhysicsSceneIDInValid, "Physics scene creation through the public API");
    harness.Check(physicsScene != nullptr, "Physics scene lookup through the public API");

    RunGravitySimulationTest(harness, physicsWorld);
    RunStaticBodyInvarianceTest(harness, physicsWorld);
    RunBoxBoxCollisionSimulationTest(harness, physicsWorld);

    std::cout << "PhysicsTests completed.\n";
    return harness.GetExitCode();
}
