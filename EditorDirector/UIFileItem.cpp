#include "UIFileItem.h"
#include <CoreBase/CallbackSystem.h>
#include <EditorDirector/UIDragSourceComponent.h>
#include <EditorDirector/UISelectableComponent.h>
#include <LogicalFileSystem/LogicalFile.h>
#include <LogicalFileSystem/LogicalNode.h>
#include <UiSystem/UIButtonComponent.h>
#include <UiSystem/UICanvas.h>
#include <UiSystem/UIDoubleClickComponent.h>
#include <UiSystem/UIEditBox.h>
#include <UiSystem/UIElementPtr.h>
#include <UiSystem/UIImage.h>
#include <UiSystem/UIImageComponent.h>
#include <UiSystem/UIText.h>
#include <UiSystem/UITextComponent.h>
UIFileItem::UIFileItem()
    : mFileImageElement(nullptr), mFileTextElement(nullptr), mFileNode(nullptr), mFileTextButtonComponent(nullptr),
      mDragDropCom(nullptr), mSelectableCom(nullptr), mSelectOverlayImageCom(nullptr)
{
    SetUseScissorRect(true);

    mBackImageCom = CreateUIComponent<UI::UIImageComponent>("BackImageCom");
    mBackImageCom->SetColor(0.2f, 0.2f, 0.2f);

    mDoubleClickCom = CreateUIComponent<UI::UIDoubleClickComponent>("DoubleClickCom");

    mDragDropCom = CreateUIComponent<UIDragSourceComponent>("DragDropCom");

    mSelectOverlayImageCom = CreateUIComponent<UI::UIImageComponent>("SelectOverlayCom");
    mSelectOverlayImageCom->SetColor(UI::UIColor{0.0F, 0.0F, 0.7F, 0.5f});
    mSelectOverlayImageCom->SetActiveState(false);
    mSelectOverlayImageCom->SetDepthValue(0);

    mFileTextWidthRatio = 0.9f;
}

UIFileItem::~UIFileItem() {}

void UIFileItem::OnBegin()
{
    mDoubleClickCom->mOnDoubleClickedCallbackSystem.Register(this, &UIFileItem::HandleDoubleClick);

    auto canvas = GetDestCanvas();

    mFileImageElement = canvas->CreateUIElement<UI::UIImage>("FileImageElement");
    mFileImageElement->mImageCom->UseTexture();
    mFileImageElement->mImageCom->SetTexture("Engine/DefaultGray");

    mFileImageElement->SetParent(this);
    mFileImageElement->SetPositionLocal(10, 10); // padding;
    mFileImageElement->SetUseScissorRect(true);
    mFileImageElement->SetOnlyVisible(true);
    // SetFileImageSizeRatio(mFileImageWidthRatio, mFileImageHeightRatio);

    mFileTextElement = CreateChildUIElement<UI::UIText>("FileTextElement");
    mFileTextElement->SetPositionLocal(0, 0);
    mFileTextElement->SetFontSize(20.0f);
    mFileTextElement->SetUseScissorRect(true);
    mFileTextElement->SetOnlyVisible(true);
    mFileTextElement->GetTextComponent()->SetOverflowMode(UI::EUITextOverflowMode::eEllipsis);

    OnTransformChanged(UI::ETransformChangeType::eSize);

    mFileTextButtonComponent = mFileTextElement->CreateUIComponent<UI::UIButtonComponent>("ButtonCom");
    mSelectableCom = CreateUIComponent<UISelectableComponent>("SelectableCom");
    mSelectableCom->mOnSelectedCallbackSystem.Register(this, &UIFileItem::OnSelected);
}

void UIFileItem::SetLogicalFileNode(QuadLF::LogicalNode *node)
{
    mFileNode = node;

    mFileTextElement->SetText(node->GetName());

    if (node->GetNodeType() == QuadLF::ELogicalNodeType::eFolder)
    {

        mFileImageElement->mImageCom->SetTexture("Engine/Folder");
    }
    else
    {
        QuadLF::LogicalFile *fileNode = static_cast<QuadLF::LogicalFile *>(node);

        const QuadLF::LogicalFileAssetInfo &assetInfo = fileNode->GetAssetInfo();

        DragPayload payload;

        switch (assetInfo.mAssetType)
        {
        case CoreAsset::EAssetType::eTexture:
        {
            payload.mType = EDragDropType::eAssetTexture;
            mFileImageElement->mImageCom->SetTexture(assetInfo.mAssetID);
        }
        break;
        case CoreAsset::EAssetType::eMaterial:
        {
            payload.mType = EDragDropType::eAssetMaterial;
            mFileImageElement->mImageCom->SetTexture("Engine/MaterialTexture");
        }
        break;
        case CoreAsset::EAssetType::eStaticMesh:
        {
            payload.mType = EDragDropType::eAssetStaticMesh;
            mFileImageElement->mImageCom->SetTexture("Engine/MeshTexture");
        }
        break;
        case CoreAsset::EAssetType::eCXX:
        {
            payload.mType = EDragDropType::eObject;
            mFileImageElement->mImageCom->SetTexture("Engine/CXXTexture");
        }
        break;
        case CoreAsset::EAssetType::ePrefab:
        {
            payload.mType = EDragDropType::ePrefab;
            mFileImageElement->mImageCom->SetTexture("Engine/Prefab");
        }
        break;
        }
        payload.mAssetID = assetInfo.mAssetID;
        mDragDropCom->SetPayload(payload);
    }
}

void UIFileItem::SetFileImageSizeRatio(float w, float h)
{

    mFileImageWidthRatio = std::min(1.0f, std::max(0.0f, w));
    mFileImageHeightRatio = std::min(1.0f, std::max(0.0f, h));

    float itemWidth = mTransform.GetSize().r;
    float itemHeight = mTransform.GetSize().g;

    mFileImageElement->SetSize(itemWidth * mFileImageWidthRatio, itemHeight * mFileImageHeightRatio);
}

void UIFileItem::SetFileTextSizeRatio(float w, float h)
{

    mFileTextWidthRatio = std::min(1.0f, std::max(0.0f, w));
    mFileTextHeightRatio = std::min(1.0f, std::max(0.0f, h));

    float itemWidth = mTransform.GetSize().r;
    float itemHeight = mTransform.GetSize().g;

    mFileTextElement->SetSize(itemWidth * mFileTextWidthRatio, itemHeight * mFileTextHeightRatio);
}

void UIFileItem::SetFileTextPosRatio(float x, float y)
{

    mFileTextPosXRatio = std::min(1.0f, std::max(0.0f, x));
    mFileTextPosYRatio = std::min(1.0f, std::max(0.0f, y));

    float itemWidth = mTransform.GetSize().r;
    float itemHeight = mTransform.GetSize().g;

    mFileTextElement->SetPositionLocal(itemWidth * mFileTextPosXRatio, itemHeight * mFileTextPosYRatio);
}

void UIFileItem::OnTransformChanged(UI::ETransformChangeType type)
{

    SetFileImageSizeRatio(mFileImageWidthRatio, mFileImageHeightRatio);
    SetFileTextSizeRatio(mFileTextWidthRatio, mFileTextHeightRatio);
    SetFileTextPosRatio(mFileTextPosXRatio, mFileTextPosYRatio);
}

UISelectableComponent *UIFileItem::GetSelectableComponent() const
{
    return mSelectableCom;
}

void UIFileItem::HandleDoubleClick()
{
    mOnFileOpendCallbackSystem.ExecuteCallbacks(mFileNode);
}

void UIFileItem::OnSelected(bool state)
{

    if (state)
    {
        //  mFileImageElement->SetOnlyVisible(false);
        mFileTextElement->SetOnlyVisible(false);

        mSelectOverlayImageCom->SetActiveState(true);
    }
    else
    {
        //    mFileImageElement->SetOnlyVisible(true);
        mFileTextElement->SetOnlyVisible(true);

        mSelectOverlayImageCom->SetActiveState(false);
    }
}
