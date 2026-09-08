#include "common.h"
#include "../FileInfoManager.h"
#include "../Theme/IRomBrowserViewFactory.h"
#include "romBrowser/viewModels/RomBrowserItemViewModel.h"
#include "FileListFileRecyclerAdapter.h"

void FileListFileRecyclerAdapter::GetViewSize(int& width, int& height) const
{
    width = 203;
    height = 20;
}

SharedPtr<View> FileListFileRecyclerAdapter::CreateView() const
{
    return _romBrowserViewFactory->CreateFileListItemView(
        std::make_unique<RomBrowserItemViewModel>(_romBrowserController));
}

void FileListFileRecyclerAdapter::BindView(SharedPtr<View> view, int index) const
{
    auto listItemView = static_cast<FileListItemView*>(view.GetPointer());
    listItemView->GetViewModel().SetIndex(index);
    listItemView->SetFileName(_fileInfoManager->GetItem(index).GetFileName());
}

TaskResult<void> FileListFileRecyclerAdapter::BindView(SharedPtr<View> view, int index,
    const InternalFileInfo* internalFileInfo, const vu8& cancelRequested) const
{
    BindView(std::move(view), index);
    return TaskResult<void>::Completed();
}

void FileListFileRecyclerAdapter::SetQueueTask(const SharedPtr<View>& view, QueueTask<void> queueTask) const
{
    auto listItemView = static_cast<FileListItemView*>(view.GetPointer());
    listItemView->GetViewModel().SetQueueTask(std::move(queueTask));
}

void FileListFileRecyclerAdapter::ReleaseView(SharedPtr<View> view, int index) const
{
    auto listItemView = static_cast<FileListItemView*>(view.GetPointer());
    listItemView->GetViewModel().SetIndex(-1);
    listItemView->GetViewModel().CancelQueueTask();
}
