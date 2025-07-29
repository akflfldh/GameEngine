#include "UIRenderItemBuilder.h"
#include<UIManager.h>
#include<UICanvas.h>
#include<UIElement.h>
#include<queue>
#include<vector>

#include<IGpuResourceManager.h>
#include<RenderType.h>
#include<Asset/Mesh/MeshType.h>

#include<UIRectTransform.h>

Quad::UIRenderItemBuilder* Quad::UIRenderItemBuilder::mInstance = nullptr;

Quad::UIRenderItemBuilder* Quad::UIRenderItemBuilder::GetInstance()
{
	if (mInstance == nullptr)
	{
		OutputDebugString(L"UI렌더아이템빌더 인스턴스가 생성되지않았습니다.\n");
		assert(0);
	}

	return mInstance;

}


Quad::UIRenderItemBuilder::UIRenderItemBuilder(Render::IRenderSystem* renderSystem, UI::UIManager* uiManager, GRM::IGpuResourceManager* gpuResourceManager)
	:mRenderSystem(renderSystem), mUiManager(uiManager), mGpuResourceManager(gpuResourceManager)
{

	if (mInstance != nullptr)
	{
		OutputDebugString(L"UI렌더아이템빌더 생성자 두번호출하였습니다. 한번만 호출해야합니다.\n");
		assert(0);
	}

	GRM::BufferDesc vertexBufferDesc;

	vertexBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
	vertexBufferDesc.mBufferUsage = GRM::EBufferUsage::eVertexBuffer;
	vertexBufferDesc.mElementDataNum = 4 * 10000;
	vertexBufferDesc.mElementDataSize = sizeof(UIVertex);
	vertexBufferDesc.mBufferSize = 4 * 10000 * vertexBufferDesc.mElementDataSize;
	vertexBufferDesc.mData = nullptr;
	mVertexBufferGpu =	mGpuResourceManager->CreateBuffer(vertexBufferDesc);


	GRM::BufferDesc indexBufferDesc;

	indexBufferDesc.mBufferMemoryAccess = GRM::EBufferMemoryAccess::eCpuWriteOnly;
	indexBufferDesc.mBufferUsage = GRM::EBufferUsage::eIndexBuffer;
	indexBufferDesc.mElementDataNum = 6 * 10000;
	indexBufferDesc.mElementDataSize = sizeof(uint32_t);
	indexBufferDesc.mBufferSize = 6 * 10000 * indexBufferDesc.mElementDataSize;
	indexBufferDesc.mData = nullptr;
	mIndexBufferGpu = mGpuResourceManager->CreateBuffer(indexBufferDesc);


	mInstance = this;

}

Quad::UIRenderItemBuilder::~UIRenderItemBuilder()
{
}

void Quad::UIRenderItemBuilder::SubmitUIChannel(const UI::UIChannelContext& uiChannelContext, Render::RenderChannelID renderChannelID
)
{

	//먼저 uiElement들을 쭉 저장한다.


	UI::UICanvas* canvas = mUiManager->GetCanvas(uiChannelContext.mCanvasID);
	const std::vector<UI::UIElement*> childUIElementVector = canvas->GetChildUIElementAll();
	std::queue<UI::UIElement*> uiElementQueue;

	for (auto uiElement : childUIElementVector)
	{
		uiElementQueue.push(uiElement);
	}

	while (!uiElementQueue.empty())
	{
		UI::UIElement* uiElement = uiElementQueue.front();
		uiElementQueue.pop();

		mUIElementContextVectorPerRenderChannel[renderChannelID].emplace_back(uiElement, UIElementBatchKey{ renderChannelID ,uiElement->GetMaterialID(),{0,0,0,0} });

		for (auto child : uiElement->GetChildVector())
		{
			uiElementQueue.push(child);
		}
	}







}

void Quad::UIRenderItemBuilder::SubmitRenderItemAll()
{


	//모든 렌더채널들의 UIElement들에대해서 
	//각 채널별로 정렬을 수행한다.


	//MaterialID , scissorRect 이 정렬기준

	

	for (auto& uiElementVectorPerRenderChannel : mUIElementContextVectorPerRenderChannel)
	{
		Render::RenderChannelID renderChannelID =	uiElementVectorPerRenderChannel.first;
		std::vector<UIElementContext>& uiElementContextVector = uiElementVectorPerRenderChannel.second;

		
		std::sort(uiElementContextVector.begin(), uiElementContextVector.end(), [](const UIElementContext & a, const UIElementContext & b) {
			
			return a.mBatchKey < b.mBatchKey;
			});
	}



	//이제는 batchKey가 바뀔때마다 렌더아이템을 새로생성해서 그 렌더아이템으로 값을 채운다
	//그리고 renderSystem으로 제출



	size_t indexOffset = 0;// 전체 렌더아이템들의 indexOffset
	size_t vertexOffset = 0;//
	size_t renderItemIndexOffset = 0;//하나의 renderItem내에서 ui들의 인덱스offset
	for (auto& uiElementVectorPerRenderChannel : mUIElementContextVectorPerRenderChannel)
	{
		renderItemIndexOffset = 0;
		std::vector<Render::RenderItem> renderItemVector;

		Render::RenderChannelID renderChannelID = uiElementVectorPerRenderChannel.first;
		std::vector<UIElementContext>& uiElementContextVector = uiElementVectorPerRenderChannel.second;


		//첫번째 원소
		if (uiElementContextVector.size() == 0)
			continue;

		renderItemVector.emplace_back();
		InitRenderItem(renderItemVector.back(),vertexOffset,indexOffset, uiElementContextVector[0]);

		indexOffset += 6;
		vertexOffset += 4;

		for (int i = 1; i < uiElementContextVector.size(); ++i)
		{
			
			if (uiElementContextVector[i - 1].mBatchKey == uiElementContextVector[i].mBatchKey)
			{
				//batchKey가 같다면 기존렌더아이템으로 추가
				renderItemIndexOffset++;
			}
			else
			{
				//batchKey가 다르다면  새로운 렌더아이템생성
				renderItemVector.emplace_back();
				//renderItem 초기셋팅
				InitRenderItem(renderItemVector.back(), vertexOffset, indexOffset, uiElementContextVector[i]);
				renderItemIndexOffset = 0;

			}


			//vertex추가 //내부에서 렌더아이템의 vertexNum을 4증가시켜준다
			AddVertexToBatchedVector(renderItemVector.back(), uiElementContextVector[i].mUIElement);

			//index 추가 //내부에서 렌더아이템의 indexNum를 6증가시켜준다.
			AddIndexToBatchedVector(renderItemVector.back(), renderItemIndexOffset);

			indexOffset += 6;
			vertexOffset += 4;
		}

		//렌더시스템내에서 렌더아이템VECTOR를 복사함으로 걱정없다.
		mRenderSystem->SubmitRenderItems(renderChannelID, renderItemVector);

	}

	//buffer업로드
	mGpuResourceManager->UploadBufferData(mVertexBufferGpu, mBatchedVertexVector.data(), sizeof(UIVertex),
		mBatchedVertexVector.size());

	mGpuResourceManager->UploadBufferData(mIndexBufferGpu, mBatchedIndexVector.data(), sizeof(uint32_t),
		mBatchedIndexVector.size());




	mBatchedIndexVector.clear();
	mBatchedVertexVector.clear();
	mUIElementContextVectorPerRenderChannel.clear();





}

void Quad::UIRenderItemBuilder::AddVertexToBatchedVector(Render::RenderItem& oRenderItem,
	UI::UIElement* uiElement)
{

	oRenderItem.mMeshItem.mVertexNum += 4;


	std::vector<CoreMath::Vector3> pointPosVector;

	uiElement->GetRectTransform()->GetRectPointWorld(pointPosVector);

	std::vector<UIVertex> vertexVector(4);
	vertexVector[0].mPos = pointPosVector[0];
	vertexVector[1].mPos = pointPosVector[1];
	vertexVector[2].mPos = pointPosVector[2];
	vertexVector[3].mPos = pointPosVector[3];

	vertexVector[0].mTex.X = 0;
	vertexVector[0].mTex.Y = 0;

	vertexVector[1].mTex.X = 1;
	vertexVector[1].mTex.Y = 0;

	vertexVector[2].mTex.X = 0;
	vertexVector[2].mTex.Y = 1;

	vertexVector[3].mTex.X = 1;
	vertexVector[3].mTex.Y = 1;


	//vertex4개가 추가되어야한다.
	mBatchedVertexVector.insert(mBatchedVertexVector.end(), vertexVector.begin(), vertexVector.end());


	//최종하나의 버텍스버퍼로 옮기기전에 개별적으로 버텍스들을 저장할 벡터가필요하다.

	

}

void Quad::UIRenderItemBuilder::AddIndexToBatchedVector(Render::RenderItem& oRenderItem, uint32_t indexOffset)
{
	//또한 한 렌더아이템내의 각각의 ui들의 인덱스값들이 올바른 정점들의 위치를 가리키도록 
	//012 132 에 4*renderItemindex(즉 그 렌더아이템에 존재하는 ui개수(자신을제외한))를 곱한수를 더해준다.
	std::vector<uint32_t> indexVector(6, 4*indexOffset);
	indexVector[0] += 0;
	indexVector[1] += 1;
	indexVector[2] += 2;
	indexVector[3] += 1;
	indexVector[4] += 3;
	indexVector[5] += 2;



	mBatchedIndexVector.insert(mBatchedIndexVector.end(), indexVector.begin(), indexVector.end());



}

void Quad::UIRenderItemBuilder::InitRenderItem(Render::RenderItem& oRenderItem,size_t vertexOffset, size_t indexOffset,
	const UIElementContext & uiElementContext)
{
	oRenderItem.mMaterialID = uiElementContext.mBatchKey.mMaterialID;
	oRenderItem.mMeshItem.mIndexNum = 6;
	oRenderItem.mMeshItem.mIndexOffset = indexOffset;
	oRenderItem.mMeshItem.mVertexNum = 4;
	oRenderItem.mMeshItem.mVertexOffset = vertexOffset;
	oRenderItem.mMeshItem.mVertexBuffer = mVertexBufferGpu;
	oRenderItem.mMeshItem.mIndexBuffer = mIndexBufferGpu;


}
