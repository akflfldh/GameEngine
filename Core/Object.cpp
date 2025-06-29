#include "Object/Object.h"
#include"Core/System.h"
#include"Core/Controller.h"
//#include"DockingWindowController.h"

#include"Core/EventDispatcher.h"
#include"Core/SelectEvent.h"
#include"Utility/MathHelper.h"
//#include"ObjectIDManager.h"
#include"Parser/JsonParser.h"
#include"Utility/Utility.h"

#include"Component.h"

#include"Component/ComponentFactory.h"
#include"Component/SceneComponent.h"

namespace Quad
{
 
    Object::Object(EObjectType objectType)
       :SceneElement("",ESceneElementType::eObject), mMap(nullptr), mDrawFlag(true), mEntireDrawFlag(true), mActiveFlag(true),mStencilRefValue(0), mObjectType(objectType), mSystem(nullptr), mUniqueID(0), mStartObjectFlag(true)
    {
        mRootSceneComponent =  AddComponent<SceneComponent>("RootSceneComponent");
      
    }

    Object::~Object()
    {
        for (auto& component : mComponentTable)
        {
            /*delete component.second;*/

            component.second->DeActive();
            ComponentFactory::ReleaseComponent(component.second);
        }

    }


    void Object::OnEvent(Event* event)
    {

    }

    void Object::Start()
    {

        for (auto componentElement : mComponentTable)
        {
            BaseComponent* component = componentElement.second;
            component->Start();
        }
    }

    void Object::Update(float delta)
    {
      
        for (auto& componentElement : mComponentTable)
        {
            BaseComponent * component = componentElement.second;
            component->Update(delta);
        }

    }

    void Object::EndUpdate(float deltaTime)
    {


    }

    void Object::SetObjectPositionLocal(const DirectX::XMFLOAT3& pos)
    {
        mRootSceneComponent->GetTransform().SetPositionLocal(pos);
    }

    void Object::SetObjectPositionLocal(float x, float y, float z)
    {
        SetObjectPositionLocal({ x,y,z });
    }

    void Object::SetObjectPositionWorld(const DirectX::XMFLOAT3& pos)
    {
        mRootSceneComponent->SetPositionWorld(pos);

    }

    void Object::SetObjectPositionWorld(float x, float y, float z)
    {
        SetObjectPositionWorld({ x,y,z });
    }


    DirectX::XMFLOAT3 Object::GetObjectPositionLocal() const
    {
        return mRootSceneComponent->GetTransform().GetPositionLocal();
    }

    DirectX::XMFLOAT3 Object::GetObjectPositionWorld() const
    {
        return mRootSceneComponent->GetTranslationWorld();
    }

    DirectX::XMFLOAT4 Object::GetObjectQuaternionLocal() const
    {
        return mRootSceneComponent->GetRotationWorld();
    }

    DirectX::XMFLOAT4 Object::GetObjectQuaternionWorld() const
    {
        return mRootSceneComponent->GetTransform().GetQuaternionLocal();
    }

    void Object::SetObjectQuaternionWorld(const DirectX::XMFLOAT4& quaternion)
    {
        mRootSceneComponent->SetRotationWorld(quaternion);

    }

    DirectX::XMFLOAT3 Object::GetObjectLookWorld() const
    {
        return mRootSceneComponent->GetLookWorld();
    }

    DirectX::XMFLOAT3 Object::GetObjectRightWorld() const
    {
        return mRootSceneComponent->GetRightWorld();
    }

    DirectX::XMFLOAT3 Object::GetObjectUpWorld() const
    {
        return mRootSceneComponent->GetUpWorld();
    }


    





    const Transform& Object::GetTransform() const
    {

        return mRootSceneComponent->GetTransform();
        // TODO: 여기에 return 문을 삽입합니다.
    }

    Transform& Object::GetTransform()
    {
        return mRootSceneComponent->GetTransform();
        // TODO: 여기에 return 문을 삽입합니다.
    }


    void Object::Initialize()
    {

    }

    void Object::DefaultCreatingInitialize()
    {

    }


    void Object::SetDrawFlag(bool flag)
    {
        mDrawFlag = flag;
    }

    bool Object::GetDrawFlag() const
    {
        return mDrawFlag;
    }

    void Object::SetEntireDrawFlag(bool flag)
    {
        mEntireDrawFlag = flag;

        std::queue<Object*> objectQueue;
        objectQueue.push(this);

        while (!objectQueue.empty())
        {
            Object* currObject= objectQueue.front();
           objectQueue.pop();
           currObject->SetDrawFlag(flag);
           const std::vector<Object*>& childObjectVector = currObject->GetChildObjectVector();
           for (int i = 0; i < childObjectVector.size(); ++i)
           {
               childObjectVector[i]->SetDrawFlag(flag);
               objectQueue.push(childObjectVector[i]);
           }
        }


    }

    bool Object::GetEntrieDrawFlag() const
    {
        return mEntireDrawFlag;
    }
  

    void Object::SetActiveFlag(bool flag)
    {
        mActiveFlag = flag;

    }
    bool Object::GetActiveFlag()const
    {
        return mActiveFlag;
    }


    bool Object::AddChildObject(Object* childObject)
    {
      
        mMap->SetChildObject(this, childObject);
        return true;
    }

    void Object::RemoveChildObject(Object* childObject)
    {
    
        mMap->ChangeParentObject(nullptr, childObject);
    }

    const std::vector<Object*>& Object::GetChildObjectVector() const
    {
        return mChildObjectVector;
       
    }

    void Object::SetStencilRefValue(unsigned char value)
    {
        mStencilRefValue = value;
    }

    unsigned char Object::GetStencilRefValue() const
    {
        return mStencilRefValue;
    }

    const std::string& Object::GetControllerName() const
    {
       return  mSystem->GetController()->GetName();
    
    }

    void Object::SetSystem(System* system)
    {
        if (system == nullptr)
            int a = 2;
        mSystem = system;
    }


    System* Object::GetSystem() const
    {
        return mSystem;
    }


  
    EObjectType Object::GetObjectType() const
    {
        return mObjectType;
    }


    bool Object::InnerDetectCollsion(Collider* colliderA, Collider* colliderB)
    {
        return true;
    }

    bool Object::InnerDetectRayHit(const Ray& ray)
    {
        return true;
    }

    Object* Object::GetParentObject() const
    {
        return mParentObject;
    }

    void Object::RegisterAcceptEvent(const std::string& eventName)
    {
        System* system = GetSystem();
        if(system!=nullptr)
            system->RegisterEntityToEvent(eventName,this);
        else
        {
            const std::wstring output = L"System이없는데 event에" + Utility::ConvertToWString(GetName(),true) + L"엔티티를 등록할려고함 \n";
            OutputDebugStringW(output.c_str());  
        }

    }

  

    void Object::RemoveComponent(const char* componentName)
    {

        std::unordered_map<std::string,BaseComponent*>::iterator it =  mComponentTable.find(componentName);

        if (it == mComponentTable.end())
            return;

        BaseComponent* component = it->second;
        mComponentTable.erase(it);

        component->DeActive();


        ComponentFactory::ReleaseComponent(component);
    }



 /*   void Object::AddComponent(BaseComponent* component)
    {
        mComponentTable.insert({ component->GetID(),component });
    }*/

  

    void Object::SetMapLayer(int mapLayerID)
    {
        mMapLayerID = mapLayerID;
    }

    int Object::GetMapLayerID() const
    {
        return mMapLayerID;
    }

    unsigned long long Object::GetUniqueID() const
    {
        return mUniqueID;
    }

    Map* Object::GetMap() const
    {
        return mMap;
    }

    Map* Object::GetMap()
    {
        return mMap;
    }

    //void Object::UpdateComponent(float delta)
    //{

    // /*   std::for_each(mComponentTable.begin(), mComponentTable.end(), [delta](const std::pair<int,BaseComponent* >& element ) {

    //        element.second->Update(delta);

    //        });*/ 


    //    for (auto& componentElement : mComponentTable)
    //    {
    //        componentElement.second->Update(delta);
    //    }

    //}

    void Object::SetUniqueID(unsigned long long id)
    {
        mUniqueID = id;
    }

    void Object::SetKilledState(bool state)
    {
        mIsKilledState = state;
    }

    BaseComponent * Object::AddComponent(const char* componentTypeName, const char* componentName)
    {
        std::unordered_map<std::string, BaseComponent*>::iterator it =  mComponentTable.find(componentName);
        if (it != mComponentTable.end())
            return nullptr;

       
        BaseComponent * component =  ComponentFactory::CreateComponent(componentTypeName);
        
        if (component)
        {
            mComponentTable[componentName] = component;
            component->SetDestObject(this);


            if (mMap != nullptr)
                component->OnActive();


        }
        return component;

    }

    BaseComponent* Object::GetComponent(const char* componentName) const
    {
       
        std::unordered_map<std::string, BaseComponent*>::const_iterator it = mComponentTable.find(componentName);
     
       return it == mComponentTable.end() ? nullptr : it->second;

    }

    void Object::UpdateSceneComponentWorldTransformReculsivly()
    {
        for(auto componentElement : mComponentTable )
        { 
            if (componentElement.second->GetComponentType() == EComponentType::ESceneComponentType)
            {
                SceneComponent* sceneComponent = static_cast<SceneComponent*>(componentElement.second);
                sceneComponent->GetTransformWorldMatrixF();
            }
        }

    }


    bool Object::RemoveChildObjectInVector(Object* childObject)
    {
        std::vector<Object*>::iterator it = std::find_if(mChildObjectVector.begin(), mChildObjectVector.end()
            , [childObject](const Object* pointer) {
                if (childObject == pointer)
                    return true;
                return false; });


        if (it != mChildObjectVector.end())
        {
            mChildObjectVector.erase(it);
            return true;
        }
        return false;

    }

  

    void Object::Serialize()
    {
       // JsonParser::StartWriteObject();
        SceneElement::Serialize();

        JsonParser::Write("ClassName", GetClassTypeName());
        JsonParser::Write("Object_ID", GetUniqueID());
        //JsonParser::Write("Object_Name", GetName());
        JsonParser::Write("Object_MapLayerID", GetMapLayerID());
        //mTransform.Serialize();
        


    }
    
    void Object::DeSerialize()
    {
    
        SceneElement::DeSerialize();

       // JsonParser::Read("Object_Name",mName);
        JsonParser::Read("Object_MapLayerID", mMapLayerID);
       // mTransform.DeSerialize();


    
    }

   /* bool Object::GetEnginObjectFlag() const
    {
        return mIsEngineObject;
    }

    void Object::SetEngineObjectFlag(bool flag)
    {
        mIsEngineObject = flag;
    }*/

    bool Object::GetKilledState() const
    {
        return mIsKilledState;
    }

    bool Object::GetStartObjectFlag() const
    {
        return mStartObjectFlag;
    }

    void Object::SetStartObjectFlag(bool flag)
    {
        mStartObjectFlag = flag;
    }

    void Object::BeforeRemoveBehavior()
    {

    }

    void Object::SetMap(Map* map)
    {
        mMap = map;

        for (auto componentElement : mComponentTable)
        {

            componentElement.second->OnActive();

        }


    }

    DirectX::XMFLOAT4X4 Object::GetTransformWorldMatrix() const
    {

        return mRootSceneComponent->GetTransformWorldMatrixF();
    }

    SceneComponent* Object::GetRootSceneComponent() const
    {
        return mRootSceneComponent;
    }



}