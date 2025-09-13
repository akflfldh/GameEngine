#pragma once

namespace Quad
{

class EditorAssetImporterModule
{
  public:
    static EditorAssetImporterModule *GetInstance();

    EditorAssetImporterModule();
    ~EditorAssetImporterModule();

    // 하는일  :  importer들을 assetImporterManager에 등록
    void Initialize();

    //import 요청 , 수행 , 논리적파일시스템에 등록, asset 브라우저 활성화 등등
    void Import(const char *file);

  private:
};

} // namespace Quad