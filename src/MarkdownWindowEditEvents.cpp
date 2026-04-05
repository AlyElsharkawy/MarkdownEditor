#include "MarkdownWindow.h"
#include <wx/event.h>
#include <wx/fdrepdlg.h>
#include <wx/msgdlg.h>

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

  wxString findString = event.GetFindString();
  int flags = event.GetFlags();
  int resultFlags = 0;
  if(flags & wxFR_MATCHCASE)
    resultFlags |= wxFR_MATCHCASE;
  if(flags & wxFR_WHOLEWORD)
    resultFlags |= wxFR_WHOLEWORD;
  this->textCtrl->SetSearchFlags(resultFlags);
  
  wxEventType eventType = event.GetEventType();
  if(type == wxEVT_FIND || type == wxEVT_FIND_NEXT)
  {
    long startingPosition = this->textCtrl->GetCurrentPos();
    long endingPosition = this->textCtrl->GetLastPosition();

    //If we are searching upwards, then make start the current position
    if(!(flags & wxFR_DOWN)) 
    {
      endingPosition = 0;
      startingPosition = this->textCtrl->GetAnchor();
    }

    this->textCtrl->SetTargetStart(startingPosition);
    this->textCtrl->SetTargetEnd(endingPosition);
    int finalPosition = this->textCtrl->SearchInTarget(findString);
    if(finalPosition != wxNOT_FOUND)
    {
      this->textCtrl->SetSelection(this->textCtrl->GetTargetStart(), this->textCtrl->GetTargetEnd());
      this->textCtrl->EnsureCaretVisible();
    }
    else 
    {
      SetStatusText("Cannot find: \"" + findString + "", 0);
      wxMessageBox("Cannot find text: \"" + findString + "\"", "Find Result", wxICON_INFORMATION | wxOK);
    }
  }

  else if (type == wxEVT_FIND_REPLACE)
  {
    wxString replaceStr = event.GetReplaceString();
    wxString selectedText = this->textCtrl->GetStringSelection();
    // Ensure the user actually has the search string selected before replacing it
    bool match = (flags & wxFR_MATCHCASE) ? (selectedText == findString) : (selectedText.Lower() == findString.Lower());
    if (match)
    {
      this->textCtrl->ReplaceSelection(replaceStr);
    }
        
    // Automatically trigger a "Find Next" so the user can keep clicking "Replace"
    wxFindDialogEvent nextEvent(wxEVT_FIND_NEXT, event.GetId());
    nextEvent.SetFlags(flags);
    nextEvent.SetFindString(findString);
    OnFindReplaceDialogEvent(nextEvent);
  }
    
    // 5. Handle 'Replace All'
  else if (type == wxEVT_FIND_REPLACE_ALL)
  {
    wxString replaceStr = event.GetReplaceString();
    int replaceCount = 0;
        
    // Set the target to the entire document
    this->textCtrl->SetTargetStart(0);
    this->textCtrl->SetTargetEnd(this->textCtrl->GetLastPosition());
        
    // Loop through and replace every instance
    while (this->textCtrl->SearchInTarget(findString) != wxNOT_FOUND)
    {
      this->textCtrl->ReplaceTarget(replaceStr);
      replaceCount++;
            
      // Move target start forward so we don't get stuck in an infinite loop
      this->textCtrl->SetTargetStart(this->textCtrl->GetTargetEnd());
      this->textCtrl->SetTargetEnd(this->textCtrl->GetLastPosition());
    }
        
    wxMessageBox(wxString::Format("Replaced %d occurrences.", replaceCount), "Replace All", wxOK | wxICON_INFORMATION, this);
    }
}
