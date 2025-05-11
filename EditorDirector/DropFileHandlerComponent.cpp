#include "DropFileHandlerComponent.h"
#include"DockingWindowController.h"
Quad::DropFileHandlerComponent::DropFileHandlerComponent(DockingWindowController* controller)
    :mController(controller)
{

}

LRESULT Quad::DropFileHandlerComponent::HandleMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,bool & ret)
{
    switch (msg)
    {

    case WM_DROPFILES:
    {

        HDROP hdrop = (HDROP)wParam;
        int fileNum = DragQueryFileA(hdrop, 0xFFFFFFFF, nullptr, 0);
        char fileNameBuffer[300];
        std::vector<std::string> dropFileVector;
        for (int i = 0; i < fileNum; ++i)
        {
            int fileNameLength = DragQueryFileA(hdrop, i, NULL, 0);
            DragQueryFileA(hdrop, i, fileNameBuffer, fileNameLength + 1);
            dropFileVector.push_back(fileNameBuffer);
        }

        mController->DragAndDropFile(dropFileVector);

        ret = true;
    }

    }

    return 0;

}
