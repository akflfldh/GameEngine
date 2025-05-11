#include "InstancingTable.h"
#include"ResourceController.h"
#include"Shader/ShaderResourceType.h"
#include"Object/Entity.h"


namespace Quad
{
	std::unordered_map<std::wstring, std::pair<InstancingData*, StructuredBuffer*>>InstancingTable::mInstancingTable;


	void Quad::InstancingTable::RegisterEntityInstance(const std::wstring& groupName, Entity* entity)
	{
		std::unordered_map<std::wstring, std::pair<InstancingData*, StructuredBuffer*>>::iterator it =
			mInstancingTable.find(groupName);
		//if (it == mInstancingTable.end())
		//{
		//	StructuredBuffer* buffer = ResourceController::CreateStructuredBuffer(sizeof(EntityConstant), 1000);

		//	InstancingData *data=new InstancingData;
		//	data->mOriginalMesh = entity->GetModel().GetMesh();
		//	data->mSubMesh = data->mOriginalMesh->GetSubMeshVector();
		//	//메시는모두동일하다.
		//	//머터리얼은 각 인스턴스마다 다르고,
		//	//텍스처는 그룹별로 다르다.

		//	//그룹이 만들어질때 초기에는 맨처음 엔티티의 텍스처로 설정한다.
		//	for (int i = 0; i < data->mSubMesh.size(); ++i)
		//	{
		//		data->mSubMesh[i].mMateiral->SetDiffuseMap(entity->GetModel().GetSubMesh()[i].mMateiral->GetTexture());
		//	}
		//	mInstancingTable.insert({ groupName,{data,buffer} });
		//}

		////entity->SetInstanceGroup();
		//entity->SetInstancingData(true, groupName);






	}


	bool InstancingTable::GetInstanceDataAndStructuredBuffer(const std::wstring& groupName, 
		std::pair<InstancingData*, StructuredBuffer*> & oData)
	{
		std::unordered_map<std::wstring,std::pair<InstancingData * ,StructuredBuffer *>>::iterator it=
			mInstancingTable.find(groupName);

		if (it != mInstancingTable.end())
		{
			oData.first = it->second.first;
			oData.second = it->second.second;
			return true;
		}
		return false;

		
	}
}