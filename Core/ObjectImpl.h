// #pragma once
//
// #include <Core/ObjectTypeDef.h>
// #include <Utility/UniqueID.h>
// #include <string>
// #include <unordered_map>
// #include <vector>
//
// namespace Core
//{
// class Component;
// class Object;
// class Map;
//
// class ObjectImpl
//{
//   public:
//     ObjectImpl(Object *interfaceObject);
//     ~ObjectImpl();
//
//     void Start();
//
//     void SetUniqueID(const CoreUtility::UniqueID &id);
//     CoreUtility::UniqueID GetUniqueID() const;
//
//     void SetObjectName(const char *objectName);
//     const char *GetObjectName() const;
//
//     Component *CreateComponent(const char *comClassName, const char *comInstanceName);
//     Component *GetComponent(const char *comInstanceName) const;
//
//     // 맨 처음 발견한 컴포넌트를 반환
//     Component *GetComponentFromClassName(const char *className) const;
//     void SetObjectPoolHandle(const ObjectHandle &handle);
//     const ObjectHandle &GetObjectHandle() const;
//
//     Map *GetMap() const;
//     void SetMap(Map *map);
//
//     void OnAddedToMap();
//
//   private:
//     void *GetMemory(size_t size);
//     void ReleaseMemory(void *);
//
//     void ReleaseComponent(Component *com);
//
//   private:
//     std::vector<Component *> mComList;
//     std::unordered_map<std::string, Component *> mComNameTable;
//     CoreUtility::UniqueID mObjectUniqueID;
//
//     Core::ObjectHandle mObjectPoolHandle;
//     std::string mObjectName;
//     Object *mInterfaceObject;
//
//     // Object가 속한 Map
//     Map *mMap;
// };
// } // namespace Core
