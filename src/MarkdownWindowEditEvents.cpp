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
}

void MarkdownWindow::OnFindAndReplace(wxCommandEvent& event)
{
}
