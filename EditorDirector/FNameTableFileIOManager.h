#pragma once

class FNameTable;

namespace Quad
{

class FNameTableFileIOManager
{
  public:
    static FNameTableFileIOManager *GetInstance();
    FNameTableFileIOManager();
    ~FNameTableFileIOManager();


    //로드 
    // 
    //저장
    /*
        공통으로 파일경로 가필요할거고 

    */
    

private:
    FNameTable *mNameTable;
};

} // namespace Quad