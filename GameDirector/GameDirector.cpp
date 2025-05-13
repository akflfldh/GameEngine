#include "GameDirector.h"

#include"Application.h"
#include"GameWindow.h"
#include"GameWindowController.h"
#include"MapMetaData.h"
#include"Map/Map.h"
#include"GameMapInstanceGenerator.h"

#include"Project.h"
#include"Game3DSystem.h"
#include"GameUiSystem.h"
#include"Core/DefaultCollisionWorldFactoryImpl.h"
#include"Core/DefaultSpacePartitioningStructureFactoryImpl.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

    Quad::Application app;
    Quad::GameDirector gameDirector;
    Quad::AppInitData appInitData;

    appInitData.hInstance = hInstance;
    appInitData.nShowCmd = nCmdShow;
    appInitData.programDirector = &gameDirector;

    std::unique_ptr< Quad::DefaultCollisionWorldFactoryImpl> collisionFactoryImpl(new Quad::DefaultCollisionWorldFactoryImpl);
    appInitData.collisionWorldFactoryImpl = collisionFactoryImpl.get();
    std::unique_ptr< Quad::DefaultSpacePartitioningStructureFactoryImpl>sapcePartitioningStructureFactoryImpl(new Quad::DefaultSpacePartitioningStructureFactoryImpl);
    appInitData.spacePartitoingStructureFactoryImpl = sapcePartitioningStructureFactoryImpl.get();

    if (!app.Initialize(appInitData))
        return 0;

    return app.Run();
}

Quad::GameDirector::GameDirector()
    :mGameWindow(nullptr), mGameWindowController(nullptr),mProject(nullptr)
{

    wchar_t path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, path);

    mEditorPathW = path;
    mEditorPathA = Utility::ConvertToString(mEditorPathW, true);

}

void Quad::GameDirector::Initialize()
{

   // Utility::SetNewCurrentDirectory("C:\\Users\\dongd\\gitproject\\GameEngine\\x64\\Debug");
    //실행파일이있는 디렉터리가 현재 디렉터리라고 가정한다.

    //game window create, init
    //window not visible
     auto app= Quad::Application::GetInstance();
     
     //window

     mGameWindow = new GameWindow(app->GetHinstance());
     mGameWindow->CreateWindowClass();

     mGameWindowController = new GameWindowController(mGameWindow);
     mGameWindow->Initialize(mGameWindowController);
  
     ////controller(system)


  
     mGameWindowController->Initialize();








     //project resource load 
     mProject = new Project;

    auto resourceController =  ResourceController::GetInstance();


    resourceController->LoadEffect(".\\Asset\\Effect", mGameWindowController);

    //resourceController->LoadUserAsset(".\\Asset");
    resourceController->LoadAssetPackage(mEditorPathA);


    //resourceController->LoadUserTexture("Asset\\Texture");
    //resourceController->LoadUserMaterial("Asset\\Material");
    //resourceController->LoadUserMaterial("Asset\\Material");





    LoadLibrary(L".\\UserCode.dll");

    //project map load

    std::vector<MapMetaData*> map3DMetaDataVector;


    ReadMapMetaDataFile(".\\MapMetaDataFile.json", map3DMetaDataVector);

   // JsonParser::ReadFile(".\\MapMetaDataFile.json");
    

    std::string beforeDirectoryPath =    Utility::SetNewCurrentDirectory(".\\Map");

    auto gameSystem = Game3DSystem::GetInstance();
   // auto gameUiSystem = GameUiSystem::GetInstance();


    for (auto mapMetaData : map3DMetaDataVector)
    {
        Map * map =  GameMapInstanceGenerator::CreateMap(gameSystem, mapMetaData->GetMapName());
        if (map != nullptr)
        {

            JsonParser::ReadFile(map->GetName() + ".json");
            map->DeSerialize();
            mProject->AddMap(map->GetName(), map);
        }
    }
    //game3DSystem->SetMap(mProject->GetMap(map3DMetaDataVector[0]->GetMapName()));

    gameSystem->SetMap(mProject->GetMap(map3DMetaDataVector[0]->GetMapName()));
 


    Utility::SetNewCurrentDirectory(beforeDirectoryPath);



    //window visible


    //초기화 마지막, 맵전환시에 start호출

    Map * currMap =gameSystem->GetMap();
    currMap->Start();

}

void Quad::GameDirector::PreUpdate(float deltaTime)
{
}

void Quad::GameDirector::Update(float deltaTime)
{

    mGameWindow->Update(deltaTime);
    mGameWindow->UploadObjectToRenderSystem();


  //  mGameWindowController->Update(deltaTime);

}

void Quad::GameDirector::EndUpdate(float deltaTime)
{

    mGameWindow->EndUpdate(deltaTime);
}

void Quad::GameDirector::Draw()
{
    mGameWindow->Draw();


    //mGameWindowController->Draw();
}

void Quad::GameDirector::ReadMapMetaDataFile(const std::string& mapMetaDatFilePath, std::vector<Quad::MapMetaData*>& o3DMapMetaDataVector)
{

    unsigned int currentReadObjectIndext = 0;
    JsonParser::ReadFile(mapMetaDatFilePath);
    JsonParser::ReadStart();

    unsigned int map3DNum = 0;
   // unsigned int mapUiNum = 0;
    JsonParser::Read("Project_SystemMapMetaDataNum", map3DNum);

    o3DMapMetaDataVector.resize(map3DNum,new MapMetaData);

    for (auto mapElement : o3DMapMetaDataVector)
    {
        currentReadObjectIndext++;
        JsonParser::SetCurrentIndex(currentReadObjectIndext);
        mapElement->DeSerialize();
    }

}
