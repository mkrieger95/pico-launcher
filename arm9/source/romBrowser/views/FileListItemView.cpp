#include "common.h"
#include "gui/GraphicsContext.h"
#include "gui/input/InputProvider.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "FileListItemView.h"

#define FILE_NAME_LABEL_X       5
#define FILE_NAME_LABEL_Y       2

FileListItemView::FileListItemView(std::unique_ptr<IRomBrowserItemViewModel> viewModel,
    const MaterialColorScheme* materialColorScheme, const IFontRepository* fontRepository)
    : _viewModel(std::move(viewModel))
    , _fileNameLabel(Label2DView::CreateShared(192, 16, 256, fontRepository->GetFont(FontType::Regular10)))
    , _inputHandler(this, _viewModel.get())
    , _materialColorScheme(materialColorScheme)
{
    _fileNameLabel->SetEllipsisStyle(LabelView::EllipsisStyle::Ellipsis);
    AddChildTail(_fileNameLabel.GetPointer());
}

void FileListItemView::Update()
{
    _viewModel->DisposeQueueTaskWhenComplete();
    _fileNameLabel->SetPosition(_position.x + FILE_NAME_LABEL_X, _position.y + FILE_NAME_LABEL_Y);
    _fileNameLabel->SetEllipsisStyle(IsFocused()
        ? LabelView::EllipsisStyle::Marquee
        : LabelView::EllipsisStyle::Ellipsis);
    ViewContainer::Update();
}

void FileListItemView::Draw(GraphicsContext& graphicsContext)
{
    if (!graphicsContext.IsVisible(GetBounds()))
        return;

    if (IsFocused())
    {
        _fileNameLabel->SetBackgroundColor(_materialColorScheme->mainIconBg);
        _fileNameLabel->SetForegroundColor(_materialColorScheme->onSecondaryContainer);
    }
    else
    {
        _fileNameLabel->SetBackgroundColor(_materialColorScheme->surfaceBright);
        _fileNameLabel->SetForegroundColor(_materialColorScheme->onSurface);
    }

    ViewContainer::Draw(graphicsContext);
}

SharedPtr<View> FileListItemView::MoveFocus(const SharedPtr<View>& currentFocus,
    FocusMoveDirection direction, View* source)
{
    if (GetParent())
    {
        return GetParent()->MoveFocus(SharedFromThis(), direction, this);
    }

    return nullptr;
}

bool FileListItemView::HandleInput(const InputProvider& inputProvider, FocusManager& focusManager)
{
    return _inputHandler.HandleInput(inputProvider, focusManager)
        || View::HandleInput(inputProvider, focusManager);
}

void FileListItemView::HandlePenDown(const Point& touchPoint, FocusManager& focusManager)
{
    _inputHandler.HandlePenDown(touchPoint, focusManager);
}

void FileListItemView::HandlePenMove(const Point& touchPoint, FocusManager& focusManager)
{
    _inputHandler.HandlePenMove(touchPoint, focusManager);
}

void FileListItemView::HandlePenUp(const Point& lastTouchPoint, FocusManager& focusManager)
{
    _inputHandler.HandlePenUp(lastTouchPoint, focusManager);
}
