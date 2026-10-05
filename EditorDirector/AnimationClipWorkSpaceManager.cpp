#include "AnimationClipWorkSpaceManager.h"
#include <EditorDirector/EditorUIUtility.h>

#include <Core/AnimatorComponent.h>
#include <Core/CameraComponent.h>
#include <Core/CameraObject.h>
#include <Core/Entity.h>
#include <Core/LogicalWindow.h>
#include <Core/Map.h>
#include <Core/SkeletalMeshComponent.h>
#include <Core/WorkSpace.h>
#include <Core/World.h>
#include <CoreAsset/AnimationClip.h>
#include <CoreAsset/AssetManager.h>
#include <CoreAsset/GlobalAssetRegistrySystem.h>
#include <CoreAsset/Skeleton.h>
#include <CoreAsset/SkinningMesh.h>
#include <EditorDirector/EditorDirector.h>
#include <EditorDirector/EditorEditMode.h>
#include <EditorDirector/EditorSceneController.h>
#include <EditorDirector/EditorSceneManager.h>
#include <UiSystem/UIButton.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIManager.h>
#include <vector>

AnimationClipWorkSpaceManager *AnimationClipWorkSpaceManager::GetInstance()
{
    static AnimationClipWorkSpaceManager instance;
    return &instance;
}

void AnimationClipWorkSpaceManager::Initialize(Core::LogicalWindow *globalLogicalWindow, const UI::UITheme &uiTheme)
{
    auto *uiManager = UI::UIManager::GetInstance();
    const UI::UICanvasID canvasID = uiManager->CreateCanvas("AnimationClipCanvas", UI::ECanvasSizeMode::eFixSize);
    auto *canvas = uiManager->GetCanvas(canvasID);
    canvas->SetTheme(uiTheme);

    mWorkSpace = std::make_unique<Core::WorkSpace>();
    InitLogicalWindow(canvas);
    mWorkSpace->AddLogicalWindow(globalLogicalWindow);
    mWorkSpace->SetGlobalOverlayWindow(globalLogicalWindow);

    // 선택 시스템 없이 카메라와 미리보기 객체를 준비한다. 클립 재생 시간 제어는 후속 단계에서 연결한다.

    mWorld = std::make_unique<World>();

    auto engineMode = new EditorEditMode;
    engineMode->GetTransformGizmo().SetComponentControlState(true);

    auto map = new Map;
    map->SetName("PlayMap");
    mWorld->Register(engineMode->GetEditorMap());
    mWorld->SetEngineMode(engineMode);

    map->SetAmbientLightColor({1.0f, 1.0f, 1.0f});
    map->SetAmbientLightIntensity(1.3f);
    mLogicalWindow->SetWorld(mWorld.get());
    Quad::EditorSceneManager::GetInstance()->RegisterWorld("AnimationClipEditWorld", mWorld.get());
    mWorld->SetActiveState(false);

    mWorld->Register(map);
    mWorld->SetCurrentMap(map);

    auto *camera = map->CreateEngineEntity<CameraObject>("AnimationPreviewCamera");
    if (camera)
    {
        CameraComponent *cameraComponent = camera->GetCameraComponent();
        cameraComponent->SetPositionLocal({0.0f, 0.0f, -10.0f});
        cameraComponent->SetFar(10025.5f);
        // World의 활성 카메라는 Map의 인덱스가 아니라 EditorEditMode를 통해 조회된다.
        engineMode->SetEditorCameraComponent(cameraComponent);

        auto editorCameraController =
            mWorld->GetCurrentMap()->CreateEngineEntity<Quad::EditorSceneController>("SceneController");
        if (editorCameraController && camera)
        {
            editorCameraController->Possess(camera);
        }
        editorCameraController->Intialize(nullptr);
        engineMode->SetEditorController(editorCameraController);
    }

    mAnimClipTargetEntity = mWorld->CreateEntity<Entity>("AnimClipTargetEntity");
    SkeletalMeshComponent *skeletalMeshCom = static_cast<SkeletalMeshComponent *>(
        mAnimClipTargetEntity->CreateComponent<SkeletalMeshComponent>("SkeletalMeshCom"));

    if (skeletalMeshCom)
        skeletalMeshCom->SetParent(mAnimClipTargetEntity->GetRootComponent());
    auto *animatorCom =
        static_cast<AnimatorComponent *>(mAnimClipTargetEntity->CreateComponent<AnimatorComponent>("AnimCom"));
    if (animatorCom != nullptr)
        animatorCom->SetManuallyDriven(true);

    mWorld->BeginMap();

    InitUI(canvas);
}

Core::WorkSpace *AnimationClipWorkSpaceManager::GetWorkSpace() const
{
    return mWorkSpace.get();
}

void AnimationClipWorkSpaceManager::SetClip(const CoreAsset::AnimationClip &clip, const CoreAsset::Skeleton &skeleton)
{
    // 전환 경계에서 호환성 검사를 마친 Skeleton만 적용한다. 프리뷰의 Animator는 에셋을 소유하지 않는다.
    if (mAnimClipTargetEntity == nullptr)
        return;

    AnimatorComponent *animator = mAnimClipTargetEntity->GetComponent<AnimatorComponent>();
    SkeletalMeshComponent *skeletalMesh = mAnimClipTargetEntity->GetComponent<SkeletalMeshComponent>();
    if (animator == nullptr || skeletalMesh == nullptr)
        return;

    CoreAsset::AssetPtr skeletonAsset =
        CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::Skeleton>(skeleton.GetID());
    if (skeletonAsset.Get() == nullptr)
        return;

    if (mPreviewStarted)
        animator->SetClipPaused(true);
    animator->BindSkeleton(skeletonAsset);
    animator->PlayClip(clip.GetID(), Core::EAnimationReplayPolicy::eRestart);

    // Clip에는 Mesh ID가 없으므로 같은 Skeleton에 유효하게 바인딩된 메시가 하나일 때만 자동 선택한다.
    // 이전 클립의 메시가 새 Skeleton에 잘못 표시되지 않도록 후보가 없거나 복수이면 비운다.
    std::vector<CoreAsset::Asset *> meshAssets;
    CoreAsset::GlobalAssetRegistrySystem::GetInstance()->GetAssetsByType(CoreAsset::EAssetType::eSkinningMesh,
                                                                         meshAssets);
    CoreAsset::SkinningMesh *previewMesh = nullptr;
    for (CoreAsset::Asset *asset : meshAssets)
    {
        auto *candidate = static_cast<CoreAsset::SkinningMesh *>(asset);
        if (!candidate->GetSkinBinding().Validate(skeleton))
            continue;

        if (previewMesh != nullptr)
        {
            previewMesh = nullptr;
            break;
        }
        previewMesh = candidate;
    }
    skeletalMesh->SetMesh(previewMesh);

    mTargetClipAssetID = clip.GetID();
    mPreviewStarted = false;
    mPreviewPlaying = false;

    mEditUIController.ResetAnimTrackUI();
    mEditUIController.SetClipDuration(clip.GetDurationSeconds());
    mEditUIController.SetPlayButtonState(false, false);
    mEditUIController.SetSkeleton(skeleton);
    mEditUIController.SetClipLoop(clip.GetLoop());
}

void AnimationClipWorkSpaceManager::TogglePlayback()
{
    if (mAnimClipTargetEntity == nullptr || mTargetClipAssetID == NoneAssetID)
        return;

    AnimatorComponent *animator = mAnimClipTargetEntity->GetComponent<AnimatorComponent>();
    if (animator == nullptr)
        return;

    if (!mPreviewStarted)
    {
        CoreAsset::AssetPtr clip =
            CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationClip>(mTargetClipAssetID);
        if (clip.Get() == nullptr || !animator->PlayClip(clip, Core::EAnimationReplayPolicy::eRestart))
            return;

        mPreviewStarted = true;
        mPreviewPlaying = true;
        // 첫 프레임도 즉시 샘플링해 버튼을 누른 직후 bind pose가 잠시 남지 않도록 한다.
        animator->AdvanceClip(0.0f);
        mAnimClipTargetEntity->EndUpdate(0.0f);

        mEditUIController.OnStartAnimClip();
    }
    else
    {
        const bool nextPlaying = !mPreviewPlaying;
        if (!animator->SetClipPaused(!nextPlaying))
            return;
        mPreviewPlaying = nextPlaying;

        mEditUIController.OnPauseAnimClip();
    }

    mEditUIController.SetPlayButtonState(mPreviewStarted, mPreviewPlaying);
}

void AnimationClipWorkSpaceManager::SplitClip(float startTime, float endTime)
{

    CoreAsset::AssetPtr pClip =
        CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationClip>(mTargetClipAssetID);
    if (pClip.Get() == nullptr)
        return;

    CoreAsset::AnimationClip *clip = pClip.As<CoreAsset::AnimationClip>();
    float sampleIntervalSeconds = clip->GetSampleIntervalSeconds(); // ex 1/30
    int clipEndFrameIndex = clip->GetSampleCount() - 1;

    int startFrameIndex = std::clamp((int)std::round(startTime / sampleIntervalSeconds), 0, clipEndFrameIndex);
    int endFrameIndex = std::clamp((int)std::round(endTime / sampleIntervalSeconds), 0, clipEndFrameIndex);

    if (startFrameIndex >= endFrameIndex)
    {
        return;
    }

    // 분할
    std::string failReason;
    clip->TrimFrameRange(startFrameIndex, endFrameIndex, &failReason);

    CoreAsset::AssetPtr pSkeleton = clip->GetSkeletonAssetID();

    SetClip(*clip, *pSkeleton.As<CoreAsset::Skeleton>());
    return;
}

void AnimationClipWorkSpaceManager::Update(float deltaTime)
{

    if (mWorld == nullptr || !mWorld->GetActiveState() || mAnimClipTargetEntity == nullptr)
        return;

    AnimatorComponent *animator = mAnimClipTargetEntity->GetComponent<AnimatorComponent>();

    if (mSeekPending)
    {
        animator->SetClipTime(mTargetTimeSeconds);
        mSeekPending = false;
        mAnimClipTargetEntity->EndUpdate(deltaTime);
        return;
    }

    if (!mPreviewPlaying)
        return;

    if (!animator)
    {
        mPreviewStarted = false;
        mPreviewPlaying = false;
        mEditUIController.SetPlayButtonState(false, false);
        return;
    }

    if (animator->AdvanceClip(deltaTime))
    {

        mEditUIController.OnPlayingAnimClip(deltaTime);
    }
    else
    {
        mPreviewStarted = false;
        mPreviewPlaying = false;
        mEditUIController.SetPlayButtonState(false, false);
        return;
    }

    // EditorEditMode는 일반 Entity의 EndUpdate를 호출하지 않으므로 미리보기 palette만 여기서 반영한다.
    mAnimClipTargetEntity->EndUpdate(deltaTime);

    if (animator->IsClipFinished())
    {
        mPreviewStarted = false;
        mPreviewPlaying = false;
        mEditUIController.SetPlayButtonState(false, false);
    }
}

void AnimationClipWorkSpaceManager::OnWorkSpaceActive()
{

    if (mWorld)
    {
        mWorld->SetActiveState(true);
    }
}

void AnimationClipWorkSpaceManager::OnWorkSpaceInActive()
{
    if (mWorld)
    {
        mWorld->SetActiveState(false);
    }
}

void AnimationClipWorkSpaceManager::InitLogicalWindow(UI::UICanvas *canvas)
{
    mLogicalWindow = std::make_unique<Core::LogicalWindow>();
    auto configureFullViewport = [](Core::ViewportController &viewport)
    {
        viewport.SetViewportMode(Core::EViewportMode::eAnchored);
        viewport.SetAnchorLeftState(true);
        viewport.SetAnchorLeftMode(Core::EViewportAnchoredMode::eRelative);
        viewport.SetAnchorLeftRelValue(0.0f);
        viewport.SetAnchorRightState(true);
        viewport.SetAnchorRightMode(Core::EViewportAnchoredMode::eRelative);
        viewport.SetAnchorRightRelValue(0.0f);
        viewport.SetAnchorTopState(true);
        viewport.SetAnchorTopMode(Core::EViewportAnchoredMode::eRelative);
        viewport.SetAnchorTopRelValue(0.0f);
        viewport.SetAnchorBottomState(true);
        viewport.SetAnchorBottomMode(Core::EViewportAnchoredMode::eRelative);
        viewport.SetAnchorBottomRelValue(0.0f);
    };

    configureFullViewport(mLogicalWindow->mViewportController);
    // World가 없는 1차 단계에서도 렌더 경로는 3D viewport 값을 읽으므로 유효한 전체 영역을 지정한다.
    configureFullViewport(mLogicalWindow->m3DWorldViewportController);

    mLogicalWindow->SetActiveCanvas(canvas);
    mLogicalWindow->SetBackBufferClearColor(0.25f, 0.25f, 0.28f, 1.0f);
    mWorkSpace->AddLogicalWindow(mLogicalWindow.get());
}

void AnimationClipWorkSpaceManager::InitUI(UI::UICanvas *canvas)
{
    auto *toolbar = EditorUIUtility::Create<UI::UIImage>(canvas, "AnimationClipToolbar");
    toolbar->SetSize(3000.0f, mToolbarHeight);
    toolbar->SetPositionLocal(0.0f, 0.0f);
    toolbar->SetColor(UI::UIColor::DarkGray);

    auto *backButton = EditorUIUtility::CreateSmallButton(toolbar, "ToDefaultEditButton");
    // EditorUIUtility의 기본 높이 유지: backButton->SetSize(80.0f, 40.0f);
    backButton->SetWidth(80.0f);
    backButton->SetPositionLocal(5.0f, 5.0f);
    backButton->mUIImageComponent->UseTexture();
    backButton->mUIImageComponent->SetTexture("Engine/ArrowLeft");
    backButton->mUIButtonComponent->mButtonClickCallbackSystem.Register(
        [](float, float) { Quad::EditorDirector::GetInstance()->ChangeToDefaultEditWorkSpace(); });

    mEditUIController.Initialize(canvas, mToolbarHeight);
    mEditUIController.SetPlayButtonCallback([this]() { TogglePlayback(); });
    mEditUIController.SetClipLoopChangedCallback(
        [this](bool loop)
        {
            // 패널을 재사용하므로 콜백 등록 당시의 포인터 대신 현재 편집 대상 ID로 조회한다.
            CoreAsset::AssetPtr clipAsset =
                CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationClip>(mTargetClipAssetID);
            auto clip = clipAsset.As<CoreAsset::AnimationClip>();
            if (!clip || clip->GetLoop() == loop)
                return;

            clip->SetLoop(loop);
            clip->SetDirty();
            // 실행 중인 slot의 loop는 PlayClip 시 복사된다. 설정 변경만으로 미리보기를 재시작하지 않는다.
        });

    mEditUIController.SetAnimTrackStickMoveCallback(
        [this](float targetPos)
        {
            mTargetTimeSeconds = 0.0f;
            mSeekPending = false;
            AnimatorComponent *animator = mAnimClipTargetEntity->GetComponent<AnimatorComponent>();
            if (animator == nullptr)
                return;
            CoreAsset::AssetPtr clip =
                CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationClip>(mTargetClipAssetID);

            if (clip.Get() == nullptr)
                return;

            mTargetTimeSeconds = targetPos * clip.As<CoreAsset::AnimationClip>()->GetDurationSeconds();
            mSeekPending = true;
        });

    mEditUIController.SetClipSplitButtonCallback(
        [this](float normalizedStartPos, float normalizedEndPos)
        {
            AnimatorComponent *animator = mAnimClipTargetEntity->GetComponent<AnimatorComponent>();
            if (animator == nullptr)
                return;
            CoreAsset::AssetPtr clip =
                CoreAsset::AssetManager::GetInstance()->GetAsset<CoreAsset::AnimationClip>(mTargetClipAssetID);

            if (clip.Get() == nullptr)
                return;

            float startTimeSeconds = normalizedStartPos * clip.As<CoreAsset::AnimationClip>()->GetDurationSeconds();
            float endTimeSeconds = normalizedEndPos * clip.As<CoreAsset::AnimationClip>()->GetDurationSeconds();

            SplitClip(startTimeSeconds, endTimeSeconds);
            // 근데 새로운 에셋 생성보단 이 클립의 영역만 분할하는게 더 편하긴해 - > 더 복잡한 작업이 없음 .
            //
            // 원본에셋은 그냥 복사하는 기능만 만들어놓으면
        });

    // 재생하고있으면 저 stick을 같이 이동시켜줘야해
}
