// #include "ObjectImpl.h"
// #include <Core/Component.h>
// #include <ReflectSystem/ReflectionSystem.h>
// Core::ObjectImpl::ObjectImpl(Object *interfaceObject) : mInterfaceObject(interfaceObject) {}
//
// Core::ObjectImpl::~ObjectImpl()
//{
//
//     for (auto it = mComList.rbegin(); it != mComList.rend(); ++it)
//     {
//         ReleaseComponent(*it);
//     }
// }
//
// void Core::ObjectImpl::Start()
//{
//
//     for (auto com : mComList)
//     {
//         com->Start();
//     }
// }
//
// void Core::ObjectImpl::SetUniqueID(const CoreUtility::UniqueID &id)
//{
//     mObjectUniqueID = id;
// }
//
// CoreUtility::UniqueID Core::ObjectImpl::GetUniqueID() const
//{
//     return mObjectUniqueID;
// }
//
// void Core::ObjectImpl::SetObjectName(const char *objectName)
//{
//
//     mObjectName = objectName;
// }
// const char *Core::ObjectImpl::GetObjectName() const
//{
//
//     return mObjectName.c_str();
// }
//
// Core::Component *Core::ObjectImpl::CreateComponent(const char *comClassName, const char *comInstanceName)
//{
//
//     std::unordered_map<std::string, Component *>::iterator it = mComNameTable.find(comInstanceName);
//     if (it != mComNameTable.end())
//     {
//
//         // log
//         return nullptr;
//     }
//
//     // reflect system 을 통해서 com 생성
//
//     Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
//
//     Component *com = (Component *)(reflectionSystem->CreateClassInstance(comClassName));
//     if (com == nullptr)
//     {
//         return nullptr;
//     }
//     // 이름부여
//     com->SetInstanceName(comInstanceName);
//     com->SetOwnerObject(mInterfaceObject);
//
//     // 테이블 등록
//     mComNameTable[comInstanceName] = com;
//     mComList.push_back(com);
//
//     return com;
// }
//
// Core::Component *Core::ObjectImpl::GetComponent(const char *comInstanceName) const
//{
//     std::unordered_map<std::string, Component *>::const_iterator it = mComNameTable.find(comInstanceName);
//     if (it == mComNameTable.cend())
//     {
//
//         // log
//         return nullptr;
//     }
//
//     return it->second;
// }
//
// Core::Component *Core::ObjectImpl::GetComponentFromClassName(const char *className) const
//{
//     for (auto com : mComList)
//     {
//
//         if (com != nullptr && std::strcmp(com->GetRunTimeClassName(), className) == 0)
//         {
//             return com;
//         }
//     }
//
//     return nullptr;
// }
//
// void Core::ObjectImpl::SetObjectPoolHandle(const ObjectHandle &handle)
//{
//     mObjectPoolHandle = handle;
// }
// const Core::ObjectHandle &Core::ObjectImpl::GetObjectHandle() const
//{
//
//     return mObjectPoolHandle;
// }
//
// Core::Map *Core::ObjectImpl::GetMap() const
//{
//     return mMap;
// }
//
// void Core::ObjectImpl::SetMap(Map *map)
//{
//
//     mMap = map;
// }
//
// void Core::ObjectImpl::OnAddedToMap()
//{
//
//     for (Core::Component *com : mComList)
//     {
//         com->OnOwnerObjectAddedToMap();
//     }
// }
//
// void Core::ObjectImpl::ReleaseComponent(Component *com)
//{
//
//     if (com == nullptr)
//         return;
//
//     mComNameTable.erase(com->GetInstanceName());
//     mComList.erase(std::find(mComList.begin(), mComList.end(), com));
//
//     Quad::ReflectionSystem *reflectionSystem = Quad::ReflectionSystem::GetInstance();
//
//     reflectionSystem->DestoryClassInstance(com);
// }
//
// void *Core::ObjectImpl::GetMemory(size_t size)
//{
//     // 후에 커스텀할당자코드 삽입
//     return new char[size];
// }
//
// void Core::ObjectImpl::ReleaseMemory(void *pMem)
//{
//
//     delete[] pMem;
// }
