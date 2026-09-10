#include "EditorUtility.h"
#include <Utility/Utility.h>
EditorUtility::~EditorUtility() {}

#ifdef D3DX
#include <Shobjidl.h >

std::string EditorUtility::OpenFileDialog(void *windowHandle, const char *filter)
{
    IFileOpenDialog *pFileOpen = nullptr;

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void **)&pFileOpen);
    std::wstring ret;
    if (SUCCEEDED(hr))
    {

        COMDLG_FILTERSPEC filterSpec[] = {{L"Image Files", L"*.jpg; *.jpeg; *.bmp; *.png; *.fbx;"}};
        pFileOpen->SetFileTypes(1, filterSpec);
        hr = pFileOpen->Show(static_cast<HWND>(windowHandle));

        if (SUCCEEDED(hr))
        {

            IShellItem *pItem = nullptr;
            hr = pFileOpen->GetResult(&pItem);

            if (SUCCEEDED(hr))
            {
                PWSTR pwfilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwfilePath);

                if (SUCCEEDED(hr))
                {
                    ret = pwfilePath;
                }

                CoTaskMemFree(pwfilePath);
            }

            pItem->Release();
        }

        pFileOpen->Release();

        return CoreUtility::Utility::ConvertToString(ret, true);
    }
    return "";
}

std::string EditorUtility::OpenFolderDialog(void *windowHandle)
{
    IFileOpenDialog *pFileOpen = nullptr;

    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, (void **)&pFileOpen);
    std::wstring ret;
    if (SUCCEEDED(hr))
    {

        FILEOPENDIALOGOPTIONS options;
        pFileOpen->GetOptions(&options);

        pFileOpen->SetOptions(options | FOS_PICKFOLDERS);

        hr = pFileOpen->Show(static_cast<HWND>(windowHandle));

        if (SUCCEEDED(hr))
        {

            IShellItem *pItem = nullptr;
            hr = pFileOpen->GetResult(&pItem);

            if (SUCCEEDED(hr))
            {
                PWSTR pwfilePath;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pwfilePath);

                if (SUCCEEDED(hr))
                {
                    ret = pwfilePath;
                }

                CoTaskMemFree(pwfilePath);
            }

            pItem->Release();
        }

        pFileOpen->Release();

        return CoreUtility::Utility::ConvertToString(ret, true);
    }
    return "";
}

#endif