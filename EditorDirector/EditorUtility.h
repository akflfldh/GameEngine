#pragma once

#include <string>
class EditorUtility
{
  public:
    ~EditorUtility();

    static std::string OpenFileDialog(void *windowHandle,
                                      const char *filter = "All Files\0*.*\0FBX Model\0*.fbx\0Texture\0*.png;*.jpg\0");

  private:
    EditorUtility();
};
