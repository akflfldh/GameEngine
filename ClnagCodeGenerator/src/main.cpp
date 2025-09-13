#include <iostream>
#include<vector>
#include<string>
#include<fstream>

// Clang 라이브러리에서 필요한 헤더 파일들을 가져옵니다.
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include"clang/AST/RecordLayout.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"


static llvm::cl::OptionCategory MyToolCategory("My Code Generator Options");

/* ▼▼ 추가 ▼▼  */
static llvm::cl::opt<std::string> OutputDir(           // ← 전역 옵션 선언
	"outdir",
	llvm::cl::desc("출력 경로 설정"),
	llvm::cl::value_desc("path"),
	llvm::cl::init("Generated"),                       // 기본값
	llvm::cl::cat(MyToolCategory));
/* ▲▲ 추가 ▲▲  */






// deps 전용 스위치/출력 파일
static llvm::cl::opt<bool> OptDepsOnly("deps-only",
	llvm::cl::desc("헤더 의존성만 수집"), llvm::cl::init(false), llvm::cl::cat(MyToolCategory));



// 새로 추가할 depfile 경로 옵션
static llvm::cl::opt<std::string> OptDepfile(
	"depfile",
	llvm::cl::desc("Path to write Make-style depfile"),
	llvm::cl::value_desc("path"),
	llvm::cl::cat(MyToolCategory)
);

// 새로 추가할 depfile 타겟 옵션
static llvm::cl::opt<std::string> OptDepTarget(
	"deptarget",
	llvm::cl::desc("Target path to use in depfile (must match CMake OUTPUT)"),
	llvm::cl::value_desc("path"),
	llvm::cl::cat(MyToolCategory)
);

// (선택) 라인 목록 출력용 --outdeps 옵션
static llvm::cl::opt<std::string> OptOutDeps(
	"outdeps",
	llvm::cl::desc("Write raw dependency list to file (debug)"),
	llvm::cl::value_desc("path"),
	llvm::cl::cat(MyToolCategory)
);



static llvm::cl::opt<std::string> OptBasename(
	"basename",
	llvm::cl::desc("Base name for generated files (e.g., 'UIType')"),
	llvm::cl::value_desc("name"),
	llvm::cl::init(""),
	llvm::cl::cat(MyToolCategory)
);

// --out-subdir: outdir 아래에 하위 폴더를 추가해서 구조를 보존하고 싶을 때(선택)
static llvm::cl::opt<std::string> OptOutSubdir(
	"out-subdir",
	llvm::cl::desc("Subdirectory under outdir to place outputs"),
	llvm::cl::value_desc("relpath"),
	llvm::cl::init(""),
	llvm::cl::cat(MyToolCategory)
);

static std::string MakeEffectiveOutDir() {
	llvm::SmallString<260> dir(OutputDir.getValue());
	if (!OptOutSubdir.getValue().empty()) {
		llvm::sys::path::append(dir, OptOutSubdir.getValue());
	}
	llvm::sys::fs::create_directories(dir);
	return llvm::sys::path::convert_to_slash(dir.str()).c_str();
}



class DepsCollector : public clang::PPCallbacks {
public:
	DepsCollector(clang::SourceManager& SM,
		llvm::StringSet<>& uniq,
		std::vector<std::string>& out)
		: SM(SM), Uniq(uniq), Out(out) {
	}

#if LLVM_VERSION_MAJOR >= 15
	void InclusionDirective(clang::SourceLocation, const clang::Token&,
		llvm::StringRef /*FileName*/, bool /*IsAngled*/,
		clang::CharSourceRange /*FilenameRange*/,
		clang::OptionalFileEntryRef File,
		llvm::StringRef /*SearchPath*/,
		llvm::StringRef /*RelativePath*/,
		const clang::Module* /*Imported*/,
		clang::SrcMgr::CharacteristicKind FileType) override {
		if (!File) return;
		if (FileType != clang::SrcMgr::C_User) return;

		const clang::FileEntry& FE = File->getFileEntry();
		llvm::StringRef rp = FE.tryGetRealPathName();
		llvm::StringRef name = rp.empty() ? FE.getName() : rp;

		// ★ .generated. 파일은 deps에서 제외
		if (name.contains(".generated.")) {
			llvm::errs() << "[SKIP] Filtered .generated file: " << name << "\n";
			return;
		}

		llvm::SmallString<260> abs(name);
		llvm::sys::fs::make_absolute(abs);
		llvm::sys::path::remove_dots(abs, true);
		std::string absSlash = llvm::sys::path::convert_to_slash(abs.str());

		if (Uniq.insert(absSlash).second) {
			llvm::errs() << "[ADD] Adding dependency: " << absSlash << "\n";
			Out.emplace_back(std::move(absSlash));
		}
	}
#else
	void InclusionDirective(clang::SourceLocation, const clang::Token&,
		llvm::StringRef /*FileName*/, bool /*IsAngled*/,
		clang::CharSourceRange /*FilenameRange*/,
		const clang::FileEntry* File,
		llvm::StringRef /*SearchPath*/,
		llvm::StringRef /*RelativePath*/,
		const clang::Module* /*Imported*/,
		clang::SrcMgr::CharacteristicKind FileType) override {
		if (!File) return;
		if (FileType != clang::SrcMgr::C_User) return;

		llvm::StringRef rp = File->tryGetRealPathName();
		llvm::StringRef name = rp.empty() ? File->getName() : rp;

		// ★ .generated. 파일은 deps에서 제외
		if (name.contains(".generated.")) {
			llvm::errs() << "[SKIP] Filtered .generated file: " << name << "\n";
			return;
		}

		llvm::SmallString<260> abs(name);
		llvm::sys::fs::make_absolute(abs);
		llvm::sys::path::remove_dots(abs, true);
		std::string absSlash = llvm::sys::path::convert_to_slash(abs.str());

		if (Uniq.insert(absSlash).second) {
			llvm::errs() << "[ADD] Adding dependency: " << absSlash << "\n";
			Out.emplace_back(std::move(absSlash));
		}
	}
#endif

private:
	clang::SourceManager& SM;
	llvm::StringSet<>& Uniq;
	std::vector<std::string>& Out;
};

class DepsOnlyAction : public clang::PreprocessOnlyAction {
public:
	DepsOnlyAction(std::vector<std::string>& out) : Out(out) {}
protected:
	void ExecuteAction() override {
		auto& CI = getCompilerInstance();
		auto& PP = CI.getPreprocessor();
		clang::SourceManager& SM = CI.getSourceManager();

		llvm::StringSet<> Uniq;

		// ★ 메인 파일을 먼저 Out에 추가 (절대경로 + /)
		if (const clang::FileEntry* MainFE = SM.getFileEntryForID(SM.getMainFileID())) {
			llvm::StringRef rp = MainFE->tryGetRealPathName();
			llvm::StringRef name = rp.empty() ? MainFE->getName() : rp;

			// ★ 메인 파일이 .generated.가 아닌 경우만 추가
			if (!name.contains(".generated.")) {
				llvm::SmallString<260> abs(name);
				llvm::sys::fs::make_absolute(abs);
				llvm::sys::path::remove_dots(abs, true);
				std::string absSlash = llvm::sys::path::convert_to_slash(abs.str());

				if (Uniq.insert(absSlash).second) {
					llvm::errs() << "[MAIN] Adding main file: " << absSlash << "\n";
					Out.emplace_back(std::move(absSlash));
				}
			}
			else {
				llvm::errs() << "[SKIP] Main file is .generated, skipping: " << name << "\n";
			}
		}

		// include 콜백 등록해서 사용자 헤더들을 계속 수집
		PP.addPPCallbacks(std::make_unique<DepsCollector>(SM, Uniq, Out));

		// 전처리만 수행
		clang::PreprocessOnlyAction::ExecuteAction();

		// 디버그: 최종 deps 목록 출력
		llvm::errs() << "[FINAL] Total dependencies found: " << Out.size() << "\n";
		for (const auto& dep : Out) {
			llvm::errs() << "  - " << dep << "\n";
		}
	}
private:
	std::vector<std::string>& Out;
};




class DepsOnlyActionFactory : public clang::tooling::FrontendActionFactory {
public:
	explicit DepsOnlyActionFactory(std::vector<std::string>& out) : Out(out) {}
	std::unique_ptr<clang::FrontendAction> create() override {
		return std::make_unique<DepsOnlyAction>(Out); // ← 생성자 인자 전달 가능
	}
private:
	std::vector<std::string>& Out; // 외부 벡터를 참조로 보관
};

















struct ReflectedProperty {
	std::string name;
	std::string type;
	uint64_t offset;
};

struct ReflectedClass {
	std::string name;
	std::string nameWithinNameSpace;
	bool mIsAbstarct;
	std::vector<ReflectedProperty> properties;

	std::string mBaseClassNameList;
	int mBaseClassNameListNum;



};


// 1. AST 방문자 (Visitor)
// AST를 돌아다니면서 우리가 원하는 '깃발'을 찾는 로봇 탐사선입니다.
class MyASTVisitor : public clang::RecursiveASTVisitor<MyASTVisitor> {
public:
	std::vector<ReflectedClass> reflectedClasses;

	// 클래스나 구조체를 발견할 때마다 이 함수가 자동으로 호출됩니다.
	bool VisitCXXRecordDecl(clang::CXXRecordDecl* Declaration) {
		// 'annotate' 속성(우리가 꽂은 깃발)이 있는지 확인합니다.
		if (Declaration->hasAttr<clang::AnnotateAttr>()) {
			clang::AnnotateAttr* Attr = Declaration->getAttr<clang::AnnotateAttr>();
			// 그 깃발의 내용이 "REFLECT_CLASS"가 맞는지 확인합니다.
			if (Attr->getAnnotation() == "REFLECT_CLASS") {

				std::string className = Declaration->getNameAsString();
				ReflectedClass rc;
				rc.name = Declaration->getNameAsString();
				//  llvm::outs() << "Found Reflected Class: " << className << "\n";
				auto* RD = Declaration; // 이미 CXXRecordDecl*
				if (!RD->isCompleteDefinition()) return true;

				rc.mIsAbstarct = Declaration->isAbstract();

				auto& Ctx = RD->getASTContext();
				const clang::ASTRecordLayout& Layout = Ctx.getASTRecordLayout(RD);


				std::vector<std::string> baseClassNameVector;
				Declaration->forallBases([&baseClassNameVector](const clang::CXXRecordDecl* baseDecl) {

					baseClassNameVector.push_back(baseDecl->getNameAsString());
					return true;
					});

				std::stringstream baseClassNameList;
				for (int i = 0; i < baseClassNameVector.size(); ++i)
				{
					baseClassNameList << baseClassNameVector[i] << " ";
				}
				rc.mBaseClassNameList = baseClassNameList.str();
				rc.mBaseClassNameListNum = baseClassNameVector.size();

				rc.nameWithinNameSpace = Declaration->getQualifiedNameAsString();

				// 클래스의 멤버 변수들을 하나씩 살펴봅니다.
				for (auto* Field : RD->fields()) {
					// 멤버 변수에도 "REFLECT_PROPERTY" 깃발이 꽂혀 있는지 확인합니다.
					if (Field->hasAttr<clang::AnnotateAttr>() &&
						Field->getAttr<clang::AnnotateAttr>()->getAnnotation() == "REFLECT_PROPERTY") {

						//  std::string fieldName = Field->getNameAsString();
						//  std::string fieldType = Field->getType().getAsString();

						 // llvm::outs() << "  - Property: " << fieldName << " (Type: " << fieldType << ")\n";


						// ← 여기! 루프 돌 필요 없이 바로 인덱스
						unsigned idx = Field->getFieldIndex();     // 선언 순서 인덱스
						uint64_t bitOff = Layout.getFieldOffset(idx);
						uint64_t byteOff = bitOff / 8;

						rc.properties.push_back({
						   Field->getNameAsString(),
						   Field->getType().getAsString(),
						   byteOff
							});

					}
				}

				// ❗❗ 핵심 누락: 찾은 클래스 저장
				reflectedClasses.push_back(rc);
			}
		}
		return true; // 계속해서 다른 노드도 탐색하라는 의미입니다.
	}


};

// 2. AST 소비자 (Consumer)
// 위에서 만든 로봇 탐사선(Visitor)을 탑재하고, AST가 완성되면 탐사를 시작시키는 역할입니다.
class MyASTConsumer : public clang::ASTConsumer {
private:
	MyASTVisitor Visitor;
	std::string  TUBaseName; // 이 TU의 기본 베이스명 (예: UIType)

public:
	// 소스 파일 전체의 AST가 준비되면 이 함수가 호출됩니다.
	void HandleTranslationUnit(clang::ASTContext& Context) override {


		if (OptDepsOnly.getValue()) {
			// deps-only 모드에서는 아무것도 하지 않음 (전처리만으로 충분)
			return;
		}



		// Visitor를 이용해 AST 전체 탐사를 시작하라고 명령합니다.
		Visitor.TraverseDecl(Context.getTranslationUnitDecl());

		llvm::outs() << "[Debug] Reflected class count: " << Visitor.reflectedClasses.size() << "\n";

		const std::string outDir = MakeEffectiveOutDir();
		std::string baseName = OptBasename.getValue();
		if (baseName.empty()) baseName = TUBaseName;      // ← CMake가 안 준 경우 TU 파일명 기반
		if (baseName.empty()) baseName = "Generated";     // 최후 보루

		const std::string outH = outDir + "/" + baseName + ".generated.h";
		const std::string outCpp = outDir + "/" + baseName + ".generated.cpp";



		// 항상 파일을 생성(빈 파일 방지: 간단한 머리말 작성)
		{
			std::ofstream oh(outH, std::ios::trunc);
			oh << "#pragma once\n";
			oh << "// Generated by CodeGenerator\n\n";
			oh << "#undef GENERATED_BODY\n";
			oh << "#define GENERATED_BODY(TypeName) \\\n";
			oh << "    /* reflection hook */\n\n";


		}

		if (Visitor.reflectedClasses.empty())
		{
			return;
		}


		for (const auto& rc : Visitor.reflectedClasses) {


			std::string generated_h_filename = OutputDir + '/' + rc.name + ".generated.h";
			std::ofstream out_h(generated_h_filename);

			out_h << "#pragma once\n\n";

			out_h << "#undef GENERATED_BODY\n";
			out_h << "#define GENERATED_BODY(ClassType) \\\n";
			// '/* ... */' 주석을 제거하거나 '//' 스타일로 변경하여 friend 선언이 유효하게 들어갈 수 있도록
			out_h << "    friend class ClassInfo; \\\n"; // '/* ... */' 주석 제거!
			out_h << "    friend struct " << rc.name << "Registrar; \\\n";
			out_h << "	public :									\\\n";
			out_h << "		static constexpr const char * GetStaticClassName() { return #ClassType; } \\\n";
			out_h << "		virtual const char * GetClassName(){return #ClassType;}				\\\n";
			out_h << "    // 여기에 메타데이터, 함수 포인터 선언 등 추가 리플렉션 코드를 넣을 수 있습니다. \\\n"; // 주석 스타일 변경

			out_h << "\n"; // 매크로의 마지막에 빈 줄을 추가하여, 매크로 끝을 명확히 함 (안전성을 위해)


			out_h.close(); // .generated.h 파일 닫기
			llvm::outs() << "Generated: " << generated_h_filename << "\n";


			std::string filename = OutputDir + '/' + rc.name + ".generated.cpp";

			std::ofstream out(filename);

			out << "#include \"" << rc.name << ".h\"\n";
			out << "#include <ReflectSystem/ReflectionSystem.h>\n\n";
			out << "#include <ReflectSystem/ReflectionClassInfo.h>\n";
			out << "#include <cstddef>\n\n";             // offsetof 사용을 위해 <cstddef> 포함

			out << "struct " << rc.name << "Registrar {\n";

			//생성자 정의
			out << "    " << rc.name << "Registrar()";
			out << "    " << "{\n";
			out << "Quad::ReflectionSystem* reflectionSystem = Quad::ReflectionSystem::GetInstance(); \n";



			out << "Quad::ClassInfo* cls = reflectionSystem->CreateClassInfo(\"" << rc.name << "\" ," << "sizeof(" << rc.nameWithinNameSpace << ") ";
			if (rc.mIsAbstarct == false)
			{

				out << ", [](void* mem) { return (void*)(new (mem) " << rc.nameWithinNameSpace << "); } " << ", [](void* instance) { ((" << rc.nameWithinNameSpace << "*)(instance))->~" << rc.name << "(); }); \n";

			}
			else
			{
				//추상클래스인경우에는 생성자,소멸자콜백을 등록하지않는다.
				out << ", nullptr " << ", nullptr); \n";
			}

			//base class list 등록
			out << "cls->SetBaseClassList(\"" << rc.mBaseClassNameList << "\"," << rc.mBaseClassNameListNum << "); \n";



			//property등록
			for (const auto& prop : rc.properties) {

				out << "reflectionSystem->AddProperty(cls, Quad::PropertyInfo{\"" << prop.name << "\", \"" << prop.type << "\" , " << prop.offset << "}); \n";
			}






			out << "    }\n";
			out << "};\n";
			out << "static " << rc.name << "Registrar G_" << rc.name << "Registrar;\n";
		}




	}
};

// 3. 프론트엔드 액션 (Action)
// Clang 도구가 파일을 처리할 때 어떤 작업을 할지 정의합니다.
// 여기서는 "우리가 만든 AST 소비자를 생성해서 사용해줘"라고 알려주는 역할입니다.
class MyFrontendAction : public clang::ASTFrontendAction {
public:
	bool BeginSourceFileAction(clang::CompilerInstance& CI) override {
		CI.getFrontendOpts().SkipFunctionBodies = true;  // ★ 함수 본문 파싱 건너뛰기
		// 옵션: 오타 교정/컬러 비활성화로 약간 더 줄이기
		CI.getDiagnosticOpts().ShowColors = false;
		CI.getDiagnosticOpts().ShowNoteIncludeStack = false;
		return true;
	}

	std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(clang::CompilerInstance& CI, llvm::StringRef InFile) override {

		llvm::SmallString<260> p(InFile);
		std::string Base = llvm::sys::path::filename(p).str();
		// 확장자 제거
		llvm::StringRef BaseRef(Base);
		if (BaseRef.ends_with_insensitive(".h") || BaseRef.ends_with_insensitive(".hpp") ||
			BaseRef.ends_with_insensitive(".hh") || BaseRef.ends_with_insensitive(".cpp") ||
			BaseRef.ends_with_insensitive(".cc") || BaseRef.ends_with_insensitive(".cxx")) {
			Base = llvm::sys::path::stem(BaseRef).str();
		}



		return std::make_unique<MyASTConsumer>();
	}
};


// 4. 메인 함수 (프로그램의 시작점)
int main(int argc, const char** argv) {


	//파일들을 출력할 위치를 지정해야한다. 


	// 커맨드 라인에서 입력받은 파일 경로 등을 분석합니다.
	auto ExpectedParser = clang::tooling::CommonOptionsParser::create(argc, argv, MyToolCategory);
	if (!ExpectedParser) {
		llvm::errs() << ExpectedParser.takeError();
		llvm::outs() << "실패\n";
		return 1;
	}
	clang::tooling::CommonOptionsParser& OptionsParser = ExpectedParser.get();

	// Clang 도구를 생성합니다. 분석할 파일 목록을 여기서 가져옵니다.
	clang::tooling::ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());


	// (선택) 성능용 공통 플래그 주입
	//using namespace clang::tooling;
	//Tool.appendArgumentsAdjuster(getInsertArgumentAdjuster("-fsyntax-only", ArgumentInsertPosition::END));
	//Tool.appendArgumentsAdjuster(getInsertArgumentAdjuster("-fskip-function-bodies", ArgumentInsertPosition::END));
	// (선택) 툴용 PCH가 있다면:
	// Tool.appendArgumentsAdjuster(getInsertArgumentAdjuster("-include-pch=.../Reflection.pch", ArgumentInsertPosition::END));

	if (OptDepsOnly) {
		auto Sources = OptionsParser.getSourcePathList();
		if (Sources.empty()) { llvm::errs() << "no input\n"; return 1; }

		std::vector<std::string> deps;                   // 메인 + include된 사용자 헤더들이 담김
		auto Factory = std::make_unique<DepsOnlyActionFactory>(deps);
		int rc = Tool.run(Factory.get());                // 전처리 수행 → 콜백이 deps 채움
		if (rc != 0) return rc;

		// Make 형식 depfile을 쓰려면 둘 다 필요
		if (!OptDepfile.empty() && !OptDepTarget.empty()) {
			// 1) 타겟(stamp) 경로: 절대 + / 로 통일
			llvm::SmallString<260> tgt(OptDepTarget.getValue());
			llvm::sys::fs::make_absolute(tgt);
			llvm::sys::path::remove_dots(tgt, true);
			std::string tgtSlash = llvm::sys::path::convert_to_slash(tgt.str());

			// 2) depfile 출력 디렉터리 생성
			llvm::SmallString<260> out(OptDepfile.getValue());
			auto parent = llvm::sys::path::parent_path(out);
			if (!parent.empty()) llvm::sys::fs::create_directories(parent);

			// 3) deps에서 .generated 파일들 제거
			std::vector<std::string> filteredDeps;
			for (const auto& dep : deps) {
				if (dep.find(".generated.") == std::string::npos) {
					filteredDeps.push_back(dep);
				}
				else {
					llvm::errs() << "[FILTER] Removing .generated dependency: " << dep << "\n";
				}
			}

			// 4) depfile 쓰기: "<target>: <dep1> <dep2> ..."
			std::error_code ec;
			llvm::raw_fd_ostream os(out, ec, llvm::sys::fs::OF_Text);
			if (ec) { llvm::errs() << "write depfile failed: " << ec.message() << "\n"; return 1; }

			os << tgtSlash << ":";               // 타겟
			for (auto& p : filteredDeps) os << " " << p; // 필터링된 deps
			os << "\n";

			// 디버그: 최종 deps 출력
			llvm::errs() << "[DEBUG] Final deps written to " << out << ":\n";
			llvm::errs() << "Target: " << tgtSlash << "\n";
			for (const auto& dep : filteredDeps) {
				llvm::errs() << "  Dep: " << dep << "\n";
			}

			return 0;
		}

		// depfile도 outdeps도 없으면 할 일이 없음
		llvm::errs() << "deps-only mode requires --depfile and --deptarget\n";
		return 1;
	}


	{
		llvm::SmallString<260> dir(OutputDir.getValue());
		if (!dir.empty()) llvm::sys::fs::create_directories(dir);
	}








	llvm::errs() << OutputDir.c_str() << "\n";

	// 우리가 정의한 액션(Action)을 Clang 도구에서 실행시킵니다.
	return Tool.run(clang::tooling::newFrontendActionFactory<MyFrontendAction>().get());
}