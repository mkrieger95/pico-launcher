#pragma once
#include <memory>
#include "gui/views/ViewContainer.h"
#include "gui/views/Label2DView.h"
#include "romBrowser/viewModels/IRomBrowserItemViewModel.h"
#include "RomBrowserItemInputHandler.h"

class MaterialColorScheme;
class IFontRepository;

class FileListItemView : public ViewContainer
{
    SHARED_ONLY(FileListItemView)

public:
    void Update() override;
    void Draw(GraphicsContext& graphicsContext) override;

    SharedPtr<View> MoveFocus(const SharedPtr<View>& currentFocus,
        FocusMoveDirection direction, View* source) override;

    bool HandleInput(const InputProvider& inputProvider, FocusManager& focusManager) override;
    void HandlePenDown(const Point& touchPoint, FocusManager& focusManager) override;
    void HandlePenMove(const Point& touchPoint, FocusManager& focusManager) override;
    void HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager) override;

    Rectangle GetBounds() const override
    {
        return Rectangle(_position, 203, 20);
    }

    void SetFileName(const char* fileName)
    {
        _fileNameLabel->SetText(fileName);
    }

    IRomBrowserItemViewModel& GetViewModel() const
    {
        return *_viewModel;
    }

private:
    std::unique_ptr<IRomBrowserItemViewModel> _viewModel;
    SharedPtr<Label2DView> _fileNameLabel;
    RomBrowserItemInputHandler _inputHandler;
    const MaterialColorScheme* _materialColorScheme;

    FileListItemView(std::unique_ptr<IRomBrowserItemViewModel> viewModel,
        const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository);
};
