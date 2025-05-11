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


namespace Quad
{
 
    Object::Object(const std::string& name, EObjectType objectType)
       :SceneElement(ESceneElementType::eObject),mName(name),mMap(nullptr), mDrawFlag(true), mActiveFlag(true), mEntireDrawFlag(true), mSelectFlag(true), mStencilRefValue(0),mObjectType(objectType),mTransform(this),mSystem(nullptr),mUniqueID(0), mStartObjectFlag(true)
    {
        

        mTransform.Initialize();
        mStateComponent.Initialize(this);
    
    
    }

    Object::~Object()
    {
        for (auto& component : mComponentTable)
        {
            delete component.second;
        }
        



    }


    void Object::Start()
    {




    }

    void Object::Update(float delta)
    {
        auto callback = GetUpdateCallback();
        if ((*callback))
            (*callback)(this,delta);




        for (auto& componentElement : mComponentTable)
        {
            BaseComponent * component = componentElement.second;
            component->Update(delta);
        }




    }


    





    const Transform& Object::GetTransform() const
    {


        return mTransform;
        // TODO: 여기에 return 문을 삽입합니다.
    }

    Transform& Object::GetTransform()
    {
        return mTransform;
        // TODO: 여기에 return 문을 삽입합니다.
    }

    void Object::SetTransform(Transform& transform)
    {
        mTransform = transform;
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

        std::queue<ObjectSmartPointer> objectQueue;
        objectQueue.push(this);

        while (!objectQueue.empty())
        {
           ObjectSmartPointer & currObject= objectQueue.front();
           objectQueue.pop();
           currObject->SetDrawFlag(flag);
           const std::vector<ObjectSmartPointer>& childObjectVector = currObject->GetChildObjectVector();
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

    bool Object::GetActiveFlag() const
    {
        return mActiveFlag;
    }

    void Object::SetSelectFlag(bool flag)
    {
        mSelectFlag = flag;





    }

    void Object::SendSelectEvent(bool state , bool exclusiveEventHandleFlag)
    {
        SelectEvent* selectEvent = new SelectEvent;
        selectEvent->SetSelectedObject(this);
        selectEvent->SetSelectState(state);
        selectEvent->SetExclusiveEventHandleFlag(exclusiveEventHandleFlag);
        EventDispatcher::SendEvent(selectEvent, this->GetSystem()->GetSystemID());

    }

    bool Object::GetSelectFlag() const
    {
        return mSelectFlag;
    }

    bool Object::AddChildObject(Object* childObject)
    {
        //순환구조일경우실패

        if (childObject == nullptr)
            return false;

        childObject->mParentObject = this;
        mChildObjectVector.push_back(childObject);



        if (GetIsAddedToSceneFlag())
        {
            //부모object가 이미씬에 들어가있는가?
            //그렇다면 자식도 씬에 넣어야한다.
            // childObject와 그 자손들도 다 들어간다.
           //자손들은 다 씬에서 처리된다.

         //   this->GetSystem()->GetMap()->AddObject(childObject);
            this->GetSystem()->GetMap()->SetChildObject(this, childObject);


        }
        else
        {

            //맵에 안들어가더라도 부모자식관계가 성립되면 트랜스폼을 조정해야한다.(맵(씬)에 들어간경우라면 내부적으로 트랜스폼조정도 함께처리된다)

            Transform& childObjectTransform = childObject->GetTransform();


            DirectX::XMFLOAT4X4 parentWorldMatrixF = this->GetTransform().GetWorldMatrix();
            DirectX::XMFLOAT4X4 childWorldMatrixF = childObjectTransform.GetWorldMatrix();

            DirectX::XMMATRIX parentWorldMatrix = DirectX::XMLoadFloat4x4(&parentWorldMatrixF);
            DirectX::XMMATRIX childWorldMatrix = DirectX::XMLoadFloat4x4(&childWorldMatrixF);


            DirectX::XMMATRIX parentInvWorldMatrix = DirectX::XMMatrixInverse(nullptr, parentWorldMatrix);
            if (childObjectTransform.GetIndependentScaleFlag() || childObjectTransform.GetIndependentRotationFlag() || childObjectTransform.GetIndependentTransformFlag())
            {
                //분해해서 해당성분을 제거하고, 다시 부모의 월드변환행렬의 역행렬을 계산한다.

                DirectX::XMVECTOR parentScaleWorld;
                DirectX::XMVECTOR parentQuaternionWorld;
                DirectX::XMVECTOR parentTranslationWorld;

                DirectX::XMMatrixDecompose(&parentScaleWorld, &parentQuaternionWorld, &parentTranslationWorld, parentWorldMatrix);

                if (!childObjectTransform.GetIndependentScaleFlag())
                {
                    parentWorldMatrix = DirectX::XMMatrixScalingFromVector(parentScaleWorld);
                }
                else
                {
                    parentWorldMatrix = DirectX::XMMatrixIdentity();
                }

                if (!childObjectTransform.GetIndependentRotationFlag())
                {
                    parentWorldMatrix = DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixRotationQuaternion(parentQuaternionWorld));

                }

                if (!childObjectTransform.GetIndependentTransformFlag())
                {
                    parentWorldMatrix = DirectX::XMMatrixMultiply(parentWorldMatrix, DirectX::XMMatrixTranslationFromVector(parentTranslationWorld));
                }

                DirectX::XMStoreFloat4x4(&parentWorldMatrixF, parentWorldMatrix);
                parentInvWorldMatrix = DirectX::XMMatrixInverse(nullptr, parentWorldMatrix);

            }


            DirectX::XMMATRIX childLocalMatrix = DirectX::XMMatrixMultiply(childWorldMatrix, parentInvWorldMatrix);
            childObjectTransform.SetTransformLocal(childLocalMatrix);
            //부모의 행렬을 넘겨서 업데이트해줘야 제대로 world속성이 계산된다.
            childObjectTransform.UpdateWorldMatrix(parentWorldMatrixF);

        }
       


        return true;
    }

    void Object::RemoveChildObject(Object* childObject)
    {

        //벡터에서도 제거 ,
        
        bool ret = RemoveChildObjectInVector(childObject);

        //맵에서도 부모자식관계를 제거 
        //원래라면 이오브젝트가들어있는 맵에접근해야하는데 지금은 맵이 하나라고 생각

           //부모 자식관계를 끊는거고 자식의 새로운부모는 루트가되는거고.

        if (ret)
        {
            Map* map = GetSystem()->GetMap();
            GetSystem()->GetMap()->ChangeParentObject(nullptr, childObject);
        }


    }

    const std::vector<ObjectSmartPointer>& Object::GetChildObjectVector() const
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

   /* TaskWindow* Object::GetWindow() const
    {

        Controller* controller = mSystem->GetController();
        DockingWindowController* wndController = dynamic_cast<DockingWindowController*>(controller);

        return wndController != nullptr ? wndController->GetWindow() : nullptr;
    }*/

    System* Object::GetSystem() const
    {
        return mSystem;
    }

    const std::string Object::GetName() const
    {
        return mName;

        // TODO: 여기에 return 문을 삽입합니다.
    }

    void Object::SetName(const std::string& name)
    {
        mName = name;
    }

    void Object::SetIDState(bool state)
    {
        mIDState = state;
    }

    bool Object::GetIDState() const
    {
        return mIDState;
    }

    void Object::SetSelectKeepingFlag(bool flag)
    {
        mSelectKeepingFlag = flag;
    }

    bool Object::GetSelectKeepingFlag() const
    {
        return mSelectKeepingFlag;
    }

    void Object::SetSelectAvailableFlag(bool flag)
    {
        mSelectAvailableFlag = flag;


   





    }

    bool Object::GetSelectAvailableFlag() const
    {
        return mSelectAvailableFlag;
    }

    void Object::SetSelectBlockFlag(bool flag)
    {
        mSelectBlockFlag = flag;
    }

    bool Object::GetSelectBlockFlag() const
    {
        return mSelectBlockFlag;
    }

    void Object::SetEntireSelectAvailableFlag(bool flag)
    {
        mEntireSelectAvailableFlag = flag;
        mSelectAvailableFlag = flag;


         std::queue<ObjectSmartPointer> objectQueue;
         objectQueue.push(this);

         while (!objectQueue.empty())
         {
         ObjectSmartPointer & currObject = objectQueue.front();
        objectQueue.pop();
        //currObject->SetDrawFlag(flag);
        const std::vector<ObjectSmartPointer>& childObjectVector = currObject->GetChildObjectVector();
        for (int i = 0; i < childObjectVector.size(); ++i)
        {
            childObjectVector[i]->SetSelectAvailableFlag(flag);
            objectQueue.push(childObjectVector[i]);
        }

        }

    }

    bool Object::GetEntireSelectAvailableFlag() const
    {
        return mEntireSelectAvailableFlag;
    }

    EObjectType Object::GetObjectType() const
    {
        return mObjectType;
    }

    const StateComponent* Object::GetStateComponent() const
    {
        return &mStateComponent;
    }

    StateComponent* Object::GetStateComponent()
    {
        return &mStateComponent;
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
        return mParentObject.GetPointer();
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

    void Object::RegisterEventCallback(const std::string& eventName, const std::function<void(Event* pEvent)> &  callback)
    {
        if (callback == nullptr)
            return;


        mEventCallbackUnMap[eventName].push_back(callback);



    }

    std::function<void(Event*pEvent)> *  Object::GetEventCallback(const std::string& name, int index)
    {
        std::unordered_map<std::string, std::vector<std::function<void(Quad::Event*)>>>::iterator it  = mEventCallbackUnMap.find(name);
        if (it == mEventCallbackUnMap.end())
            return nullptr;

        std::vector<std::function<void(Quad::Event*)>>& callbackVector = it->second;
        if (callbackVector.size() <= index)
            return nullptr;

        return &callbackVector[index];
    }

    void Object::CallEventCallback(const std::string& name,Event * pEvent)
    {
        std::unordered_map<std::string, std::vector<std::function<void(Quad::Event*)>>>::iterator it = mEventCallbackUnMap.find(name);
        if (it == mEventCallbackUnMap.end())
            return ;


        std::vector<std::function<void(Quad::Event*)>>& eventCallbackVector = it->second;
        for (auto& callbackElement : eventCallbackVector)
        {
            callbackElement(pEvent);
        }




    }

    void Object::SetUpdateCallback(const std::function<void(Object * object ,float deltaTime)> & callback)
    {
        mUpdateCallback = callback;
    }

    const std::function<void(Object * object ,float deltaTime)> * Object::GetUpdateCallback() const
    {

        return &mUpdateCallback;
    }

    void Object::HandleDefaultEvent(Event* pEvent)
    {
        const std::string& eventName = pEvent->GetName();

      
        auto callback = GetEventCallback(eventName);
        if(callback!= nullptr)
            (*callback)(pEvent);
    

    }



    void Object::AddComponent(BaseComponent* component)
    {
        mComponentTable.insert({ component->GetID(),component });
    }

    BaseComponent* Object::GetComponent(int componentID) const
    {
        std::unordered_map<int,BaseComponent*> ::const_iterator ret=  mComponentTable.find(componentID);

        if (ret != mComponentTable.cend())
            return ret->second;

        return nullptr;
    }

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


    void Object::SetIsAddedToSceneFlag(bool state)
    {
        mIsAddedToSceneState = state;
    }

    bool Object::GetIsAddedToSceneFlag() const
    {
        return mIsAddedToSceneState;
    }

    void Object::UpdateComponent(float delta)
    {

        std::for_each(mComponentTable.begin(), mComponentTable.end(), [delta](const std::pair<int,BaseComponent* >& element ) {

            element.second->Update(delta);

            });
    }

    void Object::SetUniqueID(unsigned long long id)
    {
        mUniqueID = id;
    }

    void Object::SetKilledState(bool state)
    {
        mIsKilledState = state;
    }

    bool Object::RemoveChildObjectInVector(Object* childObject)
    {
        std::vector<ObjectSmartPointer>::iterator it = std::find_if(mChildObjectVector.begin(), mChildObjectVector.end()
            , [childObject](const ObjectSmartPointer& pointer) {
                if (childObject == pointer.GetPointer())
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
        JsonParser::Write("Object_Name", GetName());
        JsonParser::Write("Object_MapLayerID", GetMapLayerID());
        mTransform.Serialize();
        


    }
    
    void Object::DeSerialize()
    {
    
        SceneElement::DeSerialize();

        JsonParser::Read("Object_Name",mName);
        JsonParser::Read("Object_MapLayerID", mMapLayerID);
        mTransform.DeSerialize();


    
    }

    bool Object::GetEnginObjectFlag() const
    {
        return mIsEngineObject;
    }

    void Object::SetEngineObjectFlag(bool flag)
    {
        mIsEngineObject = flag;
    }

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
    }



}