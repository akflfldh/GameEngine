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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{

    Quad::Application app;

    Quad::GameDirector gameDirector;

    if (!app.Initialize(hInstance, nCmdShow, &gameDirector))
        return 0;

    return app.Run();
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
    resourceController->LoadUserAsset(".\\Asset");


    //resourceController->LoadUserTexture("Asset\\Texture");
    //resourceController->LoadUserMaterial("Asset\\Material");
    //resourceController->LoadUserMaterial("Asset\\Material");





    LoadLibrary(L".\\UserCode.dll");

    //project map load

    std::vector<MapMetaData*> map3DMetaDataVector;
    std::vector<MapMetaData*> mapUiMetaDataVector;


    ReadMapMetaDataFile(".\\MapMetaDataFile.json", map3DMetaDataVector, mapUiMetaDataVector);

   // JsonParser::ReadFile(".\\MapMetaDataFile.json");
    

    std::string beforeDirectoryPath =    Utility::SetNewCurrentDirectory(".\\Map");

    auto game3DSystem = Game3DSystem::GetInstance();
    auto gameUiSystem = GameUiSystem::GetInstance();


    for (auto mapMetaData : map3DMetaDataVector)
    {
        Map * map =  GameMapInstanceGenerator::CreateMap(game3DSystem, mapMetaData->GetMapName());
        if (map != nullptr)
        {

            JsonParser::ReadFile(map->GetName() + ".json");
            map->DeSerialize();
            mProject->AddMap(map->GetName(), map);
        }
    }
    //game3DSystem->SetMap(mProject->GetMap(map3DMetaDataVector[0]->GetMapName()));


    for (auto mapMetaData : mapUiMetaDataVector)
    {
        Map* map = GameMapInstanceGenerator::CreateMap(gameUiSystem, mapMetaData->GetMapName());
        if (map != nullptr)
        {
            JsonParser::ReadFile(map->GetName() + ".json");
            map->DeSerialize();
            mProject->AddMap(map->GetName(), map);
        }
    }



 
    game3DSystem->SetMap(mProject->GetMap(map3DMetaDataVector[0]->GetMapName()));
    gameUiSystem->SetMap(mProject->GetMap(mapUiMetaDataVector[0]->GetMapName()));


    Utility::SetNewCurrentDirectory(beforeDirectoryPath);



    //window visible

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
}

void Quad::GameDirector::Draw()
{
    mGameWindow->Draw();


    //mGameWindowController->Draw();
}

void Quad::GameDirector::ReadMapMetaDataFile(const std::string& mapMetaDatFilePath, std::vector<Quad::MapMetaData*>& o3DMapMetaDataVector, std::vector<Quad::MapMetaData*>& oUiMapMetaDataVector)
{

    unsigned int currentReadObjectIndext = 0;
    JsonParser::ReadFile(mapMetaDatFilePath);
    JsonParser::ReadStart();

    unsigned int map3DNum = 0;
    unsigned int mapUiNum = 0;
    JsonParser::Read("Project_3DSystemMapMetaDataNum", map3DNum);

    o3DMapMetaDataVector.resize(map3DNum,new MapMetaData);

    for (auto mapElement : o3DMapMetaDataVector)
    {
        currentReadObjectIndext++;
        JsonParser::SetCurrentIndex(currentReadObjectIndext);
        mapElement->DeSerialize();
    }

    currentReadObjectIndext++;
    JsonParser::SetCurrentIndex(currentReadObjectIndext);
    JsonParser::Read("Project_UiSystemMapMetaDataNum", mapUiNum);

    oUiMapMetaDataVector.resize(mapUiNum,new MapMetaData);

    for (auto mapElement : oUiMapMetaDataVector)
    {
        currentReadObjectIndext++;
        JsonParser::SetCurrentIndex(currentReadObjectIndext);
        mapElement->DeSerialize();
    }


}
