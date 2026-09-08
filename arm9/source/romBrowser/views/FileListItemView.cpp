#include "common.h"
#include "gui/GraphicsContext.h"
#include "gui/IVramManager.h"
#include "gui/OamBuilder.h"
#include "gui/OamManager.h"
#include "gui/VramContext.h"
#include "gui/input/InputProvider.h"
#include "gui/palette/GradientPalette.h"
#include "themes/material/MaterialColorScheme.h"
#include "themes/IFontRepository.h"
#include "FileListItemView.h"

#define FILE_NAME_LABEL_X       5
#define FILE_NAME_LABEL_Y       2
#define SELECTOR_WIDTH          208
#define SELECTOR_HEIGHT         16

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

void FileListItemView::InitVram(const VramContext& vramContext)
{
    const auto objVramManager = vramContext.GetObjVramManager();
    if (objVramManager)
    {
        // A single solid 32x16 4bpp tile is reused across the selector sprites.
        u32 selectorTiles[64];
        for (u32& tile : selectorTiles)
        {
            tile = 0x11111111;
        }

        _selectorVramOffset = objVramManager->Alloc(sizeof(selectorTiles));
        dma_ntrCopy32(3, selectorTiles,
            objVramManager->GetVramAddress(_selectorVramOffset), sizeof(selectorTiles));
    }

    ViewContainer::InitVram(vramContext);
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
        const auto selectorColor = _materialColorScheme->mainIconBg;
        const auto selectorPaletteRow = graphicsContext.GetPaletteManager().AllocRow(
            GradientPalette(selectorColor, selectorColor),
            _position.y + FILE_NAME_LABEL_Y,
            _position.y + FILE_NAME_LABEL_Y + SELECTOR_HEIGHT);

        auto selectorOams = graphicsContext.GetOamManager().AllocOams(7);
        for (int i = 0; i < 6; i++)
        {
            OamBuilder::OamWithSize<32, 16>(
                    _position.x + i * 32,
                    _position.y + FILE_NAME_LABEL_Y,
                    _selectorVramOffset >> 7)
                .WithPalette16(selectorPaletteRow)
                .WithPriority(graphicsContext.GetPriority())
                .Build(selectorOams[i]);
        }
        OamBuilder::OamWithSize<16, 16>(
                _position.x + 6 * 32,
                _position.y + FILE_NAME_LABEL_Y,
                _selectorVramOffset >> 7)
            .WithPalette16(selectorPaletteRow)
            .WithPriority(graphicsContext.GetPriority())
            .Build(selectorOams[6]);

        _fileNameLabel->SetBackgroundColor(selectorColor);
        _fileNameLabel->SetForegroundColor(_materialColorScheme->onSecondaryContainer);
    }
    else
    {
        _fileNameLabel->SetBackgroundColor(_materialColorScheme->surfaceBright);
        _fileNameLabel->SetForegroundColor(_materialColorScheme->onSurface);
    }

    // Labels are allocated after the selector OAMs, placing the text above the bar.
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
