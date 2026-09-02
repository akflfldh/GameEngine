#include "EditorFontImporter.h"
#include "CoreAsset/IntermediateAsset.h"
#include <CoreAsset/Asset.h>
#include <CoreAsset/AssetImporterManager.h>
#include <EditorDirector/EditorAssetImporterManager.h>;
#include <JsonParserWrapping/JsonParser.h>
#include <Utility/Utility.h>

Quad::EditorFontImporter *Quad::EditorFontImporter::GetInstance()
{

    static EditorFontImporter instance;
    return &instance;
}

Quad::EditorFontImporter::EditorFontImporter() {}

Quad::EditorFontImporter::~EditorFontImporter() {}

CoreAsset::ImportPackage Quad::EditorFontImporter::Import(
    const std::filesystem::path &filepath, CoreAsset::AssetImporterManager *importerManager,
    const CoreAsset::ImportExecutionContext &executionContext) const
{

    // fontdata.json파일 read ,

    // texture altas read

    JsonParser::Parser jsonParser;
    std::string pathStr = filepath.string();

    bool ret = jsonParser.ReadFile(pathStr);
    if (!ret)
    {
        return {};
    }

    std::vector<CoreAsset::FontGlyph> glyphVec;
    CoreAsset::FontMatrix fontMatrix;
    CoreAsset::FontAltas fontAtlas;
    while (1)
    {
        const char *key = jsonParser.GetKeyFromReadPointer();

        if (strcmp(key, "atlas") == 0)
        {
            jsonParser.IntoReadPointer();

            do
            {
                key = jsonParser.GetKeyFromReadPointer();

                if (strcmp(key, "type") == 0)
                {
                }
                else if (strcmp(key, "distanceRange") == 0)
                {
                    jsonParser.GetValueFromReadPointer<int>(fontAtlas.mDistanceRange);
                }
                else if (strcmp(key, "distanceRangeMiddle") == 0)
                {
                    int i = 0;
                }
                else if (strcmp(key, "size") == 0)
                {
                    jsonParser.GetValueFromReadPointer<float>(fontAtlas.mSize);
                }
                else if (strcmp(key, "width") == 0)
                {
                    jsonParser.GetValueFromReadPointer<uint32_t>(fontAtlas.mWidth);
                }
                else if (strcmp(key, "height") == 0)
                {
                    jsonParser.GetValueFromReadPointer<uint32_t>(fontAtlas.mHeight);
                }
                else if (strcmp(key, "yOrigin") == 0)
                {
                    int i = 0;
                }

            } while (jsonParser.NextReadPointer());

            jsonParser.OutReadPointer();
        }
        else if (strcmp(key, "metrics") == 0)
        {
            jsonParser.IntoReadPointer();
            do
            {
                key = jsonParser.GetKeyFromReadPointer();

                if (strcmp(key, "emSize") == 0)
                {
                }
                else if (strcmp(key, "lineHeight") == 0)
                {
                    jsonParser.GetValueFromReadPointer<float>(fontMatrix.mLineHeight);
                }
                else if (strcmp(key, "ascender") == 0)
                {
                    jsonParser.GetValueFromReadPointer<float>(fontMatrix.mAscender);
                }
                else if (strcmp(key, "descender") == 0)
                {
                    jsonParser.GetValueFromReadPointer<float>(fontMatrix.mDescender);
                }
                else if (strcmp(key, "underlineY") == 0)
                {
                    jsonParser.GetValueFromReadPointer<float>(fontMatrix.mUnderlineY);
                }
                else if (strcmp(key, "underlineThickness") == 0)
                {
                    int i = 0;
                }

            } while (jsonParser.NextReadPointer());
            jsonParser.OutReadPointer();
        }
        else if (strcmp(key, "glyphs") == 0)
        {
            jsonParser.IntoReadPointer();
            while (1)
            {
                CoreAsset::FontGlyph fontGlyph;
                jsonParser.IntoReadPointer();

                while (1)
                {
                    const char *key = jsonParser.GetKeyFromReadPointer();
                    if (strcmp(key, "unicode") == 0)
                    {
                        jsonParser.GetValueFromReadPointer<uint32_t>(fontGlyph.mUnicode);
                    }
                    else if (strcmp(key, "advance") == 0)
                    {
                        jsonParser.GetValueFromReadPointer<float>(fontGlyph.mAdvance);
                    }
                    else if (strcmp(key, "planeBounds") == 0)
                    {
                        jsonParser.IntoReadPointer();

                        do
                        {
                            key = jsonParser.GetKeyFromReadPointer();

                            if (strcmp(key, "left") == 0)
                            {
                                jsonParser.GetValueFromReadPointer<float>(fontGlyph.mPlaneBounds.mLeft);
                            }
                            else if (strcmp(key, "bottom") == 0)
                            {
                                jsonParser.GetValueFromReadPointer<float>(fontGlyph.mPlaneBounds.mBottom);
                            }
                            else if (strcmp(key, "right") == 0)
                            {
                                jsonParser.GetValueFromReadPointer<float>(fontGlyph.mPlaneBounds.mRight);
                            }
                            else if (strcmp(key, "top") == 0)
                            {
                                jsonParser.GetValueFromReadPointer<float>(fontGlyph.mPlaneBounds.mTop);
                            }

                        } while (jsonParser.NextReadPointer());

                        jsonParser.OutReadPointer();
                    }
                    else if (strcmp(key, "atlasBounds") == 0)
                    {
                        jsonParser.IntoReadPointer();

                        do
                        {
                            key = jsonParser.GetKeyFromReadPointer();

                            if (strcmp(key, "left") == 0)
                            {
                                jsonParser.GetValueFromReadPointer<float>(fontGlyph.mAltasBounds.mLeft);
                            }
                            else if (strcmp(key, "bottom") == 0)
                            {
                                jsonParser.GetValueFromReadPointer<float>(fontGlyph.mAltasBounds.mBottom);
                            }
                            else if (strcmp(key, "right") == 0)
                            {
                                jsonParser.GetValueFromReadPointer<float>(fontGlyph.mAltasBounds.mRight);
                            }
                            else if (strcmp(key, "top") == 0)
                            {
                                jsonParser.GetValueFromReadPointer<float>(fontGlyph.mAltasBounds.mTop);
                            }

                        } while (jsonParser.NextReadPointer());

                        jsonParser.OutReadPointer();
                    }

                    if (jsonParser.NextReadPointer() == false)
                    {
                        jsonParser.OutReadPointer();

                        break;
                    }
                }

                if (jsonParser.NextReadPointer() == false)
                {

                    jsonParser.OutReadPointer();
                    break;
                }

                glyphVec.push_back(fontGlyph);
            }
            jsonParser.OutReadPointer();
        }

        if (jsonParser.NextReadPointer() == false)
        {
            break;
        }
    }

    for (int i = 0; i < glyphVec.size(); ++i)
    {

#ifdef D3DX
        glyphVec[i].mAltasBounds.mBottom /= fontAtlas.mWidth;
        glyphVec[i].mAltasBounds.mTop /= fontAtlas.mWidth;
        glyphVec[i].mAltasBounds.mRight /= fontAtlas.mWidth;
        glyphVec[i].mAltasBounds.mLeft /= fontAtlas.mWidth;

#endif
        glyphVec[i].mAltasBounds.mTop = 1.0f - glyphVec[i].mAltasBounds.mTop;
        glyphVec[i].mAltasBounds.mBottom = 1.0f - glyphVec[i].mAltasBounds.mBottom;
    }

    fontAtlas.mPxRange = fontAtlas.mDistanceRange / fontAtlas.mSize;

    std::string path = CoreUtility::Utility::GetParentFolderPathFromPath(pathStr);
    std::string filename = CoreUtility::Utility::GetFileNameFromPath(pathStr);

    std::string glyphTexturePath = pathStr + "\\" + filename + "_atlas.png";

    // EditorAssetImporterModule *editorImporterModule = EditorAssetImporterModule::GetInstance();

    EditorAssetImporterManager *editorAssetImporterManager = EditorAssetImporterManager::GetInstance();

    //  std::vector<CoreAsset::Asset *> assetList = editorImporterModule->Import(glyphTexturePath.c_str());

    ImportResult importResult =
        editorAssetImporterManager->ImportDedencySync(glyphTexturePath.c_str(), executionContext.bEngineAsset);

    std::unique_ptr<CoreAsset::IntermediateFont> intermeidateFont = std::make_unique<CoreAsset::IntermediateFont>();
    intermeidateFont->mGlyphAltasID = importResult.mAssets[0]->GetID();
    intermeidateFont->mGlyphVector = std::move(glyphVec);
    intermeidateFont->mFontMatrix = fontMatrix;
    intermeidateFont->mFontAltas = fontAtlas;
    intermeidateFont->mAssetName = filename.c_str();

    std::vector<std::unique_ptr<CoreAsset::IntermediateAsset>> IntermediateAssetVec;
    IntermediateAssetVec.push_back(std::move(intermeidateFont));

    CoreAsset::ImportPackage importPackage;
    importPackage.mInteremdiateAssets.resize(IntermediateAssetVec.size());

    for (int i = 0; i < IntermediateAssetVec.size(); ++i)
    {
        importPackage.mInteremdiateAssets[i].mIntermediateAsset = std::move(IntermediateAssetVec[i]);
    }

    return importPackage;
}
