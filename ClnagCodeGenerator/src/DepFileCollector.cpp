#include "DepFileCollector.h"

void Quad::DepFileCollector::InclusionDirective(clang::SourceLocation, const clang::Token &,
                                                llvm::StringRef /*FileName*/, bool /*IsAngled*/,
                                                clang::CharSourceRange /*FilenameRange*/,
                                                clang::OptionalFileEntryRef File, llvm::StringRef /*SearchPath*/,
                                                llvm::StringRef /*RelativePath*/, const clang::Module * /*Imported*/,
                                                clang::SrcMgr::CharacteristicKind FileType)
{

#if LLVM_VERSION_MAJOR >= 15
    {
        if (!File)
            return;
        if (FileType != clang::SrcMgr::C_User)
            return;

        const clang::FileEntry &FE = File->getFileEntry();
        llvm::StringRef rp = FE.tryGetRealPathName();
        llvm::StringRef name = rp.empty() ? FE.getName() : rp;

        // ★ .generated. 파일은 deps에서 제외
        if (name.contains(".generated."))
        {
            llvm::errs() << "[SKIP] Filtered .generated file: " << name << "\n";
            return;
        }

        llvm::SmallString<260> abs(name);
        llvm::sys::fs::make_absolute(abs);
        llvm::sys::path::remove_dots(abs, true);
        std::string absSlash = llvm::sys::path::convert_to_slash(abs.str());

        if (Uniq.insert(absSlash).second)
        {
            llvm::errs() << "[ADD] Adding dependency: " << absSlash << "\n";
            Out.emplace_back(std::move(absSlash));
        }
    }
#else
    void InclusionDirective(clang::SourceLocation, const clang::Token &, llvm::StringRef /*FileName*/,
                            bool /*IsAngled*/, clang::CharSourceRange /*FilenameRange*/, const clang::FileEntry *File,
                            llvm::StringRef /*SearchPath*/, llvm::StringRef /*RelativePath*/,
                            const clang::Module * /*Imported*/, clang::SrcMgr::CharacteristicKind FileType) override
    {
        if (!File)
            return;
        if (FileType != clang::SrcMgr::C_User)
            return;

        llvm::StringRef rp = File->tryGetRealPathName();
        llvm::StringRef name = rp.empty() ? File->getName() : rp;

        // ★ .generated. 파일은 deps에서 제외
        if (name.contains(".generated."))
        {
            llvm::errs() << "[SKIP] Filtered .generated file: " << name << "\n";
            return;
        }

        llvm::SmallString<260> abs(name);
        llvm::sys::fs::make_absolute(abs);
        llvm::sys::path::remove_dots(abs, true);
        std::string absSlash = llvm::sys::path::convert_to_slash(abs.str());

        if (Uniq.insert(absSlash).second)
        {
            llvm::errs() << "[ADD] Adding dependency: " << absSlash << "\n";
            Out.emplace_back(std::move(absSlash));
        }
    }

#endif
}
