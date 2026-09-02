#pragma once
#include <EditorDirector/ImportCommonHeader.h>
#include <filesystem>
#include <vector>
namespace CoreAsset
{
class Asset;
}

/*


- 향후 개선 사안
[ 멀티스레드 ]
[ 진행도(문자열) 전달하는 콜백시스템]





*/

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

    // import 요청 , 수행 , 논리적파일시스템에 등록, asset 브라우저 활성화 등등 (비동기 )
    ImportTaskHandle RequestImport(const std::string &file, bool bEngine = false);

    ImportResult RequestImportSync(const std::filesystem::path &file, bool bEngine = false);

    // importer내에서 중첩하여 asset을 임포트하기위해 사용하는 메서드
    ImportResult ImportDendencySync(const char *file, bool bEngine = false);

    // 지금 첫실행시에도 에디터에서 엔진 에셋들을 임포트를 해야하고
    // 이후에 에디터내에서 유저파일로 임포트해야하는데 하나로 통일하면좋을거같은데

    // 엔진에셋 임포트시에 메인스레드 block 할거면 멀티스레드가 필요해?
    //   메인스레드에서 그냥 수행하면됨
    //  ImportTaskHandle즉 멀티스레드버전이 아닌 것도 제공하는거 어떰
    // 그리고 리턴으로그냥 결과값받고

    // 그 결과 post 처리는 workding data클래스의 메서드로 처리하고
    // 어느정도 유지는됨

    // 더 깔끔하고 좋은 구조는없나
    // 동일하게 하되 importEvent를 리턴하는거어떰 그럼 여기에 결과 or 실패 만 담는거고 중간 progress evnet는 오지않음.

    // 그리고 그것을 working data클래스의 메서드로 처리는 동일 .

    // 여기서 한번더 개선할수있나?
    // 아니면 아에 import working data 를 통해서만  import 모듈에 접근하도록?
    // 극러면 import working data 에서 두가지버전의 import함수 제공? 하나는 (멀티스레드), 다른하나는 (싱글 스레드 버전
    // )

    //

  private:
    ImportResult Import(const std::filesystem::path &file, bool bEngine, ImportJobContext *jobContext);
};

} // namespace Quad