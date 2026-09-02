#pragma once

#include <EditorDirector/ClassGenerator.h>
#include <EditorDirector/ImportCommonHeader.h>

#include "CoreBase/CallbackSystem.h"

namespace UI
{
class UICanvas;
class UIImage;
class UIText;
class UIButton;
class UIElement;

} // namespace UI

class ITaskUIController
{
  public:
    ITaskUIController();
    virtual ~ITaskUIController() = 0;

    //   virtual void Update(TaskContext *context) = 0;
    virtual void Update() = 0;
    virtual void Initialize(UI::UICanvas *canvas) = 0;

    virtual void SetActive(bool flag) = 0;
    virtual void SetPosition(float x, float y) = 0;
    virtual void RequestKeyboardCapture() = 0;
    virtual void RequestMouseCapture() = 0;

    virtual void ReleaseKeyboardCapture() = 0;
    virtual void ReleaseMouseCapture() = 0;

    void SetUIUpdateFlag(bool flag)
    {
        mUIActiveFlag = flag;
    }

    bool GetUIUpdateFlag() const
    {
        return mUIActiveFlag;
    }

  private:
    bool mUIActiveFlag = false;
};

using OnClickedExitButtonCallbackSystem = Core::MultiCallbackSystem<>;

class ClassGenerationTaskUIController : public ITaskUIController
{
  public:
    static ClassGenerationTaskUIController *GetInstance();

    ClassGenerationTaskUIController();
    ~ClassGenerationTaskUIController();

    void Initialize(UI::UICanvas *canvas);

    // virtual void Update(TaskContext *context) override;

    virtual void Update();

    void SetMessage(const std::string &message);
    void SetProgress(float progress);

    virtual void SetActive(bool flag) override;
    virtual void SetPosition(float x, float y) override;
    virtual void RequestKeyboardCapture() override;
    virtual void RequestMouseCapture() override;

    virtual void ReleaseKeyboardCapture() override;
    virtual void ReleaseMouseCapture() override;

    void SetGenerationTaskHandle(ClassGenerationTaskHandle handle);
    OnClickedExitButtonCallbackSystem mOnClickedExitButtonCallbackSystem;

  private:
    void CreateExitButton(UI::UIElement *parentElement);
    void OnClickedExitButton();

  private:
    // UI
    UI::UIImage *mBackgroundUI = nullptr;
    UI::UIText *mProgressText = nullptr;
    UI::UIButton *mExitButton = nullptr;
};

class ImportTaskUIController : public ITaskUIController
{
  public:
    static ImportTaskUIController *GetInstance();
    ImportTaskUIController();
    virtual ~ImportTaskUIController();

    virtual void Initialize(UI::UICanvas *canvas) override;
    // virtual void Update(TaskContext *context) override;

    virtual void Update() override;

    virtual void SetActive(bool flag) override;
    virtual void SetPosition(float x, float y) override;
    virtual void RequestKeyboardCapture() override;
    virtual void RequestMouseCapture() override;

    virtual void ReleaseKeyboardCapture() override;
    virtual void ReleaseMouseCapture() override;

    void SetMessage(const std::string &message);
    void SetProgress(float progress);
    OnClickedExitButtonCallbackSystem mOnClickedExitButtonCallbackSystem;

  private:
    void CreateExitButton(UI::UIElement *parent);
    void OnClickedExitButton();

  private:
    UI::UIImage *mBackgroundUI = nullptr;
    UI::UIText *mProgressText = nullptr;
    UI::UIButton *mExitButton = nullptr;

  private:
};
