#pragma once

#include "EditorDirector/ImportCommonHeader.h"

namespace Quad
{

class EditorAssetImportFinalizer
{
  public:
    static EditorAssetImportFinalizer *GetInstance();

    EditorAssetImportFinalizer();
    ~EditorAssetImportFinalizer();

    void Finalize(const ImportResult &result);

  private:
};
} // namespace Quad
