#include "MarkdownWindow.h"
#include <wx/event.h>

void MarkdownWindow::OnUndo(wxCommandEvent& event)
{
  if(this->textCtrl->CanUndo())
  {
    this->textCtrl->Undo();
  }
}

void MarkdownWindow::OnRedo(wxCommandEvent& event)
{
  if(this->textCtrl->CanRedo())
  {
    this->textCtrl->Redo();
  }
}

void MarkdownWindow::OnPreferences(wxCommandEvent& event)
{
}

void MarkdownWindow::OnFind(wxCommandEvent& event)
{
  if (this->findDialog)
  {
    this->findDialog->Show(true);
    this->findDialog->Raise();
    return;
  }
  // Create the dialog in "Find Only" mode
  this->findDialog = new wxFindReplaceDialog(this, &this->findData, "Find Text", 0);
  this->findDialog->Show(true);
}

void MarkdownWindow::OnFindAndReplace(wxCommandEvent& event)
{
  if (this->findDialog)
  {
    // If it's already open, destroy it so we can recreate it with replace fields
    this->findDialog->Destroy();
    this->findDialog = nullptr;
  }
  // Create the dialog with the replace fields enabled
  this->findDialog = new wxFindReplaceDialog(this, &this->findData, "Find and Replace", wxFR_REPLACEDIALOG);
  this->findDialog->Show(true);
}
  
void MarkdownWindow::OnFindReplaceDialogEvent(wxFindDialogEvent& event)
{
  wxEventType type = event.GetEventType();
  // 1. Handle Dialog Closing
  if (type == wxEVT_FIND_CLOSE)
  {
    this->findDialog->Destroy();
    this->findDialog = nullptr;
    return;
  }
}
