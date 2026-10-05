#include "FBXImporter.h"
#include "FBXAxisPolicy.h"
#include <CoreAsset/AnimationTypes.h>
#include <CoreAsset/IntermediateAsset.h>
#include <CoreBase/CoreAssert.h>
#include <Logger/Logger.h>
#include <Utility/Utility.h>
#include <fbxsdk.h>
#include <limits>
#include <map>
#include <sstream>
#include <stack>

namespace Import
{

static void Copy(const fbxsdk::FbxPropertyT<FbxDouble3> &source, CoreMath::Vector3 &dest)
{

    const FbxDouble3 &fbxD3 = source.Get();
    dest.X = fbxD3[0];
    dest.Y = fbxD3[1];
    dest.Z = fbxD3[2];
}

static void Copy(const fbxsdk::FbxPropertyT<FbxDouble> &source, float &dest)
{
    dest = source.Get();
}

static void Copy(const fbxsdk::FbxDouble4 &source, CoreMath::Vector3 &dest)
{
    dest.X = source.mData[0];
    dest.Y = source.mData[1];
    dest.Z = source.mData[2];
}

static void Copy(const fbxsdk::FbxDouble4 &source, CoreMath::Vector4 &dest)
{
    dest.X = source.mData[0];
    dest.Y = source.mData[1];
    dest.Z = source.mData[2];
    dest.W = source.mData[3];
}

static void Copy(const fbxsdk::FbxDouble4 &source, CoreMath::Quaternion &dest)
{
    dest.X = source.mData[0];
    dest.Y = source.mData[1];
    dest.Z = source.mData[2];
    dest.W = source.mData[3];
}

static void Copy(const fbxsdk::FbxVector2 &source, CoreMath::Vector2 &dest)
{
    dest.X = source.mData[0];
    dest.Y = source.mData[1];
}

static void Copy(const fbxsdk::FbxAMatrix &source, CoreMath::Matrix4X4 &dest)
{

    CoreMath::Vector4 col[4];

    for (int c = 0; c < 4; ++c)
    {
        Copy(source.GetRow(c), col[c]);
    }

    dest = CoreMath::Matrix4X4(col[0], col[1], col[2], col[3]);
}

static std::string MakeJointStableKeySegment(const std::string &name, uint32_t sameNameOrdinal)
{
    // 길이 접두부로 이름 속 구분자를 구별하고, 같은 부모 아래 중복 이름은 출현 순서로 구별한다.
    return std::to_string(name.size()) + ":" + name + ":" + std::to_string(sameNameOrdinal);
}

static void TryCopyVector3Property(fbxsdk::FbxObject *fbxObject, const char *propertyName,
                                   CoreMath::Vector3 &outVector3)
{
    FbxProperty fbxProperty = fbxObject->FindProperty(propertyName);
    if (fbxProperty.IsValid() == false)
        return;

    fbxsdk::FbxDouble3 srcVector3 = fbxProperty.Get<fbxsdk::FbxDouble3>();

    outVector3.X = srcVector3.mData[0];
    outVector3.Y = srcVector3.mData[1];
    outVector3.Z = srcVector3.mData[2];
}

static void TryCopyDoubleProperty(fbxsdk::FbxObject *fbxObject, const char *propertyName, float &oValue)
{
    FbxProperty fbxProperty = fbxObject->FindProperty(propertyName);

    if (fbxProperty.IsValid() == false)
        return;

    fbxsdk::FbxDouble srcValue = fbxProperty.Get<fbxsdk::FbxDouble>();

    oValue = srcValue;
}

FBXLoadResult::FBXLoadResult() {}

FBXLoadResult::~FBXLoadResult() {}

namespace
{
struct FBXDestroyDeleter
{

    template <typename T> void operator()(T *object) const
    {
        if (object)
            object->Destroy();
    }
};

template <typename T> using FbxUniquePtr = std::unique_ptr<T, FBXDestroyDeleter>;
} // namespace

FBXImporter *FBXImporter::GetInstance()
{
    static FBXImporter instance;
    return &instance;
}

FBXImporter::FBXImporter() {}

FBXImporter::~FBXImporter()
{

    if (mFbxManager)
    {
        mFbxManager->Destroy();
    }
}

void FBXImporter::Initialize()
{

    mFbxManager = fbxsdk::FbxManager::Create();

    fbxsdk::FbxIOSettings *ios = fbxsdk::FbxIOSettings::Create(mFbxManager, IOSROOT);
    mFbxManager->SetIOSettings(ios);
}

CoreAsset::ImportPackage FBXImporter::Import(const std::filesystem::path &filePath,
                                             CoreAsset::AssetImporterManager *importerManager,
                                             const CoreAsset::ImportExecutionContext &executionContext) const
{
    // 일단 임시로
    return Load(filePath);
}

CoreAsset::ImportPackage FBXImporter::Load(const std::filesystem::path &filePath) const
{
    CoreAsset::ImportPackage importPackage;

    if (filePath.empty())
    {
        importPackage.mFailReason = "파일 경로가 올바르지못합니다.";
        return importPackage;
    }

    FbxUniquePtr<fbxsdk::FbxImporter> lImporter(fbxsdk::FbxImporter::Create(mFbxManager, ""));
    if (!lImporter->Initialize(filePath.string().c_str(), -1, mFbxManager->GetIOSettings()))
    {

        fbxsdk::FbxString error = lImporter->GetStatus().GetErrorString();
        importPackage.mFailReason = error;
        return importPackage;
    }

    FbxUniquePtr<fbxsdk::FbxScene> lScene(fbxsdk::FbxScene::Create(mFbxManager, "scene"));
    bool bImportResult = lImporter->Import(lScene.get());
    if (bImportResult == false)
    {
        fbxsdk::FbxString error = lImporter->GetStatus().GetErrorString();
        importPackage.mFailReason = error;
        return importPackage;
    }

    FBXImportContext importContext;
    // importContext는 이 Load 호출 동안만 scene을 참조하며 소유권은 lScene에 남긴다.
    importContext.mFbxScene = lScene.get();
    importContext.mFbxFileName = filePath.filename().string();
    // CoreUtility::Utility::GetFileNameFromPath(filePath);
    // 지원 여부가 불명확한 축계를 identity/zero matrix로 계속 처리하지 않고 traversal과 asset 생성 전에 종료한다.
    if (!ConvertAxisSystem(importContext, lScene.get(), importPackage.mFailReason))
    {
        importPackage.mFailed = true;
        return importPackage;
    }
    // 공식 길이 단위 cm로 scene을 정규화한 뒤 joint 계약을 검사하여 저장될 translation/scale 기준과 맞춘다.
    ConvertUnitSystem(lScene.get());
    if (!ValidateAnimationJointTransforms(lScene.get(), importPackage.mFailReason))
    {
        importPackage.mFailed = true;
        return importPackage;
    }
    Triangulate(lScene.get());

    TraverseScene(lScene.get(), importContext);

    ExtractSkeletonData(importContext);

    ExtractAnimationData(importContext);

    ExtractData(importContext);

    if (!BuildIntermediateAssets(importContext, importPackage))
    {
        CoreAsset::ImportPackage failedPackage;
        failedPackage.mFailed = true;
        failedPackage.mFailReason = std::move(importPackage.mFailReason);
        return failedPackage;
    }

    SetImportPackageOptions(importContext, importPackage);

    return importPackage;
}

bool FBXImporter::ConvertAxisSystem(FBXImportContext &importContext, fbxsdk::FbxScene *scene,
                                    std::string &failureReason) const
{
    const fbxsdk::FbxAxisSystem axisSystem = scene->GetGlobalSettings().GetAxisSystem();
    int upSign = 0;
    const fbxsdk::FbxAxisSystem::EUpVector sourceUpAxis = axisSystem.GetUpVector(upSign);

    // SDK enum을 독립 정책 타입으로 축소한다. 지원하지 않는 X-Up은 기본값으로 남아 아래 정책에서 명시적으로 거부된다.
    EFBXUpAxis upAxis = EFBXUpAxis::eX;
    if (sourceUpAxis == fbxsdk::FbxAxisSystem::eYAxis)
        upAxis = EFBXUpAxis::eY;
    else if (sourceUpAxis == fbxsdk::FbxAxisSystem::eZAxis)
        upAxis = EFBXUpAxis::eZ;

    const EFBXHandedness handedness = axisSystem.GetCoorSystem() == fbxsdk::FbxAxisSystem::eRightHanded
                                          ? EFBXHandedness::eRightHanded
                                          : EFBXHandedness::eLeftHanded;
    const FBXAxisConversion conversion = ResolveFBXAxisConversion(handedness, upAxis, upSign);
    if (!conversion.mSupported)
    {
        failureReason = conversion.mFailureReason;
        return false;
    }

    importContext.mBakeAxisTransformMatrix = conversion.mBakeMatrix;
    failureReason.clear();
    return true;
}

void FBXImporter::ConvertUnitSystem(fbxsdk::FbxScene *scene) const
{
    fbxsdk::FbxSystemUnit::cm.ConvertScene(scene);
}

void FBXImporter::Triangulate(fbxsdk::FbxScene *scene) const
{

    FbxGeometryConverter converter(mFbxManager);
    converter.Triangulate(scene, true);
}

bool FBXImporter::ValidateAnimationJointTransforms(fbxsdk::FbxScene *scene, std::string &failureReason) const
{
    fbxsdk::FbxNode *rootNode = scene->GetRootNode();
    if (rootNode == nullptr)
    {
        failureReason = "FBX scene root가 없습니다.";
        return false;
    }
    return ValidateAnimationJointNode(rootNode, failureReason);
}

bool FBXImporter::ValidateAnimationJointNode(fbxsdk::FbxNode *node, std::string &failureReason) const
{
    fbxsdk::FbxNodeAttribute *attribute = node->GetNodeAttribute();
    if (attribute != nullptr && attribute->GetAttributeType() == fbxsdk::FbxNodeAttribute::eSkeleton)
    {
        // V1 제한은 mesh node 전체가 아니라 Skeleton joint transform 경계에 적용한다.
        const fbxsdk::FbxAMatrix localMatrix = node->EvaluateLocalTransform();
        CoreAsset::AnimationLocalTransform localTransform;
        Copy(localMatrix.GetT(), localTransform.mPosition);
        Copy(localMatrix.GetQ(), localTransform.mRotation);
        Copy(localMatrix.GetS(), localTransform.mScale);

        // 분해된 T/Q/S만으로는 shear를 검출할 수 없으므로 원본 local matrix도 validator에 함께 전달한다.
        CoreMath::Matrix4X4 sourceMatrix;
        Copy(localMatrix, sourceMatrix);
        const CoreAsset::AnimationTransformValidationResult validation =
            CoreAsset::ValidateAnimationJointTransform(localTransform, &sourceMatrix);
        if (!validation.mValid)
        {
            std::ostringstream stream;
            stream << "FBX joint transform이 V1 계약을 위반합니다. node=" << node->GetName() << ", scale=("
                   << localTransform.mScale.X << ", " << localTransform.mScale.Y << ", " << localTransform.mScale.Z
                   << "), error=" << static_cast<int>(validation.mError);
            failureReason = stream.str();
            return false;
        }
    }

    for (int childIndex = 0; childIndex < node->GetChildCount(); ++childIndex)
    {
        if (!ValidateAnimationJointNode(node->GetChild(childIndex), failureReason))
            return false;
    }
    return true;
}

void FBXImporter::TraverseScene(fbxsdk::FbxScene *scene, FBXImportContext &importContext) const
{
    fbxsdk::FbxNode *rootNode = scene->GetRootNode();
    TraverseNode(rootNode, importContext);
}

void FBXImporter::TraverseNode(fbxsdk::FbxNode *node, FBXImportContext &importContext) const
{
    //  importContext.mAllNodes.push_back(node);

    std::stack<fbxsdk::FbxNode *> nodeStack;

    nodeStack.push(node);

    while (nodeStack.empty() == false)
    {
        auto node = nodeStack.top();
        nodeStack.pop();

        fbxsdk::FbxNodeAttribute *nodeAttribute = node->GetNodeAttribute();

        if (nodeAttribute)
        {
            switch (nodeAttribute->GetAttributeType())
            {
            case fbxsdk::FbxNodeAttribute::EType::eMesh:
            {
                fbxsdk::FbxMesh *fbxMesh = static_cast<fbxsdk::FbxMesh *>(nodeAttribute);

                importContext.mAllMeshNodes.push_back(fbxMesh);
                // IsSkinning fbx mesh
                if (IsSkinningFbxMesh(fbxMesh))
                {

                    importContext.mSkinnedMeshNodeList.push_back(fbxMesh);
                }
                else
                {

                    importContext.mStaticMeshToNodes[fbxMesh].push_back(node);
                }
            }
            break;
            case fbxsdk::FbxNodeAttribute::EType::eSkeleton:
            {
                if (importContext.mSkeleton.mRootSkeletonNode == nullptr)
                    importContext.mSkeleton.mRootSkeletonNode = node;

                //   fbxsdk::FbxSkeleton *fbxSkeleton = static_cast<fbxsdk::FbxSkeleton *>(nodeAttribute);
            }
            break;
            }
        }

        for (size_t i = 0; i < node->GetChildCount(); ++i)
        {
            nodeStack.push(node->GetChild(i));
        }
    }

    // if root skeleton node 가 exsit
    //  -> 그 루트 스켈레톤 노드의 모든 자손노드에대한 계층구조를  수집
    //  skeleton 이든 아닌듯 다 수집그냥

    if (importContext.mSkeleton.mRootSkeletonNode != nullptr)
    {
        std::stack<fbxsdk::FbxNode *> nodeStack;
        std::stack<int32_t> skeletonIndexStack;

        importContext.mSkeleton.mJointNodeIndexTable[importContext.mSkeleton.mRootSkeletonNode] = 0;
        FBXImportJoint rootJoint;
        rootJoint.mName = importContext.mSkeleton.mRootSkeletonNode->GetName();
        rootJoint.mStableKey = MakeJointStableKeySegment(rootJoint.mName, 0);
        rootJoint.mParentJointIndex = -1;
        importContext.mSkeleton.mJoints.push_back(rootJoint);

        nodeStack.push(importContext.mSkeleton.mRootSkeletonNode);

        while (nodeStack.empty() == false)
        {
            auto node = nodeStack.top();
            nodeStack.pop();

            int32_t parentNodeIndex = importContext.mSkeleton.mJointNodeIndexTable[node];

            std::unordered_map<std::string, uint32_t> siblingNameCounts;

            for (int i = 0; i < node->GetChildCount(); ++i)
            {
                auto childNode = node->GetChild(i);

                FBXImportJoint joint;
                joint.mName = childNode->GetName();
                const uint32_t sameNameOrdinal = siblingNameCounts[joint.mName]++;
                joint.mStableKey = importContext.mSkeleton.mJoints[parentNodeIndex].mStableKey + "/" +
                                   MakeJointStableKeySegment(joint.mName, sameNameOrdinal);
                joint.mParentJointIndex = parentNodeIndex;

                importContext.mSkeleton.mJoints.push_back(joint);
                importContext.mSkeleton.mJointNodeIndexTable[childNode] = importContext.mSkeleton.mJoints.size() - 1;

                importContext.mSkeleton.mJoints[parentNodeIndex].mChildJointIndexList.push_back(
                    importContext.mSkeleton.mJointNodeIndexTable[childNode]);

                nodeStack.push(childNode);
            }
        }
    }
}

void FBXImporter::ExtractSkeletonData(FBXImportContext &importContext) const
{
    fbxsdk::FbxAMatrix bakeAxisTransformMatFbx;
    for (int i = 0; i < 4; ++i)
    {
        bakeAxisTransformMatFbx.mData[i] = {
            importContext.mBakeAxisTransformMatrix.mat[i].X, importContext.mBakeAxisTransformMatrix.mat[i].Y,
            importContext.mBakeAxisTransformMatrix.mat[i].Z, importContext.mBakeAxisTransformMatrix.mat[i].W};
    }

    // reference pose
    for (auto &e : importContext.mSkeleton.mJointNodeIndexTable)
    {
        auto skeletonNode = e.first;

        FBXImportJoint &joint = importContext.mSkeleton.mJoints[e.second];

        fbxsdk::FbxAMatrix referenceMatrix =
            bakeAxisTransformMatFbx * skeletonNode->EvaluateLocalTransform() * bakeAxisTransformMatFbx.Inverse();

        Copy(referenceMatrix, joint.mReferencePose);
    }
}

void FBXImporter::ExtractAnimationData(FBXImportContext &importContext) const
{
    // FBX scene의 animation stack을 열거하고, 각 stack을 별도 clip으로 추출할 대상을 결정한다.
    // Skeleton이 없거나 유효한 재생 구간이 없는 경우의 처리도 여기서 결정한다.
    // 추출 결과를 보관할 임시 데이터와 최종 intermediate asset 생성은 구현 단계에서 연결한다.

    int animStackCount = importContext.mFbxScene->GetSrcObjectCount<FbxAnimStack>();

    for (int stackIndex = 0; stackIndex < animStackCount; ++stackIndex)
    {
        FbxAnimStack *animStack = importContext.mFbxScene->GetSrcObject<FbxAnimStack>(stackIndex);

        ExtractAnimationClipData(importContext, animStack, stackIndex);

        /// 끝?
    }
}

void FBXImporter::ExtractAnimationClipData(FBXImportContext &importContext, fbxsdk::FbxAnimStack *animStack,
                                           int stackIndex) const
{
    // stack의 시간 범위를 seconds 기반 duration과 uniform sample 시각으로 변환한다.
    // 수집된 Skeleton joint와 FBX node의 대응을 사용해 각 시각의 parent-local transform을 평가한다.
    // 메시·reference pose와 같은 좌표계로 변환한 뒤 V1 TRS 제한을 검사하고 joint key별 track을 구성한다.
    // 누락된 channel의 reference pose fallback 및 [0, duration) loop 경계 정책을 반영한다.

    // 0번 layer만 가정
    const int layerCount = animStack->GetMemberCount<fbxsdk::FbxAnimLayer>();
    if (layerCount != 1 || importContext.mSkeleton.mJoints.empty())
        return;

    importContext.mFbxScene->SetCurrentAnimationStack(animStack);

    fbxsdk::FbxTimeSpan timeSpan = animStack->GetLocalTimeSpan();

    fbxsdk::FbxTime duration = timeSpan.GetDuration();

    long long frameCount = duration.GetFrameCount(fbxsdk::FbxTime::eFrames30);
    if (frameCount < 0 || frameCount >= static_cast<long long>(std::numeric_limits<uint32_t>::max()))
        return;

    // unordered_map의 순서는 joint 배열 순서가 아니다. Skeleton joint index를 그대로 track slot으로 사용한다.
    auto clip = std::make_unique<CoreAsset::IntermediateAnimationClip>();
    clip->mAssetName = animStack->GetName();
    clip->mSampleRateNumerator = 30;
    clip->mSampleRateDenominator = 1;
    clip->mSampleCount = static_cast<uint32_t>(frameCount + 1);
    // 끝 샘플을 포함하는 uniform 30fps 계약에 맞춰 FBX 구간을 frame 경계의 길이로 저장한다.
    clip->mDurationSeconds = static_cast<float>(frameCount) / 30.0f;
    clip->mTracks.resize(importContext.mSkeleton.mJoints.size());
    for (const auto &entry : importContext.mSkeleton.mJointNodeIndexTable)
    {
        const int32_t jointIndex = entry.second;
        clip->mTracks[jointIndex].mJointKey = importContext.mSkeleton.mJoints[jointIndex].mStableKey;
    }

    fbxsdk::FbxAMatrix bakeAxisTransformMatFbx;
    for (int i = 0; i < 4; ++i)
    {
        bakeAxisTransformMatFbx.mData[i] = {
            importContext.mBakeAxisTransformMatrix.mat[i].X, importContext.mBakeAxisTransformMatrix.mat[i].Y,
            importContext.mBakeAxisTransformMatrix.mat[i].Z, importContext.mBakeAxisTransformMatrix.mat[i].W};
    }
    const fbxsdk::FbxAMatrix bakeAxisInverse = bakeAxisTransformMatFbx.Inverse();

    for (long long i = 0; i <= frameCount; ++i)
    {
        fbxsdk::FbxTime sampleOffset;
        sampleOffset.SetSecondDouble(static_cast<double>(i) / 30.0);

        for (auto &e : importContext.mSkeleton.mJointNodeIndexTable)
        {
            const int32_t jointIndex = e.second;
            auto fbxSkeletonNode = e.first;

            fbxsdk::FbxTime currTime = timeSpan.GetStart() + sampleOffset;
            // reference pose와 같은 엔진 축계의 parent-local 행렬로 변환한 뒤 TRS를 추출한다.
            fbxsdk::FbxAMatrix animLocalMatrix =
                bakeAxisTransformMatFbx * fbxSkeletonNode->EvaluateLocalTransform(currTime) * bakeAxisInverse;

            const fbxsdk::FbxVector4 fbxTranslation = animLocalMatrix.GetT();

            const fbxsdk::FbxQuaternion fbxRotation = animLocalMatrix.GetQ();

            const fbxsdk::FbxVector4 fbxScale = animLocalMatrix.GetS();

            CoreMath::Vector3 scale;
            CoreMath::Quaternion quat;
            CoreMath::Vector3 translation;

            Copy(fbxScale, scale);
            Copy(fbxRotation, quat);
            Copy(fbxTranslation, translation);

            clip->mTracks[jointIndex].mScales.push_back(scale);
            clip->mTracks[jointIndex].mRotations.push_back(quat);
            clip->mTracks[jointIndex].mPositions.push_back(translation);
        }
    }

    CoreAsset::ImportedIntermediateAsset importedClip;
    importedClip.mKey = importContext.mFbxFileName + "::AnimationClip::" + std::to_string(stackIndex);
    importedClip.mImportKey = "AnimationClip::" + std::to_string(stackIndex);
    importedClip.mIntermediateAsset = std::move(clip);
    importContext.mAnimationClipAssets.push_back(std::move(importedClip));
}

void FBXImporter::ExtractData(FBXImportContext &importContext) const
{

    std::unordered_map<fbxsdk::FbxMesh *, std::vector<fbxsdk::FbxNode *>> &meshToNodes =
        importContext.mStaticMeshToNodes;
    std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> fbxMaterialKeyTable;

    // umap< mesh, globalIndexOffset int>
    std::vector<fbxsdk::FbxSurfaceMaterial *> &globalFbxMaterialList = importContext.mGlobalFbxMaterialList;
    std::vector<fbxsdk::FbxTexture *> fbxTextureList;
    //< fbx surface material - id > table 구축
    // 머터리얼들 수집
    for (const auto &e : meshToNodes)
    {
        const std::vector<fbxsdk::FbxNode *> &fbxNodes = e.second;

        for (auto node : fbxNodes)
        {
            int materialCount = node->GetMaterialCount();

            if (materialCount == 0)
            {
                auto it = fbxMaterialKeyTable.find(nullptr);
                if (it == fbxMaterialKeyTable.end())
                {

                    globalFbxMaterialList.push_back(nullptr);
                    fbxMaterialKeyTable[nullptr].mGlobalIndex = globalFbxMaterialList.size() - 1;
                    fbxMaterialKeyTable[nullptr].mVaild = false;
                    fbxMaterialKeyTable[nullptr].mKey = "";
                }
            }
            else
            {

                for (int i = 0; i < materialCount; ++i)
                {
                    fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial = node->GetMaterial(i);

                    auto it = fbxMaterialKeyTable.find(fbxSurfaceMaterial);
                    if (it == fbxMaterialKeyTable.end())
                    {

                        globalFbxMaterialList.push_back(fbxSurfaceMaterial);
                        int index = globalFbxMaterialList.size() - 1;
                        fbxMaterialKeyTable[fbxSurfaceMaterial].mKey =
                            fbxSurfaceMaterial != nullptr ? fbxSurfaceMaterial->GetName() : "";
                        fbxMaterialKeyTable[fbxSurfaceMaterial].mVaild = fbxSurfaceMaterial != nullptr;
                        fbxMaterialKeyTable[fbxSurfaceMaterial].mGlobalIndex = index;
                    }
                }
            }
        }
    }

    // 정적 메시와 별개로 저장되는 스키닝 메시의 재질도 같은 에셋 의존성 테이블에 등록한다.
    for (fbxsdk::FbxMesh *fbxMesh : importContext.mSkinnedMeshNodeList)
    {
        fbxsdk::FbxNode *node = fbxMesh->GetNode();
        if (node->GetMaterialCount() == 0 && fbxMaterialKeyTable.find(nullptr) == fbxMaterialKeyTable.end())
        {
            globalFbxMaterialList.push_back(nullptr);
            FBXMaterialKeyContext &materialContext = fbxMaterialKeyTable[nullptr];
            materialContext.mGlobalIndex = static_cast<int>(globalFbxMaterialList.size() - 1);
            materialContext.mVaild = false;
            materialContext.mKey = "";
        }
        for (int materialIndex = 0; materialIndex < node->GetMaterialCount(); ++materialIndex)
        {
            fbxsdk::FbxSurfaceMaterial *material = node->GetMaterial(materialIndex);
            if (fbxMaterialKeyTable.find(material) != fbxMaterialKeyTable.end())
                continue;

            globalFbxMaterialList.push_back(material);
            FBXMaterialKeyContext &materialContext = fbxMaterialKeyTable[material];
            materialContext.mGlobalIndex = static_cast<int>(globalFbxMaterialList.size() - 1);
            materialContext.mVaild = material != nullptr;
            materialContext.mKey = material != nullptr ? material->GetName() : "";
        }
    }

    // 표시 이름은 중복될 수 있으므로 FBX 재질 객체의 수집 인덱스를 임포트 내부 식별자로 사용한다.
    for (size_t materialIndex = 0; materialIndex < globalFbxMaterialList.size(); ++materialIndex)
    {
        fbxsdk::FbxSurfaceMaterial *material = globalFbxMaterialList[materialIndex];
        if (material != nullptr)
            fbxMaterialKeyTable[material].mImportKey = "Material::" + std::to_string(materialIndex);
    }

    // fbx material에서 fbx texture list 구축
    // 텍스처들 수집
    ExtractTextureList(globalFbxMaterialList, fbxTextureList);

    // static mesh-node list table 구축

    // skinned mesh -node table 구축

    // 순수하게 mesh 정보 추출
    for (auto fbxMesh : importContext.mAllMeshNodes)
    {

        // 메시및 관련 데이터 추출 및 구축
        ExtractMeshData(importContext, importContext.mAssetContext, fbxMesh, fbxMaterialKeyTable,
                        importContext.mWindingFlipFlag);

        // 하나의 메시라도 필요하다면 전체메시들에대해서 다 계산한다.

        importContext.mNeedToCalculateNormals =
            importContext.mNeedToCalculateNormals || !CheckFbxLayerNormalElement(fbxMesh);

        importContext.mNeedToCalculateTangents =
            importContext.mNeedToCalculateTangents || !CheckFbxLayerTangentElement(fbxMesh);
    }

    // 정적메시에 한해서
    MergeMeshData(importContext.mAssetContext, importContext.mStaticMeshToNodes, fbxMaterialKeyTable,
                  importContext.mFbxFileName, importContext.mFlipZ, importContext.mBakeAxisTransformMatrix);

    // skinning Mesh 한해서
    BuildSkinnedMeshData(importContext.mAssetContext, importContext.mSkinnedMeshNodeList, fbxMaterialKeyTable,
                         importContext.mBakeAxisTransformMatrix, importContext.mSkeleton);

    importContext.mAssetContext.mMeshNodeInfoList = std::move(importContext.mMeshNodeInfoList);
    importContext.mMaterialKeyTable = std::move(fbxMaterialKeyTable);
    importContext.mFbxTextureList = std::move(fbxTextureList);
}

void FBXImporter::ExtractMeshData(
    FBXImportContext &importContext, FBXImportAssetContext &importAssetContext, fbxsdk::FbxMesh *fbxMesh,
    const std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable,
    bool bIndexFlip) const
{

    importAssetContext.mMeshTable[fbxMesh] = std::make_unique<FBXImportMesh>();
    FBXImportMesh *pImportMesh = importAssetContext.mMeshTable[fbxMesh].get();
    pImportMesh->mName = fbxMesh->GetName();

    std::vector<FBXImportVertex> tempVertices;
    std::vector<uint32_t> templIndices;
    bool bSkinned = false;
    BuildTempVertices(fbxMesh, tempVertices, fbxMaterialKeyTable, importContext.mSkeleton, bSkinned);

    std::vector<FBXImportVertex> finalVertices;
    std::vector<uint32_t> finalIndices;
    std::vector<FBXImportSubMesh> importSubMeshList;

    // 중복 처리 (중복 정점 제거, 인덱스값 조정 )
    // 위치, 노멀, uv ,material index 가 동일하면 동일한 vertex 중복 처
    BuildIndexedVertices(tempVertices, finalVertices, finalIndices, importSubMeshList, bIndexFlip, bSkinned);

    pImportMesh->mVertices = std::move(finalVertices);
    pImportMesh->mIndices = std::move(finalIndices);
    pImportMesh->mSubMeshes = std::move(importSubMeshList);
    pImportMesh->bSkinned = bSkinned;

    // BuildMeshParts(pImportMesh, fbxNodes, fbxMaterialKeyTable);
}

void FBXImporter::ExtractMaterialData(FbxMeshNodeInfo &fbxMeshNodeInfo) const
{

    if (fbxMeshNodeInfo.mNode == nullptr)
        return;

    fbxsdk::FbxNode *node = fbxMeshNodeInfo.mNode;

    int materialCount = node->GetMaterialCount();

    for (int i = 0; i < materialCount; ++i)
    {
        fbxsdk::FbxSurfaceMaterial *surfaceMaterial = node->GetMaterial(i);
        fbxMeshNodeInfo.mMaterials.push_back(surfaceMaterial);

        if (surfaceMaterial->GetClassId().Is(fbxsdk::FbxSurfacePhong::ClassId))
        {

            fbxsdk::FbxSurfacePhong *surfacePhong = static_cast<fbxsdk::FbxSurfacePhong *>(surfaceMaterial);
        }
        else if (surfaceMaterial->GetClassId().Is(fbxsdk::FbxSurfaceLambert::ClassId))
        {
        }
    }
}

void FBXImporter::ExtractMeshLayerData(fbxsdk::FbxMesh *fbxMesh) const
{
    // 0번 layer만 처리
    fbxsdk::FbxLayer *layer = fbxMesh->GetLayer(0);

    if (layer == nullptr)
        return;

    fbxsdk::FbxLayerElementMaterial *layerElementMaterial = layer->GetMaterials();
    if (layerElementMaterial)
    {
        // ExtractMaterial
        ExtractMaterialLayerElement(layerElementMaterial);
    }
}

void FBXImporter::ExtractMaterialLayerElement(fbxsdk::FbxLayerElementMaterial *layerElementMaterial) const {}

bool FBXImporter::ExtractVertexNormal(fbxsdk::FbxMesh *fbxMesh, int polygonIndex, int polygonVertexIndex,
                                      int controlPointIndex, CoreMath::Vector3 &oNormal) const
{

    fbxsdk::FbxLayerElementNormal *fbxLayerElementNormal = fbxMesh->GetElementNormal();
    if (fbxLayerElementNormal == nullptr)
        return false;

    fbxsdk::FbxLayerElement::EMappingMode mappingMode = fbxLayerElementNormal->GetMappingMode();
    fbxsdk::FbxLayerElement::EReferenceMode referenceMode = fbxLayerElementNormal->GetReferenceMode();

    int normalIndex = 0;
    if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByControlPoint)
    {
        normalIndex = controlPointIndex;
    }
    else if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByPolygonVertex)
    {
        normalIndex = fbxMesh->GetPolygonVertexIndex(polygonIndex) + polygonVertexIndex;
    }

    if (referenceMode == fbxsdk::FbxLayerElement::eIndexToDirect)
    {
        normalIndex = fbxLayerElementNormal->GetIndexArray().GetAt(normalIndex);
    }

    fbxsdk::FbxVector4 fbxNormal = fbxLayerElementNormal->GetDirectArray().GetAt(normalIndex);
    Copy(fbxNormal, oNormal);

    return true;
}

bool FBXImporter::ExtractVertexUV(fbxsdk::FbxMesh *fbxMesh, int polygonIndex, int polygonVertexIndex,
                                  int controlPointIndex, CoreMath::Vector2 &oUV) const
{

    fbxsdk::FbxLayerElementUV *fbxLayerElementUV = fbxMesh->GetElementUV();
    if (fbxLayerElementUV == nullptr)
        return false;

    fbxsdk::FbxVector2 uv;
    bool unmapped = false;
    bool result =
        fbxMesh->GetPolygonVertexUV(polygonIndex, polygonVertexIndex, fbxLayerElementUV->GetName(), uv, unmapped);

    if (!result || unmapped)
        return false;

    Copy(uv, oUV);
    oUV.Y = 1.0f - oUV.Y;

    return true;

    // fbxsdk::FbxLayerElement::EMappingMode mappingMode = fbxLayerElementUV->GetMappingMode();
    // fbxsdk::FbxLayerElement::EReferenceMode referenceMode = fbxLayerElementUV->GetReferenceMode();

    // int index = 0;
    // if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByControlPoint)
    //{
    //     index = controlPointIndex;
    //     if (referenceMode == fbxsdk::FbxLayerElement::eIndexToDirect)
    //     {
    //         index = fbxLayerElementUV->GetIndexArray().GetAt(index);
    //     }
    // }
    // else if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByPolygonVertex)
    //{
    //     //   index = fbxMesh->GetPolygonVertexIndex(polygonIndex) + polygonVertexIndex;
    //     index = fbxMesh->GetTextureUVIndex(polygonIndex, polygonVertexIndex);
    // }

    //// if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eIndexToDirect)
    ////{
    ////     index = fbxLayerElementUV->GetIndexArray().GetAt(index);
    //// }

    // fbxsdk::FbxVector2 fbxTex = fbxLayerElementUV->GetDirectArray().GetAt(index);
    // Copy(fbxTex, oUV);
    // oUV.Y = 1.0f - oUV.Y;

    // return true;
}

bool FBXImporter::ExtractVertexTangent(fbxsdk::FbxMesh *fbxMesh, int polygonIndex, int polygonVertexIndex,
                                       int controlPointIndex, CoreMath::Vector4 &oTangent) const
{

    fbxsdk::FbxLayerElementTangent *fbxLayerElementTangent = fbxMesh->GetElementTangent();

    if (fbxLayerElementTangent == nullptr)
    {
        return false;
    }

    fbxsdk::FbxLayerElement::EMappingMode mappingMode = fbxLayerElementTangent->GetMappingMode();
    fbxsdk::FbxLayerElement::EReferenceMode referenceMode = fbxLayerElementTangent->GetReferenceMode();
    int index = 0;
    if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByControlPoint)
    {
        index = controlPointIndex;
    }
    else if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByPolygonVertex)
    {
        index = fbxMesh->GetPolygonVertexIndex(polygonIndex) + polygonVertexIndex;
        // index = fbxMesh->GetTextureUVIndex(polygonIndex, polygonVertexIndex);
    }

    if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eIndexToDirect)
    {
        index = fbxLayerElementTangent->GetIndexArray().GetAt(index);
    }

    fbxsdk::FbxVector4 fbxTangent = fbxLayerElementTangent->GetDirectArray().GetAt(index);
    Copy(fbxTangent, oTangent);

    return true;
}

void FBXImporter::ExtractTextureList(const std::vector<fbxsdk::FbxSurfaceMaterial *> &fbxSurfaceMaterialList,
                                     std::vector<fbxsdk::FbxTexture *> &oFbxTextureList) const
{

    for (auto fbxSurfaceMaterial : fbxSurfaceMaterialList)
    {
        // 재질이 없는 submesh의 기본 재질 표식은 FBX 재질 객체가 아니므로 텍스처를 조회하지 않는다.
        if (fbxSurfaceMaterial == nullptr)
            continue;

        FbxProperty diffuseProperty = fbxSurfaceMaterial->FindProperty(fbxsdk::FbxSurfaceMaterial::sDiffuse);
        if (diffuseProperty.IsValid())
        {
            fbxsdk::FbxTexture *fbxTexture = diffuseProperty.GetSrcObject<fbxsdk::FbxTexture>();
            if (fbxTexture)
            {
                auto it = std::find(oFbxTextureList.begin(), oFbxTextureList.end(), fbxTexture);
                if (it == oFbxTextureList.end())
                {
                    oFbxTextureList.push_back(fbxTexture);
                }
            }
        }
    }
}

bool FBXImporter::CheckFbxLayerTangentElement(fbxsdk::FbxMesh *fbxMesh) const
{
    return fbxMesh->GetElementTangent() != nullptr ? true : false;
}

bool FBXImporter::CheckFbxLayerNormalElement(fbxsdk::FbxMesh *fbxMesh) const
{

    return fbxMesh->GetElementNormal() != nullptr ? true : false;
}

void FBXImporter::MergeMeshData(
    FBXImportAssetContext &importAssetContext,
    const std::unordered_map<fbxsdk::FbxMesh *, std::vector<fbxsdk::FbxNode *>> &meshToNodes,
    const std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable,
    const std::string &totalMeshName, bool flipZ, const CoreMath::Matrix4X4 &bakeAxisTransformMatrix) const
{

    std::unordered_map<int, FBXImportTempMeshPerMat> tempMeshPerMatTable;

    // 병합된 vertext list 에서의 Node별 mesh vertex offset
    std::unordered_map<fbxsdk::FbxNode *, uint32_t> meshOffsetTable;

    // mesh들의 vertex 병합

    fbxsdk::FbxAMatrix bakeAxisTransformMatFbx;
    for (int i = 0; i < 4; ++i)
    {
        bakeAxisTransformMatFbx.mData[i] = {bakeAxisTransformMatrix.mat[i].X, bakeAxisTransformMatrix.mat[i].Y,
                                            bakeAxisTransformMatrix.mat[i].Z, bakeAxisTransformMatrix.mat[i].W};
    }

    std::vector<FBXImportVertex> &totalVertices = importAssetContext.mTotalMesh.mVertices;
    std::vector<uint32_t> &totalIndices = importAssetContext.mTotalMesh.mIndices;

    bool indexWindingFlip = false;
    for (auto &e : meshToNodes)
    {
        fbxsdk::FbxMesh *fbxMesh = e.first;
        const std::vector<fbxsdk::FbxNode *> &nodes = e.second;

        std::unique_ptr<FBXImportMesh> &importMesh = importAssetContext.mMeshTable[fbxMesh];

        for (auto node : nodes)
        {

            uint32_t meshOffset = totalVertices.size();

            // vertex bake 적용로직이 향후 들어갈것이다.

            // BakeVertex(importMesh->mVertices, localMatrix );

            // CoreMath::Matrix4X4 globalMatrix;
            // Copy(node->EvaluateGlobalTransform(), globalMatrix);
            //  CoreMath::Matrix4X4 matrix = globalMatrix * GetGeometrix(node);
            fbxsdk::FbxAMatrix geoMat;
            GetGeometrix(node, geoMat);

            fbxsdk::FbxAMatrix Amatrix = bakeAxisTransformMatFbx * node->EvaluateGlobalTransform() * geoMat;
            if (Amatrix.Determinant() < 0)
            {
                indexWindingFlip = true;
            }
            BakeVertex(importMesh->mVertices, Amatrix, flipZ);
            totalVertices.insert(totalVertices.end(), importMesh->mVertices.begin(), importMesh->mVertices.end());

            meshOffsetTable[node] = meshOffset;
        }
    }

    for (const auto &e : meshToNodes)
    {
        fbxsdk::FbxMesh *fbxMesh = e.first;
        const std::vector<fbxsdk::FbxNode *> &nodes = e.second;

        for (auto fbxNode : nodes)
        {
            std::unique_ptr<FBXImportMesh> &pFbxImportMesh = importAssetContext.mMeshTable[fbxMesh];

            for (const auto &subMesh : pFbxImportMesh->mSubMeshes)
            {

                // global mat index 구하기
                int localMaterialIndex = subMesh.mMaterialSlotIndex;
                auto fbxSurfaceMatIt = fbxMaterialKeyTable.find(fbxNode->GetMaterial(localMaterialIndex));

                if (fbxSurfaceMatIt == fbxMaterialKeyTable.end())
                {
                    continue;
                }

                const FBXMaterialKeyContext &FBXMaterialkeyContext = fbxSurfaceMatIt->second;
                int globalMaterialIndex = FBXMaterialkeyContext.mGlobalIndex;

                // index 복사
                const std::vector<uint32_t> &localIndices = pFbxImportMesh->mIndices;

                uint32_t meshVertexOffset = meshOffsetTable[fbxNode];
                for (size_t i = subMesh.mIndexOffset; i < subMesh.mIndexNum + subMesh.mIndexOffset; ++i)
                {
                    uint32_t globalIndex = localIndices[i] + meshVertexOffset;
                    tempMeshPerMatTable[globalMaterialIndex].mIndices.push_back(globalIndex);
                }
            }
        }
    }

    // 머터리얼별 index list 병합
    // 머터리얼별 서브메시 구축

    for (auto &e : tempMeshPerMatTable)
    {
        int matIndex = e.first;
        FBXImportTempMeshPerMat &tempMeshPerMat = e.second;

        size_t indexOffset = totalIndices.size();

        FBXImportSubMesh subMesh;
        subMesh.mIndexOffset = indexOffset;
        subMesh.mIndexNum = tempMeshPerMat.mIndices.size();
        subMesh.mVertexOffset = 0;
        subMesh.mMaterialSlotIndex = matIndex;

        importAssetContext.mTotalMesh.mSubMeshes.push_back(subMesh);

        // 인덱스 리스트 합병
        totalIndices.insert(totalIndices.end(), tempMeshPerMat.mIndices.begin(), tempMeshPerMat.mIndices.end());
    }

    if (indexWindingFlip)
    {
        for (size_t i = 0; i < totalIndices.size() / 3; ++i)
        {
            std::swap(totalIndices[i * 3 + 1], totalIndices[i * 3 + 2]);
        }
    }

    importAssetContext.mTotalMesh.mName = totalMeshName;
}

void FBXImporter::BakeVertex(std::vector<FBXImportVertex> &vertices, const fbxsdk::FbxAMatrix &Amatrix,
                             bool bFlipZ) const
{

    Amatrix.Determinant();

    fbxsdk::FbxAMatrix normalMatrix = Amatrix.Inverse().Transpose();

    for (auto &vertex : vertices)
    {
        fbxsdk::FbxVector4 p(vertex.mPosition.X, vertex.mPosition.Y, vertex.mPosition.Z, 1.0);
        fbxsdk::FbxVector4 n(vertex.mNormal.X, vertex.mNormal.Y, vertex.mNormal.Z, 0.0);
        fbxsdk::FbxVector4 t(vertex.mTangent.X, vertex.mTangent.Y, vertex.mTangent.Z, 0.0f);

        fbxsdk::FbxVector4 bp = Amatrix.MultT(p);
        fbxsdk::FbxVector4 bn = normalMatrix.MultT(n);
        fbxsdk::FbxVector4 bt = Amatrix.MultT(t);
        bt.Normalize();

        vertex.mPosition = {static_cast<float>(bp[0]), static_cast<float>(bp[1]), static_cast<float>(bp[2])};
        vertex.mNormal = {static_cast<float>(bn[0]), static_cast<float>(bn[1]), static_cast<float>(bn[2])};
        vertex.mTangent = {static_cast<float>(bt[0]), static_cast<float>(bt[1]), static_cast<float>(bt[2]),
                           vertex.mTangent.W};

        if (bFlipZ)
        {
            vertex.mPosition.Z *= -1.0f;
            vertex.mNormal.Z *= -1.0f;
            vertex.mTangent.Z *= -1.0f;
            vertex.mTangent.W *= -1.0f;
        }
        vertex.mNormal.Normalize();
    }
}

void FBXImporter::BuildSkinnedMeshData(
    FBXImportAssetContext &importAssetContext, const std::vector<fbxsdk::FbxMesh *> &fbxSkinnedMeshList,
    std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable,
    const CoreMath::Matrix4X4 &bakeAxisTransformMatrix, const FBXImportSkeleton &fbxImportSkeleton) const
{
    fbxsdk::FbxAMatrix bakeAxisTransformMatFbx;
    for (int i = 0; i < 4; ++i)
    {
        bakeAxisTransformMatFbx.mData[i] = {bakeAxisTransformMatrix.mat[i].X, bakeAxisTransformMatrix.mat[i].Y,
                                            bakeAxisTransformMatrix.mat[i].Z, bakeAxisTransformMatrix.mat[i].W};
    }

    for (fbxsdk::FbxMesh *fbxMesh : fbxSkinnedMeshList)
    {
        auto meshIt = importAssetContext.mMeshTable.find(fbxMesh);
        if (meshIt == importAssetContext.mMeshTable.end() || meshIt->second == nullptr)
            continue;

        FBXImportMesh &importMesh = *meshIt->second;
        fbxsdk::FbxNode *meshNode = fbxMesh->GetNode();
        if (meshNode == nullptr)
            continue;

        fbxsdk::FbxAMatrix geoMat;
        GetGeometrix(meshNode, geoMat);

        auto fbxSkin = static_cast<fbxsdk::FbxSkin *>(fbxMesh->GetDeformer(0, fbxsdk::FbxDeformer::eSkin));
        if (fbxSkin == nullptr || fbxSkin->GetClusterCount() == 0)
            continue;

        fbxsdk::FbxCluster *cluster = fbxSkin->GetCluster(0);
        if (cluster == nullptr)
            continue;

        fbxsdk::FbxAMatrix meshBindGlobal;
        cluster->GetTransformMatrix(meshBindGlobal);

        // 메시 bind 배치는 추후 joint별 inverse bind에 포함한다. 여기서는 모든 정점에 공통인
        // geometric 변환과 엔진 축 변환만 저장 정점에 적용해 메시 로컬 공간을 유지한다.
        const fbxsdk::FbxAMatrix vertexBakeMatrix = bakeAxisTransformMatFbx * geoMat;
        BakeVertex(importMesh.mVertices, vertexBakeMatrix, false);

        // bind 자세의 최종 위치에는 inverse bind를 통해 mesh bind 배치도 적용된다.
        // 따라서 인덱스 방향은 정점 bake만이 아니라 전체 bind 위치 변환의 부호로 메시별 판정한다.
        const fbxsdk::FbxAMatrix bindPositionMatrix = bakeAxisTransformMatFbx * meshBindGlobal * geoMat;
        if (bindPositionMatrix.Determinant() < 0.0)
        {
            for (size_t index = 0; index + 2 < importMesh.mIndices.size(); index += 3)
            {
                std::swap(importMesh.mIndices[index + 1], importMesh.mIndices[index + 2]);
            }
        }

        // ExtractMeshData가 이미 이 메시만의 정점·인덱스와 material slot별 submesh를 만들었다.
        // 병합하지 않고 해당 node의 재질 key를 submesh 순서에 대응시키는 part 하나만 구성한다.
        BuildMeshParts(&importMesh, {meshNode}, fbxMaterialKeyTable);

        int clusterCount = fbxSkin->GetClusterCount();
        for (int i = 0; i < clusterCount; ++i)
        {
            fbxsdk::FbxCluster *cluster = fbxSkin->GetCluster(i);

            auto linkNode = cluster->GetLink();

            auto jointIndexIt = fbxImportSkeleton.mJointNodeIndexTable.find(linkNode);

            if (jointIndexIt == fbxImportSkeleton.mJointNodeIndexTable.end())
                continue;

            int32_t jointIndex = jointIndexIt->second;

            importMesh.mPaletteToSkeletonJoint.push_back(jointIndex);

            fbxsdk::FbxAMatrix bindToBoneMatrix;
            cluster->GetTransformLinkMatrix(bindToBoneMatrix);
            bindToBoneMatrix = bindToBoneMatrix.Inverse();

            fbxsdk::FbxAMatrix offsetMatrix =
                bakeAxisTransformMatFbx * bindToBoneMatrix * meshBindGlobal * bakeAxisTransformMatFbx.Inverse();

            CoreMath::Matrix4X4 B;
            Copy(offsetMatrix, B);
            importMesh.mInverseBindMatrices.push_back(B);
        }

        // BuildTempVertices는 Skeleton joint index를 기록한다. 저장 정점은 이 메시의
        // palette를 가리켜야 하므로 remap 배열의 순서를 기준으로 한 번만 변환한다.
        std::unordered_map<int32_t, int32_t> jointToPaletteIndex;
        for (size_t paletteIndex = 0; paletteIndex < importMesh.mPaletteToSkeletonJoint.size(); ++paletteIndex)
        {
            jointToPaletteIndex.emplace(static_cast<int32_t>(importMesh.mPaletteToSkeletonJoint[paletteIndex]),
                                        static_cast<int32_t>(paletteIndex));
        }

        // 유효한 영향이 palette에서 빠졌다면 일부 정점만 재매핑된 상태로 남기지 않는다.
        for (const FBXImportVertex &vertex : importMesh.mVertices)
        {
            for (int slot = 0; slot < 4; ++slot)
            {
                if ((vertex.mJointWeight0[slot] > 0.0f &&
                     jointToPaletteIndex.find(vertex.mJointIndex0[slot]) == jointToPaletteIndex.end()) ||
                    (vertex.mJointWeight1[slot] > 0.0f &&
                     jointToPaletteIndex.find(vertex.mJointIndex1[slot]) == jointToPaletteIndex.end()))
                {
                    LOG_MESSAGE_ERROR("FBX Importer", "스키닝 정점의 joint index가 메시 palette에 없습니다.");
                    return;
                }
            }
        }

        auto remapIndex = [&jointToPaletteIndex](int32_t &index, float weight)
        {
            if (!(weight > 0.0f))
            {
                index = 0;
                return;
            }
            index = jointToPaletteIndex.at(index);
        };

        for (FBXImportVertex &vertex : importMesh.mVertices)
        {
            for (int slot = 0; slot < 4; ++slot)
            {
                remapIndex(vertex.mJointIndex0[slot], vertex.mJointWeight0[slot]);
                remapIndex(vertex.mJointIndex1[slot], vertex.mJointWeight1[slot]);
            }
        }
    }
}

bool FBXImporter::BuildIntermediateAssets(FBXImportContext &importContext,
                                          CoreAsset::ImportPackage &oImportPackage) const
{
    const CoreAsset::ImportAssetKey skeletonKey = importContext.mFbxFileName + "::Skeleton";
    const CoreAsset::ImportAssetKey skeletonImportKey = "Skeleton::0";
    if (importContext.mSkeleton.mJoints.empty() &&
        (!importContext.mSkinnedMeshNodeList.empty() || !importContext.mAnimationClipAssets.empty()))
    {
        oImportPackage.mFailReason = "스키닝 메시 또는 클립이 있지만 Skeleton joint가 없습니다.";
        return false;
    }

    if (!importContext.mSkeleton.mJoints.empty())
    {
        auto skeleton = std::make_unique<CoreAsset::IntermediateSkeleton>();
        const std::string skeletonName = importContext.mFbxFileName + "_Skeleton";
        skeleton->mAssetName = skeletonName.c_str();
        skeleton->mSkeletonJoints.reserve(importContext.mSkeleton.mJoints.size());

        for (size_t jointIndex = 0; jointIndex < importContext.mSkeleton.mJoints.size(); ++jointIndex)
        {
            const FBXImportJoint &sourceJoint = importContext.mSkeleton.mJoints[jointIndex];
            CoreAsset::SkeletonJoint joint;
            joint.mDisplayName = sourceJoint.mName;
            joint.mStableKey = sourceJoint.mStableKey;

            // FBX의 -1 root와 CoreAsset의 NoParent를 맞추고 parent-first 계층을 확인한다.
            if (sourceJoint.mParentJointIndex == -1)
                joint.mParentIndex = CoreAsset::SkeletonJoint::NoParent;
            else if (sourceJoint.mParentJointIndex >= 0 &&
                     static_cast<size_t>(sourceJoint.mParentJointIndex) < jointIndex)
                joint.mParentIndex = static_cast<uint32_t>(sourceJoint.mParentJointIndex);
            else
            {
                oImportPackage.mFailReason = "FBX Skeleton joint의 부모 인덱스가 parent-first 규칙을 위반합니다.";
                return false;
            }

            // 최종 Skeleton은 행렬이 아닌 parent-local TRS를 소유한다. 분해 성공만으로 shear가
            // 보존되는 것은 아니므로 변환 전 원본 행렬도 V1 validator에 함께 전달한다.
            if (!sourceJoint.mReferencePose.Decompose(joint.mReferenceLocalPose.mPosition,
                                                      joint.mReferenceLocalPose.mRotation,
                                                      joint.mReferenceLocalPose.mScale) ||
                !CoreAsset::ValidateAnimationJointTransform(joint.mReferenceLocalPose, &sourceJoint.mReferencePose)
                     .mValid)
            {
                oImportPackage.mFailReason =
                    "FBX Skeleton reference pose가 V1 local TRS로 표현되지 않습니다: " + sourceJoint.mStableKey;
                return false;
            }
            skeleton->mSkeletonJoints.push_back(std::move(joint));
        }

        CoreAsset::ImportedIntermediateAsset skeletonAsset;
        skeletonAsset.mKey = skeletonKey;
        skeletonAsset.mImportKey = skeletonImportKey;
        skeletonAsset.mIntermediateAsset = std::move(skeleton);
        oImportPackage.mInteremdiateAssets.push_back(std::move(skeletonAsset));
    }

    // intermediate mesh asset 생성
    // 하나의 mesh만 나옴(static combine mode라고 보면됨 )
    if (!importContext.mStaticMeshToNodes.empty())
    {
        std::vector<CoreAsset::ImportDependencyContext> importDependencyContextList;
        CoreAsset::ImportedIntermediateAsset importAsset =
            BuildIntermediateMeshAsset(importContext, importDependencyContextList);

        oImportPackage.mInteremdiateAssets.push_back(std::move(importAsset));
        oImportPackage.mDependencyContexts.insert(oImportPackage.mDependencyContexts.begin(),
                                                  importDependencyContextList.begin(),
                                                  importDependencyContextList.end());
    }

    for (size_t meshIndex = 0; meshIndex < importContext.mSkinnedMeshNodeList.size(); ++meshIndex)
    {
        fbxsdk::FbxMesh *fbxMesh = importContext.mSkinnedMeshNodeList[meshIndex];
        const auto meshIt = importContext.mAssetContext.mMeshTable.find(fbxMesh);
        if (meshIt == importContext.mAssetContext.mMeshTable.end() || meshIt->second == nullptr)
        {
            oImportPackage.mFailReason = "스키닝 메시의 추출 데이터가 없습니다.";
            return false;
        }

        const FBXImportMesh &sourceMesh = *meshIt->second;
        if (!sourceMesh.bSkinned || sourceMesh.mVertices.empty() || sourceMesh.mIndices.empty() ||
            sourceMesh.mSubMeshes.empty() || sourceMesh.mPaletteToSkeletonJoint.empty() ||
            sourceMesh.mPaletteToSkeletonJoint.size() != sourceMesh.mInverseBindMatrices.size() ||
            sourceMesh.mMeshPartInstances.empty() ||
            sourceMesh.mMeshPartInstances[0].mSubMeshMaterialKeyList.size() != sourceMesh.mSubMeshes.size())
        {
            oImportPackage.mFailReason =
                "스키닝 메시의 정점, palette 또는 submesh 데이터가 불완전합니다: " + sourceMesh.mName;
            return false;
        }

        const CoreAsset::ImportAssetKey meshKey =
            importContext.mFbxFileName + "::SkinningMesh::" + std::to_string(meshIndex);
        const CoreAsset::ImportAssetKey meshImportKey = "SkinningMesh::" + std::to_string(meshIndex);
        auto skinningMesh = std::make_unique<CoreAsset::IntermediateSkinningMesh>();
        const std::string meshName = sourceMesh.mName.empty()
                                         ? importContext.mFbxFileName + "_SkinningMesh_" + std::to_string(meshIndex)
                                         : sourceMesh.mName;
        skinningMesh->mAssetName = meshName.c_str();
        skinningMesh->bCaculateAABB = true;
        skinningMesh->mIndexVector = sourceMesh.mIndices;
        skinningMesh->mInverseBindMatrices = sourceMesh.mInverseBindMatrices;
        skinningMesh->mPaletteToSkeletonJoint = sourceMesh.mPaletteToSkeletonJoint;
        skinningMesh->mVertexVector.reserve(sourceMesh.mVertices.size());

        for (uint32_t jointIndex : skinningMesh->mPaletteToSkeletonJoint)
        {
            if (jointIndex >= importContext.mSkeleton.mJoints.size())
            {
                oImportPackage.mFailReason =
                    "스키닝 메시 palette가 Skeleton joint 범위를 벗어났습니다: " + sourceMesh.mName;
                return false;
            }
        }

        for (const FBXImportVertex &sourceVertex : sourceMesh.mVertices)
        {
            CoreAsset::SkinningVertex vertex;
            vertex.mPos = sourceVertex.mPosition;
            vertex.mTex = sourceVertex.mUV;
            vertex.mNormal = sourceVertex.mNormal;
            vertex.mTangent = sourceVertex.mTangent;

            std::vector<CoreAsset::SkinInfluence> influences;
            influences.reserve(8);
            for (int slot = 0; slot < 4; ++slot)
            {
                const int32_t indices[2] = {sourceVertex.mJointIndex0[slot], sourceVertex.mJointIndex1[slot]};
                const float weights[2] = {sourceVertex.mJointWeight0[slot], sourceVertex.mJointWeight1[slot]};
                for (int influenceIndex = 0; influenceIndex < 2; ++influenceIndex)
                {
                    if (!(weights[influenceIndex] > 0.0f))
                        continue;
                    if (indices[influenceIndex] < 0 ||
                        static_cast<size_t>(indices[influenceIndex]) >= skinningMesh->mPaletteToSkeletonJoint.size())
                    {
                        oImportPackage.mFailReason =
                            "스키닝 정점의 palette index가 범위를 벗어났습니다: " + sourceMesh.mName;
                        return false;
                    }
                    influences.push_back({static_cast<uint32_t>(indices[influenceIndex]), weights[influenceIndex]});
                }
            }

            // 저장 정점은 V1 계약에 따라 최대 4개 influence만 보유하고 남은 weight 합을 1로 정규화한다.
            std::string influenceFailureReason;
            if (!CoreAsset::NormalizeSkinInfluences(influences, vertex, &influenceFailureReason))
            {
                oImportPackage.mFailReason =
                    "스키닝 정점의 influence가 유효하지 않습니다: " + sourceMesh.mName + ", " + influenceFailureReason;
                return false;
            }
            skinningMesh->mVertexVector.push_back(std::move(vertex));
        }

        for (const FBXImportMeshPart &sourcePart : sourceMesh.mMeshParts)
        {
            CoreAsset::MeshPart part;
            part.mStartSubMeshIndex = sourcePart.mStartSubMeshIndex;
            part.mSubMeshCount = sourcePart.mSubMeshCount;
            skinningMesh->mMeshPartVector.push_back(part);
        }

        const FBXImportMeshPartInstance &sourceInstance = sourceMesh.mMeshPartInstances[0];
        CoreAsset::MeshPartInstance partInstance;
        partInstance.mMeshPartIndex = sourceInstance.mMeshPartIndex;
        // skinned vertex는 geometric transform을 이미 베이크했고 mesh bind 배치는 inverse bind에 포함된다.
        partInstance.mLocalTransform = CoreMath::Matrix4X4::Identity;
        skinningMesh->mMeshPartInstanceVector.push_back(std::move(partInstance));
        for (size_t subMeshIndex = 0; subMeshIndex < sourceMesh.mSubMeshes.size(); ++subMeshIndex)
        {
            const FBXImportSubMesh &sourceSubMesh = sourceMesh.mSubMeshes[subMeshIndex];
            if (static_cast<size_t>(sourceSubMesh.mIndexOffset) + sourceSubMesh.mIndexNum > sourceMesh.mIndices.size())
            {
                oImportPackage.mFailReason = "스키닝 submesh의 index 범위가 잘못되었습니다: " + sourceMesh.mName;
                return false;
            }

            CoreAsset::SubMesh subMesh;
            subMesh.mIndexNum = sourceSubMesh.mIndexNum;
            subMesh.mIndexOffset = sourceSubMesh.mIndexOffset;
            subMesh.mVertexOffset = sourceSubMesh.mVertexOffset;
            subMesh.mMaterialID = NoneAssetID;
            skinningMesh->mSubMeshVector.push_back(subMesh);

            const FBXMaterialKeyContext &materialKey = sourceInstance.mSubMeshMaterialKeyList[subMeshIndex];
            CoreAsset::ImportDependencyContext materialDependency{};
            materialDependency.mDependencyType = CoreAsset::EImportDependencyType::eSubMeshDefaultMaterial;
            materialDependency.mOwnerAssetKey = meshImportKey;
            materialDependency.mDependencyAssetKey = materialKey.mImportKey;
            materialDependency.mSlotIndex = static_cast<int>(subMeshIndex);
            materialDependency.mSubInfo = materialKey.mVaild ? CoreAsset::EImportDependencySubInfo::eNone
                                                             : CoreAsset::EImportDependencySubInfo::eUseDefaultMaterial;
            oImportPackage.mDependencyContexts.push_back(std::move(materialDependency));
        }

        CoreAsset::ImportDependencyContext skeletonDependency{};
        skeletonDependency.mDependencyType = CoreAsset::EImportDependencyType::eSkinningMesh_Skeleton;
        skeletonDependency.mOwnerAssetKey = meshImportKey;
        skeletonDependency.mDependencyAssetKey = skeletonImportKey;
        oImportPackage.mDependencyContexts.push_back(std::move(skeletonDependency));

        CoreAsset::ImportedIntermediateAsset meshAsset;
        meshAsset.mKey = meshKey;
        meshAsset.mImportKey = meshImportKey;
        meshAsset.mIntermediateAsset = std::move(skinningMesh);
        oImportPackage.mInteremdiateAssets.push_back(std::move(meshAsset));
    }

    std::unordered_map<fbxsdk::FbxTexture *, CoreAsset::ImportAssetKey> textureImportKeyTable;
    for (size_t textureIndex = 0; textureIndex < importContext.mFbxTextureList.size(); ++textureIndex)
    {
        fbxsdk::FbxTexture *texture = importContext.mFbxTextureList[textureIndex];
        if (texture->GetClassId() == fbxsdk::FbxFileTexture::ClassId)
            textureImportKeyTable.emplace(texture, "Texture::" + std::to_string(textureIndex));
    }

    // intermediate material asset 생성
    for (const auto &e : importContext.mMaterialKeyTable)
    {
        std::vector<CoreAsset::ImportDependencyContext> importDependencyContextList;
        fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial = e.first;
        CoreAsset::ImportedIntermediateAsset importAsset;

        bool bKeyValid = e.second.mVaild;
        importAsset.mKey = e.second.mKey;
        importAsset.mImportKey = e.second.mImportKey;

        // 동시에 텍스처 리소스 수집

        bool ret = BuildIntermediateMaterialAssets(fbxSurfaceMaterial, e.second.mImportKey, textureImportKeyTable,
                                                   importDependencyContextList, importAsset);

        // 디폴트머터리얼사용시 경우도 해당된다.
        if (ret == false)
            importAsset.mValid = false;

        oImportPackage.mInteremdiateAssets.push_back(std::move(importAsset));
        oImportPackage.mDependencyContexts.insert(oImportPackage.mDependencyContexts.begin(),
                                                  importDependencyContextList.begin(),
                                                  importDependencyContextList.end());
    }

    // fbxTexture들에대해서 requestTextureImport 생성
    for (size_t textureIndex = 0; textureIndex < importContext.mFbxTextureList.size(); ++textureIndex)
    {
        fbxsdk::FbxTexture *fbxTexture = importContext.mFbxTextureList[textureIndex];
        fbxsdk::FbxClassId classID = fbxTexture->GetClassId();

        CoreAsset::ImportRequestTextureContext requestTextureContext;

        if (classID == fbxsdk::FbxFileTexture::ClassId)
        {
            fbxsdk::FbxFileTexture *fbxFileTexture = static_cast<fbxsdk::FbxFileTexture *>(fbxTexture);
            requestTextureContext.mKey = textureImportKeyTable.at(fbxTexture);
            requestTextureContext.mFilePath = fbxFileTexture->GetFileName();
            oImportPackage.mImportRequestTextureContexts.push_back(requestTextureContext);
        }
        else if (classID == fbxsdk::FbxLayeredTexture::ClassId)
        {
            LOG_MESSAGE_ERROR("FBX Importer", "FbxTexture가 FbxLayerdTexture 타입이라서 중지함");
            CHECK(0);

            //    oImportPackage.mImportRequestTextureContexts.push_back(requestTextureContext);
        }
    }

    // clip은 추출 단계에서 완성된 항목만 context에 보관한다. 여기서는 복사하지 않고 package로 소유권을 넘긴다.
    for (CoreAsset::ImportedIntermediateAsset &animationClipAsset : importContext.mAnimationClipAssets)
    {
        CoreAsset::ImportDependencyContext skeletonDependency{};
        skeletonDependency.mDependencyType = CoreAsset::EImportDependencyType::eAnimClip_Skeleton;
        skeletonDependency.mOwnerAssetKey = animationClipAsset.mImportKey;
        skeletonDependency.mDependencyAssetKey = skeletonImportKey;
        oImportPackage.mDependencyContexts.push_back(std::move(skeletonDependency));
        oImportPackage.mInteremdiateAssets.push_back(std::move(animationClipAsset));
    }

    return true;
}

CoreAsset::ImportedIntermediateAsset FBXImporter::BuildIntermediateMeshAsset(
    FBXImportMesh *importMesh, std::vector<CoreAsset::ImportDependencyContext> &oDependencyList) const
{

    CoreAsset::ImportedIntermediateAsset importedIntermediateAsset;

    std::unique_ptr<CoreAsset::IntermediateStaticMesh> staticMesh =
        std::make_unique<CoreAsset::IntermediateStaticMesh>();

    CoreAsset::ImportAssetKey meshAssetKey = importMesh->mName;
    const CoreAsset::ImportAssetKey meshImportKey = "StaticMesh::0";

    // vertex
    for (size_t i = 0; i < importMesh->mVertices.size(); ++i)
    {
        CoreAsset::StaticVertex vertex;
        vertex.mPos = importMesh->mVertices[i].mPosition;
        vertex.mTex = importMesh->mVertices[i].mUV;
        vertex.mNormal = importMesh->mVertices[i].mNormal;
        vertex.mTangent = {0, 0, 1, 0};

        staticMesh->mVertexVector.push_back(vertex);
    }

    // index
    for (size_t i = 0; i < importMesh->mIndices.size(); ++i)
    {
        staticMesh->mIndexVector.push_back(importMesh->mIndices[i]);
    }

    // sub mesh
    for (size_t i = 0; i < importMesh->mSubMeshes.size(); ++i)
    {
        CoreAsset::SubMesh subMesh;

        subMesh.mIndexNum = importMesh->mSubMeshes[i].mIndexNum;
        subMesh.mIndexOffset = importMesh->mSubMeshes[i].mIndexOffset;
        subMesh.mVertexOffset = importMesh->mSubMeshes[i].mVertexOffset;
        subMesh.mMaterialID = NoneAssetID; // 설정할수없으니 NoneAssetID - > 의존성 CONTEXT에서 관련정보를 담음
        staticMesh->mSubMeshVector.push_back(subMesh);
    }

    // mesh part
    for (size_t i = 0; i < importMesh->mMeshParts.size(); ++i)
    {
        const FBXImportMeshPart &importMeshPart = importMesh->mMeshParts[i];
        CoreAsset::MeshPart meshPart;
        meshPart.mStartSubMeshIndex = importMeshPart.mStartSubMeshIndex;
        meshPart.mSubMeshCount = importMeshPart.mStartSubMeshIndex;

        staticMesh->mMeshPartVector.push_back(meshPart);
    }

    // mesh part instance;

    for (size_t i = 0; i < importMesh->mMeshPartInstances.size(); ++i)
    {
        FBXImportMeshPartInstance &importMeshPartInstance = importMesh->mMeshPartInstances[i];

        CoreAsset::MeshPartInstance meshPartInstance;
        meshPartInstance.mLocalTransform = importMeshPartInstance.mLocalTransform;
        meshPartInstance.mMeshPartIndex = importMeshPartInstance.mMeshPartIndex;

        // 의존성 context 구축
        //        meshPartInstance.mSubMeshMaterialIDList = importMeshPartInstance.mSubMeshMaterialKeyList;

        staticMesh->mMeshPartInstanceVector.push_back(meshPartInstance);

        for (int j = 0; j < importMeshPartInstance.mSubMeshMaterialKeyList.size(); ++j)
        {

            CoreAsset::ImportDependencyContext importDependencyContext;
            importDependencyContext.mDependencyType = CoreAsset::EImportDependencyType::eMeshPartInstanceMaterial;
            importDependencyContext.mOwnerAssetKey = meshImportKey;
            importDependencyContext.mDependencyAssetKey = importMeshPartInstance.mSubMeshMaterialKeyList[j].mImportKey;
            importDependencyContext.mSlotIndex = i;
            oDependencyList.push_back(importDependencyContext);
        }
    }

    // mesh 의 기본머터리얼 설정
    FBXImportMeshPartInstance &defaultMeshPartInstance = importMesh->mMeshPartInstances[0];

    for (int i = 0; i < defaultMeshPartInstance.mSubMeshMaterialKeyList.size(); ++i)
    {
        CoreAsset::ImportDependencyContext importDependencyContext;
        importDependencyContext.mDependencyType = CoreAsset::EImportDependencyType::eSubMeshDefaultMaterial;
        importDependencyContext.mOwnerAssetKey = meshImportKey;
        importDependencyContext.mDependencyAssetKey = defaultMeshPartInstance.mSubMeshMaterialKeyList[i].mImportKey;
        importDependencyContext.mSlotIndex = i;
        if (defaultMeshPartInstance.mSubMeshMaterialKeyList[i].mVaild == false)
        {
            importDependencyContext.mSubInfo = CoreAsset::EImportDependencySubInfo::eUseDefaultMaterial;
        }

        oDependencyList.push_back(importDependencyContext);
    }

    staticMesh->mAssetName = importMesh->mName.c_str();
    staticMesh->bCaculateAABB = true;

    importedIntermediateAsset.mIntermediateAsset = std::move(staticMesh);
    importedIntermediateAsset.mKey = meshAssetKey;
    importedIntermediateAsset.mImportKey = meshImportKey;

    return importedIntermediateAsset;
}

CoreAsset::ImportedIntermediateAsset FBXImporter::BuildIntermediateMeshAsset(
    const FBXImportContext &importContext, std::vector<CoreAsset::ImportDependencyContext> &oDependencyList) const
{

    CoreAsset::ImportedIntermediateAsset importedIntermediateAsset;

    std::unique_ptr<CoreAsset::IntermediateStaticMesh> staticMesh =
        std::make_unique<CoreAsset::IntermediateStaticMesh>();

    const FBXImportMesh &importMesh = importContext.mAssetContext.mTotalMesh;

    CoreAsset::ImportAssetKey meshAssetKey = importMesh.mName;
    const CoreAsset::ImportAssetKey meshImportKey = "StaticMesh::0";

    // vertex
    for (size_t i = 0; i < importMesh.mVertices.size(); ++i)
    {
        CoreAsset::StaticVertex vertex;
        vertex.mPos = importMesh.mVertices[i].mPosition;
        vertex.mTex = importMesh.mVertices[i].mUV;
        vertex.mNormal = importMesh.mVertices[i].mNormal;
        vertex.mTangent = importMesh.mVertices[i].mTangent;

        staticMesh->mVertexVector.push_back(vertex);
    }

    // index
    for (size_t i = 0; i < importMesh.mIndices.size(); ++i)
    {
        staticMesh->mIndexVector.push_back(importMesh.mIndices[i]);
    }

    // sub mesh
    for (size_t i = 0; i < importMesh.mSubMeshes.size(); ++i)
    {
        CoreAsset::SubMesh subMesh;

        subMesh.mIndexNum = importMesh.mSubMeshes[i].mIndexNum;
        subMesh.mIndexOffset = importMesh.mSubMeshes[i].mIndexOffset;
        subMesh.mVertexOffset = importMesh.mSubMeshes[i].mVertexOffset;
        subMesh.mMaterialID = NoneAssetID; // 설정할수없으니 NoneAssetID - > 의존성 CONTEXT에서 관련정보를 담음
        staticMesh->mSubMeshVector.push_back(subMesh);

        // dependency  구축

        fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial =
            importContext.mGlobalFbxMaterialList[importMesh.mSubMeshes[i].mMaterialSlotIndex];
        auto it = importContext.mMaterialKeyTable.find(fbxSurfaceMaterial);

        CoreAsset::ImportDependencyContext importDependencyContext{};
        importDependencyContext.mDependencyType = CoreAsset::EImportDependencyType::eSubMeshDefaultMaterial;
        importDependencyContext.mOwnerAssetKey = meshImportKey;
        if (it != importContext.mMaterialKeyTable.end())
            importDependencyContext.mDependencyAssetKey = it->second.mImportKey;
        if (it == importContext.mMaterialKeyTable.end() || it->second.mVaild == false)
            importDependencyContext.mSubInfo = CoreAsset::EImportDependencySubInfo::eUseDefaultMaterial;

        importDependencyContext.mSlotIndex = i;
        oDependencyList.push_back(importDependencyContext);
    }

    staticMesh->mAssetName = meshAssetKey.c_str();
    staticMesh->bCaculateAABB = true;

    importedIntermediateAsset.mIntermediateAsset = std::move(staticMesh);
    importedIntermediateAsset.mKey = meshAssetKey;
    importedIntermediateAsset.mImportKey = meshImportKey;
    return importedIntermediateAsset;
}

bool FBXImporter::BuildIntermediateMaterialAssets(
    fbxsdk::FbxSurfaceMaterial *surfaceMaterial, const CoreAsset::ImportAssetKey &materialImportKey,
    const std::unordered_map<fbxsdk::FbxTexture *, CoreAsset::ImportAssetKey> &textureImportKeyTable,
    std::vector<CoreAsset::ImportDependencyContext> &oDependencyList,
    CoreAsset::ImportedIntermediateAsset &oImportedIntermediateAsset) const
{

    if (surfaceMaterial == nullptr)
        return false;

    CoreAsset::ImportedIntermediateAsset &importedIntermediateAsset = oImportedIntermediateAsset;

    std::unique_ptr<CoreAsset::IntermediateMaterial> intermediateMaterial =
        std::make_unique<CoreAsset::IntermediateMaterial>();

    intermediateMaterial->mAssetName = surfaceMaterial->GetName();

    TryCopyVector3Property(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sDiffuse, intermediateMaterial->mDiffuseColor);
    TryCopyDoubleProperty(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sDiffuseFactor,
                          intermediateMaterial->mDiffuseFactor);
    TryCopyDoubleProperty(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sShininess, intermediateMaterial->mShininess);
    TryCopyVector3Property(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sSpecular, intermediateMaterial->mSpecular);
    TryCopyDoubleProperty(surfaceMaterial, fbxsdk::FbxSurfaceMaterial::sSpecularFactor,
                          intermediateMaterial->mSpecularFactor);

    // collect texture dependency
    CollectTextureDependencyFromProperty(surfaceMaterial, materialImportKey, textureImportKeyTable,
                                         fbxsdk::FbxSurfaceMaterial::sDiffuse,
                                         CoreAsset::EImportDependencySubInfo::eDiffuseMap, oDependencyList);

    importedIntermediateAsset.mIntermediateAsset = std::move(intermediateMaterial);

    return true;
}

void FBXImporter::BuildTempVertices(
    fbxsdk::FbxMesh *fbxMesh, std::vector<FBXImportVertex> &oTempVertices,
    const std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &fbxMaterialKeyTable,
    FBXImportSkeleton &skeleton, bool &bSkinned) const
{
    int controlPointCount = fbxMesh->GetControlPointsCount();
    fbxsdk::FbxVector4 *controlPointArray = fbxMesh->GetControlPoints();

    std::vector<FBXControlPoint> fbxControlPointList(controlPointCount);
    for (int i = 0; i < controlPointCount; ++i)
    {
        Copy(controlPointArray[i], fbxControlPointList[i].mPos);
    }

    fbxsdk::FbxSkin *fbxskin = static_cast<fbxsdk::FbxSkin *>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
    if (fbxskin)
    {
        bSkinned = true;
        int clusterCount = fbxskin->GetClusterCount();

        for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
        {

            fbxsdk::FbxCluster *fbxCluster = fbxskin->GetCluster(clusterIndex);

            if (fbxCluster == nullptr)
                continue;

            fbxsdk::FbxNode *fbxSkeletonNode = fbxCluster->GetLink();

            auto jointIndexIt = skeleton.mJointNodeIndexTable.find(fbxSkeletonNode);
            if (jointIndexIt == skeleton.mJointNodeIndexTable.end())
            {
                continue;
            }

            int32_t jointIndex = jointIndexIt->second;

            // 이 jointIndex는 지금 그 skeleton구조에서 비 skeleton 도 포함된상태에서의 index이기에 나중에 한번더
            // 가공을 거쳐야할것

            int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
            int *controlPointIndices = fbxCluster->GetControlPointIndices();
            double *controlPointWeights = fbxCluster->GetControlPointWeights();

            for (int j = 0; j < controlPointIndicesCount; ++j)
            {

                int controlpointIndex = controlPointIndices[j];
                double jointWeight = controlPointWeights[j];

                int nextJointLocalIndex = fbxControlPointList[controlpointIndex].mJointCount;

                fbxControlPointList[controlpointIndex].mJointIndex[nextJointLocalIndex] = jointIndex;
                fbxControlPointList[controlpointIndex].mWeight[nextJointLocalIndex] = jointWeight;

                fbxControlPointList[controlpointIndex].mJointCount++;
            }
        }
    }

    int polygonCount = fbxMesh->GetPolygonCount();

    for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
    {

        // 폴리곤별로 material slot index 설정
        // 머터리얼이없다면 -1
        int materialSlotIndex = GetPolygonMaterialSlot(fbxMesh, polygonIndex);

        for (int i = 0; i < fbxMesh->GetPolygonSize(polygonIndex); ++i)
        {
            // 폴리곤마다 중복정점생성
            FBXImportVertex vertex;
            vertex.mMaterialSlotIndex = materialSlotIndex;

            int controlPointIndex = fbxMesh->GetPolygonVertex(polygonIndex, i);
            //            Copy(controlPointArray[controlPointIndex], vertex.mPosition);

            vertex.mPosition = fbxControlPointList[controlPointIndex].mPos;

            if (bSkinned)
            {
                for (int i = 0; i < 4; ++i)
                {
                    vertex.mJointIndex0[i] = fbxControlPointList[controlPointIndex].mJointIndex[i];
                    vertex.mJointIndex1[i] = fbxControlPointList[controlPointIndex].mJointIndex[i + 4];

                    vertex.mJointWeight0[i] = fbxControlPointList[controlPointIndex].mWeight[i];
                    vertex.mJointWeight1[i] = fbxControlPointList[controlPointIndex].mWeight[i + 4];
                }
            }

            // normal
            ExtractVertexNormal(fbxMesh, polygonIndex, i, controlPointIndex, vertex.mNormal);

            // uv
            ExtractVertexUV(fbxMesh, polygonIndex, i, controlPointIndex, vertex.mUV);

            ExtractVertexTangent(fbxMesh, polygonIndex, i, controlPointIndex, vertex.mTangent);

            oTempVertices.push_back(vertex);
        }
    }
}

void FBXImporter::BuildIndexedVertices(const std::vector<FBXImportVertex> &tempVertices,
                                       std::vector<FBXImportVertex> &oFinalVertices,
                                       std::vector<uint32_t> &oFinalIndices,
                                       std::vector<FBXImportSubMesh> &oSubMeshList, bool bIndexFlip,
                                       bool bSkinned) const
{

    // 1 temp vertices 순회하면서 final vertices에 중복되는 vertex가있는지 검사
    // 있다면 temp-final table에 각 vertex의 index를 기록
    // 없다면 해당 vertex를 finalvertices에 추가
    std::vector<FBXImportVertex> finalVertices;
    std::vector<uint32_t> tempIndices;
    std::vector<FBXImportSubMesh> subMeshList;

    //  std::unordered_map<uint32_t, uint32_t> tempFinalVertexIndexTable;

    // material slot - vertex index list table
    std::map<int, std::vector<uint32_t>> matIndexListTable;

    size_t tempVertexCount = tempVertices.size();
    // final vertex, temp-key table 구축
    for (uint32_t tempVertexIndex = 0; tempVertexIndex < tempVertexCount; ++tempVertexIndex)
    {

        const auto &tempVertex = tempVertices[tempVertexIndex];

        auto it = std::find_if(finalVertices.begin(), finalVertices.end(),
                               [&tempVertex, bSkinned](const FBXImportVertex &vertex)
                               {
                                   if (tempVertex.mPosition != vertex.mPosition)
                                       return false;
                                   if (tempVertex.mNormal != vertex.mNormal)
                                       return false;
                                   if (tempVertex.mUV != vertex.mUV)
                                       return false;

                                   if (bSkinned)
                                   {
                                       for (int i = 0; i < 4; ++i)
                                       {
                                           if (tempVertex.mJointIndex0[i] != vertex.mJointIndex0[i])
                                               return false;
                                           if (tempVertex.mJointIndex1[i] != vertex.mJointIndex1[i])
                                               return false;
                                           if (tempVertex.mJointWeight0[i] != vertex.mJointWeight0[i])
                                               return false;
                                           if (tempVertex.mJointWeight1[i] != vertex.mJointWeight1[i])
                                               return false;
                                       }
                                   }

                                   return true;
                               });

        uint32_t Index = 0;

        if (it == finalVertices.end())
        {
            // final vertex 에 추가
            // temp-final table에 기록
            //   tempFinalVertexIndexTable[tempVertexIndex] = finalVertices.size();
            Index = finalVertices.size();
            finalVertices.push_back(tempVertex);
        }
        else
        {
            // temp-final table에 기록만
            Index = it - finalVertices.begin();
            //  tempFinalVertexIndexTable[tempVertexIndex] = it - finalVertices.begin();
        }

        tempIndices.push_back(Index);
        matIndexListTable[tempVertex.mMaterialSlotIndex].push_back(tempVertexIndex);
    }

    std::vector<uint32_t> finalIndices;
    // mat별 index 들을 정렬 (subMesh별)

    int indexOrder[3] = {0, 1, 2};

    /* if (bIndexFlip)
    {
        indexOrder[1] = 2;
        indexOrder[2] = 1;
    }*/

    for (const auto &e : matIndexListTable)
    {

        int matSlotIndex = e.first;
        const std::vector<uint32_t> &indexList = e.second;

        FBXImportSubMesh importSubMesh;
        importSubMesh.mIndexNum = indexList.size();
        importSubMesh.mIndexOffset = finalIndices.size();
        importSubMesh.mMaterialSlotIndex = matSlotIndex;
        importSubMesh.mVertexOffset = 0;

        for (size_t j = 0; j < indexList.size() / 3; ++j)
        {
            finalIndices.push_back(tempIndices[indexList[j * 3 + indexOrder[0]]]);
            finalIndices.push_back(tempIndices[indexList[j * 3 + indexOrder[1]]]);
            finalIndices.push_back(tempIndices[indexList[j * 3 + indexOrder[2]]]);
        }

        subMeshList.push_back(importSubMesh);
    }

    oFinalVertices = std::move(finalVertices);
    oFinalIndices = std::move(finalIndices);
    oSubMeshList = std::move(subMeshList);
}

void FBXImporter::BuildMeshParts(
    FBXImportMesh *pImportMesh, const std::vector<fbxsdk::FbxNode *> &fbxNodes,
    std::unordered_map<fbxsdk::FbxSurfaceMaterial *, FBXMaterialKeyContext> &oFbxMaterialKeyTable) const
{

    // 현재 MeshPart는 subMesh전체영역이라 한개만 존재
    FBXImportMeshPart importMeshPart;
    importMeshPart.mStartSubMeshIndex = 0;
    importMeshPart.mSubMeshCount = pImportMesh->mSubMeshes.size();

    // FbxImportMeshPart Instance 구축
    for (auto pNode : fbxNodes)
    {

        FBXImportMeshPartInstance importMeshPartInstance;
        importMeshPartInstance.mMeshPartIndex = 0;
        // pImportMesh->mMeshParts.size();

        // int materialCount = pNode->GetMaterialCount();

        // 실제 subMesh가 사용하는 material 의 asset key를 meshPartInstance에 등록
        for (const auto &subMesh : pImportMesh->mSubMeshes)
        {
            int matLocalIndex = subMesh.mMaterialSlotIndex;
            if (matLocalIndex == -1)
            {
                oFbxMaterialKeyTable.try_emplace(nullptr, "", false);
                importMeshPartInstance.mSubMeshMaterialKeyList.push_back({"", false});
            }
            else
            {

                fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial = pNode->GetMaterial(matLocalIndex);

                if (fbxSurfaceMaterial)
                {
                    const auto materialIt = oFbxMaterialKeyTable.find(fbxSurfaceMaterial);
                    if (materialIt != oFbxMaterialKeyTable.end())
                        importMeshPartInstance.mSubMeshMaterialKeyList.push_back(materialIt->second);
                }
            }
        }
        pImportMesh->mMeshPartInstances.push_back(importMeshPartInstance);
    }

    pImportMesh->mMeshParts.push_back(importMeshPart);
}

void FBXImporter::CollectTextureDependencyFromProperty(
    fbxsdk::FbxSurfaceMaterial *fbxSurfaceMaterial, const CoreAsset::ImportAssetKey &materialImportKey,
    const std::unordered_map<fbxsdk::FbxTexture *, CoreAsset::ImportAssetKey> &textureImportKeyTable,
    const char *propertyName, CoreAsset::EImportDependencySubInfo subInfo,
    std::vector<CoreAsset::ImportDependencyContext> &oDependencyList) const
{
    if (fbxSurfaceMaterial == nullptr)
        return;

    fbxsdk::FbxProperty fbxProperty = fbxSurfaceMaterial->FindProperty(propertyName);

    if (fbxProperty.IsValid() == false)
        return;

    fbxsdk::FbxTexture *texture = fbxProperty.GetSrcObject<fbxsdk::FbxTexture>();
    const auto textureIt = textureImportKeyTable.find(texture);
    if (textureIt == textureImportKeyTable.end())
        return;

    CoreAsset::ImportDependencyContext importDependencyContext{};

    importDependencyContext.mSubInfo = subInfo;
    importDependencyContext.mOwnerAssetKey = materialImportKey;
    importDependencyContext.mDependencyAssetKey = textureIt->second;
    importDependencyContext.mDependencyType = CoreAsset::EImportDependencyType::eMaterialTexture;
    oDependencyList.push_back(importDependencyContext);
}

void FBXImporter::SetImportPackageOptions(const FBXImportContext &context,
                                          CoreAsset::ImportPackage &oImportPackage) const
{

    oImportPackage.mOption.mNeedToCalculateNormals = context.mNeedToCalculateNormals;
    oImportPackage.mOption.mNeedToCalculateTangents = context.mNeedToCalculateTangents;
}

int FBXImporter::GetPolygonMaterialSlot(fbxsdk::FbxMesh *fbxMesh, int polygonIndex) const
{
    fbxsdk::FbxLayer *layer = fbxMesh->GetLayer(0);
    fbxsdk::FbxLayerElementMaterial *layerElementMaterial = layer->GetMaterials();

    if (layerElementMaterial)
    {
        fbxsdk::FbxLayerElement::EMappingMode mappingMode = layerElementMaterial->GetMappingMode();
        fbxsdk::FbxLayerElement::EReferenceMode referenceMode = layerElementMaterial->GetReferenceMode();

        if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eByPolygon)
        {
            if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eIndexToDirect)
            {

                return layerElementMaterial->GetIndexArray().GetAt(polygonIndex);
            }
            else if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eDirect)
            {
                // 사용한되는 Mode
                return 0;
            }
        }
        else if (mappingMode == fbxsdk::FbxLayerElement::EMappingMode::eAllSame)
        {
            if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eIndexToDirect)
            {
                return layerElementMaterial->GetIndexArray().GetAt(0);
            }
            else if (referenceMode == fbxsdk::FbxLayerElement::EReferenceMode::eDirect)
            {
                return 0;
            }
        }
    }

    return -1;
}

void FBXImporter::GetGeometrix(fbxsdk::FbxNode *node, FbxAMatrix &oMatrix) const
{

    if (node == nullptr)
    {
        return;
        //   return CoreMath::Matrix4X4::Identity;
    }
    fbxsdk::FbxDouble3 geoTranslation = node->GeometricTranslation;
    fbxsdk::FbxDouble3 geoRotation = node->GeometricRotation;
    fbxsdk::FbxDouble3 geoScaling = node->GeometricScaling;

    fbxsdk::FbxAMatrix matrix(geoTranslation, geoRotation, geoScaling);

    oMatrix = matrix;

    //  return matrix;

    // CoreMath::Matrix4X4 ret;
    // Copy(matrix, ret);
    //  return ret;
}

bool FBXImporter::IsSkinningFbxMesh(fbxsdk::FbxMesh *mesh) const
{

    if (mesh == nullptr)
        return false;

    if (mesh->GetDeformerCount(fbxsdk::FbxDeformer::eSkin) > 0)
        return true;

    return false;
}

} // namespace Import
