//#include "WorldEditEntityFactory.h"
//#include"ReaderWriter.h"
//#include"WorldEditUiEntity.h"
//#include"RawResourceType.h"
//#include"MeshManager.h"
//#include"TextureManager.h"
//
//namespace Quad
//{
//	ReaderWriter WorldEditEntityFactory::mTableSynObject;
//	std::unordered_map<std::wstring, RawSchemaData> WorldEditEntityFactory::mEntitySchemaTable;
//	
//
//	bool WorldEditEntityFactory::RegisterNewEntityClass(RawSchemaData& rawSchemaData)
//	{
//		std::pair<std::unordered_map<std::string,RawSchemaData>::iterator , bool> ret
//			=mEntitySchemaTable.insert({ rawSchemaData.mClassId, rawSchemaData });
//		
//		return ret.second;
//
//	}
//
//	Object* WorldEditEntityFactory::GetEntityClass(const std::string& entityName, const std::wstring& classId)
//	{
//		std::unordered_map<std::wstring, RawSchemaData>::iterator it = mEntitySchemaTable.find(classId);
//		if (it != mEntitySchemaTable.end())
//		{
//			////if (it->second.mType == L"UiEntity")
//			//{
//			//	RawSchemaData& schemaData = it->second;
//			//	WorldEditUiEntity* entity = new WorldEditUiEntity(entityName);
//			//	//init option값도 들어있어야하는데
//			//	//스키마파일에 집어넣어야한다.
//			//	// 
//			//	Mesh * mesh = MeshManager::GetMesh(schemaData.mMesh);	//메시를가져오는데 없다면
//			//	//기본메시를 리턴하는거다 이것도 핸들로처리하면 댕글링포인터를 방지할수있으니 좋겠다.
//			//	entity->Initialize();
//
//			//	entity->GetTransform().SetPositionLocal(schemaData.mPos);
//			//	//entity->GetTransform().SetRotationnLocal(schemaData.mRotate);
//			//	entity->GetTransform().SetScaleLocal(schemaData.mScale);
//
//			//	//텍스처를 스키마파일에서 어떻게 구성할지고려해야한다. 
//			//	Texture* tex = TextureManager::GetTexture(L"Button.png");
//			//	entity->GetModel()->SetTextureToAllSubMesh(tex);
//		
//
//			//	for (auto it = schemaData.mAttributeTable.begin(); it != schemaData.mAttributeTable.end(); ++it)
//			//	{
//			//		entity->AddAttribute(it->first, it->second);
//			//	}
//
//			//	return entity;
//			//}
//			
//
//
//
//
//
//
//		}
//
//
//
//
//
//
//		return nullptr;
//	}
//
//}