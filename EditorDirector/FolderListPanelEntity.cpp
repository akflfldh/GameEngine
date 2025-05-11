#include "FolderListPanelEntity.h"
//#include"TextPanelEntity.h"
#include"System.h"
#include"System/FileUiUiSystem.h"

#include"FolderPanelEntity.h"
Quad::FolderListPanelEntity::FolderListPanelEntity(const std::string& name)
	:PanelUiEntity(name)
{
}

Quad::FolderListPanelEntity::~FolderListPanelEntity()
{
}

void Quad::FolderListPanelEntity::Initialize()
{
	
	PanelUiEntity::Initialize();

	

}

Quad::FolderListPanelEntity* Quad::FolderListPanelEntity::Create(Map* map, int mapLayerIndex)
{
	
	FolderListPanelEntity* ob = static_cast<FolderListPanelEntity*>(map->CreateObject("FolderListPanelEntity", mapLayerIndex));
//	ob->Initialize();
	ob->InitCreating();
	return ob;

}

void Quad::FolderListPanelEntity::Update(float deltaTime)
{
	PanelUiEntity::Update(deltaTime);


}

void Quad::FolderListPanelEntity::OnEvent(Event* event)
{
	

}

Quad::FolderPanelEntity * Quad::FolderListPanelEntity::CreateRootFolderPanelEntity(const std::string& folderName)
{
	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
	float width = GetWidth();
	float height = GetHeight();
	DirectX::XMFLOAT3 pos = GetTransform().GetPositionWorld();


	FolderPanelEntity* folderPanelUiEntity = FolderPanelEntity::Create(map, mapLayerIndex, width, mFolderPanelHeight, folderName, this);

	folderPanelUiEntity->SetName(folderName);



	pos.x -= width / 2;
	pos.y += height / 2 - mFolderPanelHeight/2;
	pos.z -= 1;
	folderPanelUiEntity->GetTransform().SetPositionWorld(pos);
	AddChildPanelUiEntity(folderPanelUiEntity);
	mFolderPanelEntityVector.push_back(folderPanelUiEntity);

	mRootFolderPanelEntity = folderPanelUiEntity;

	return folderPanelUiEntity;
	
}

//createRootFolderPanelEntity();

Quad::FolderPanelEntity* Quad::FolderListPanelEntity::CreateFolderPanelEntity(const std::string& folderName, FolderPanelEntity* parentFolderPanelEntity)
{
	if (parentFolderPanelEntity == nullptr)
		return nullptr;

	float width = GetWidth();
	float height = GetHeight();
	Map* map = GetMap();
	int mapLayerIndex = GetMapLayerID();
//	FolderPanelEntity* folderPanelUiEntity = new FolderPanelEntity(folderName);
	//FolderPanelEntity* folderPanelUiEntity = static_cast<FolderPanelEntity*>(map->CreateObject("FolderPanelEntity"));
	FolderPanelEntity* folderPanelUiEntity = FolderPanelEntity::Create(map, mapLayerIndex, width, mFolderPanelHeight, folderName, this);
	folderPanelUiEntity->SetName(folderName);


	parentFolderPanelEntity->IncreaseChildFolderPanelEntityNum();
	int childFolderPanelEntityNum = parentFolderPanelEntity->GetChildFolderPanelEntityNum();


	DirectX::XMFLOAT3 parentFolderPanelPos;
	parentFolderPanelPos = parentFolderPanelEntity->GetTransform().GetPositionWorld();
	//자식Panel의 초기월드위치를 설정한다.
	parentFolderPanelPos.x += mChildPanelShiftX;
	parentFolderPanelPos.y -= (childFolderPanelEntityNum)*mFolderPanelHeight;
	
	
	//folderPanelUiEntity->SetSystem(GetSystem());
	//folderPanelUiEntity->Initialize(width, mFolderPanelHeight, folderName,this);
	folderPanelUiEntity->GetTransform().SetPositionWorld(parentFolderPanelPos);
	parentFolderPanelEntity->AddChildPanelUiEntity(folderPanelUiEntity);

	
	mFolderPanelEntityVector.push_back(folderPanelUiEntity);

	//무조건 부모는 expand상태가된다.
	//그리고 folder 리스트들의 위치를 재조정한다



	if (parentFolderPanelEntity->GetChildFolderExpandFlag() == false)
	{
		parentFolderPanelEntity->SetChildFolderExpandFlag(true);

	}
	else
	{
		ReBuildFolderPanelEntities(parentFolderPanelEntity, true);
	}





	//생성되면 , 위치조정이들어가야한다.
	









	return folderPanelUiEntity;


}

Quad::FolderPanelEntity* Quad::FolderListPanelEntity::CreateFolderPanelEntity(const std::string& initName, const std::wstring& parentFolderPanelEntityName)
{

	return nullptr;



}

const Quad::FolderPanelEntity* Quad::FolderListPanelEntity::GetFolderPanelEntity(const std::string& folderName) const
{
	std::vector<FolderPanelEntity*>::const_iterator ret= std::find_if(mFolderPanelEntityVector.begin(), mFolderPanelEntityVector.end(),
		[&folderName](const FolderPanelEntity* panel) { return panel->GetFolderName() == folderName ? true : false; });


	return ret != mFolderPanelEntityVector.cend() ? *ret : nullptr;

}

Quad::FolderPanelEntity* Quad::FolderListPanelEntity::GetFolderPanelEntity(const std::string& folderName)
{

	std::vector<FolderPanelEntity*>::iterator ret = std::find_if(mFolderPanelEntityVector.begin(), mFolderPanelEntityVector.end(),
		[&folderName](const FolderPanelEntity* panel) { return panel->GetFolderName() == folderName ? true : false; });


	return ret != mFolderPanelEntityVector.cend() ? *ret : nullptr;

}

void Quad::FolderListPanelEntity::ReBuildFolderPanelEntities(FolderPanelEntity * destFolderPanelEntity ,bool folderAddFlag)
{

	//mRootFolderPanelEntity

	//shiftY확인해보자.
	float shiftY = 0;
	std::queue<FolderPanelEntity*> childFolderPanelQueue;

	//expand된 엔티티들만 들언간다
	//그 엔티티들의 자식수 * height 를 shiftY에더한다. 그 자식들중 expand된 자식을 큐에 다시추가한다.
	//destFolder는 expand가 버튼 up을 통해서 false로 변경됬을것이지만, 예외적으로 들어간다.
	
	if (folderAddFlag == true && destFolderPanelEntity->GetChildFolderExpandFlag())
	{
		//추가할떄 펼쳐져있는경우에
		shiftY = mFolderPanelHeight;

	}
	else
	{

		childFolderPanelQueue.push(destFolderPanelEntity);
		while (!childFolderPanelQueue.empty())
		{

			FolderPanelEntity* folderPanel = childFolderPanelQueue.front();
			childFolderPanelQueue.pop();

			const std::vector<UiEntity*>& childFolderPanelVector = folderPanel->GetChildPanelUiEntityVector();
			shiftY += childFolderPanelVector.size() * mFolderPanelHeight;

			std::for_each(childFolderPanelVector.begin(), childFolderPanelVector.end(), [&childFolderPanelQueue](UiEntity* folderPanel)
				{
					if (((FolderPanelEntity*)folderPanel)->GetChildFolderExpandFlag())  childFolderPanelQueue.push((FolderPanelEntity*)folderPanel);
				});

		}

	}
		//펼쳐지는거라면 Y가 내려가야하니 shiftY는 음수가된다.
		if (destFolderPanelEntity->GetChildFolderExpandFlag())
			shiftY *= -1.0F;
	

	//shiftY 계산했다.







	//destParentFolderPanelEntity 에서 이 destFolderPanelEntity의 다음형제들들에대해서 shiftY를 적용시킨다.
	//재귀적으로 destParentFolderPanalEntity를 destfolderPanelEntity로 그리고 그 엔티티의 부모를 destParent로 적용 그리고는 동일하게 다음형제들에대해서shiftY 적용 (여기서는 화면보여지는 리스트순서대로 vector에있다고 가정할것이고,그렇게 강제할것이다)



	while (1)
	{

		FolderPanelEntity* destParentFolderPanelEntity = (FolderPanelEntity*)destFolderPanelEntity->GetParentObject();
		if (destParentFolderPanelEntity == nullptr)
			break;//부모가없는 root까지오면 빠져나간다.

		const std::vector<UiEntity*>& destParentFolderPanelChildEntityVector = destParentFolderPanelEntity->GetChildPanelUiEntityVector();

		bool findDestFolderPanelEntityFlag = false;
		for (int i = 0; i < destParentFolderPanelChildEntityVector.size(); ++i)
		{
			if (findDestFolderPanelEntityFlag)
			{
				//형제엔티티들에해서 이동시킨다.
				destParentFolderPanelChildEntityVector[i]->GetTransform().TranslateLocal({ 0,shiftY,0 });

			}

			if (destParentFolderPanelChildEntityVector[i] == destFolderPanelEntity)
				findDestFolderPanelEntityFlag = true;



		}


		//계층을 올라간다.
		//부모였던 엔티티가 destFolderPanelEntity가되고 
		//그엔티티의부모가 새로운 destParaent가된다.

		destFolderPanelEntity = destParentFolderPanelEntity;
		destParentFolderPanelEntity = (FolderPanelEntity * )destParentFolderPanelEntity->GetParentObject();
	
	}


















}

void Quad::FolderListPanelEntity::InitCreating()
{
	PanelUiEntity::InitCreating();

	SetTexture("DarkGray.png");
}




