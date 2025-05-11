#pragma once

#include"Predeclare.h"
#include"Object/Entity.h"
#include"Object/Macro.h"

class {{CLASSNAME}} :public Quad::Entity
{
public:
    {{CLASSNAME}}();
    ~{{CLASSNAME}}();

    //?몄뒪?댁뒪 ?앹꽦??臾댁“嫄댄빐?쇳븯???묒뾽
    virtual void Initialize();

    //?뷀뤃?멸컪??媛吏寃뚮걫 珥덇린??
    virtual void DefaultCreatingInitialize() override;


    void Start() override;
    virtual void Update(float deltaTime) override;
    virtual void OnEvent(Quad::Event* event) override;
    virtual void UpdateCollider(float deltaTime) override;
    
    
    CLASSNAME({{CLASSNAME}})
    CLASSSIZE({{CLASSNAME}})

private:


};
REGISTERCLASS({{CLASSNAME}})