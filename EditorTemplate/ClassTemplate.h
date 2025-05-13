#pragma once

#include"Predeclare.h"
#include"Object/Entity.h"
#include"Object/Macro.h"

class {{CLASSNAME}} :public Quad::Entity
{
public:
    {{CLASSNAME}}();
    ~{{CLASSNAME}}();

    //인스턴스 생성시 항상 호출되는 초기화메서드
    virtual void Initialize();

   //맵에서 생성시 추가적으로 호출될 초기화메서드
    virtual void DefaultCreatingInitialize() override;


    void Start() override;
    virtual void Update(float deltaTime) override;
    virtual void EndUpdate(float deltaTime)override;
    virtual void OnEvent(Quad::Event* event) override;
    virtual void UpdateCollider(float deltaTime) override;
    
    
    CLASSNAME({{CLASSNAME}})
    CLASSSIZE({{CLASSNAME}})

private:


};
REGISTERCLASS({{CLASSNAME}})