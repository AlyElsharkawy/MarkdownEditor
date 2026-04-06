#include <wx/event.h>
#include <wx/fdrepdlg.h>
#include <wx/msgdlg.h>
#include "MarkdownWindow.h"

void MarkdownWindow::OnUndo(wxCommandEvent& event)
{
  if(this->styledWindows[CURRENT_TAB]->CanUndo())
  {
    this->styledWindows[CURRENT_TAB]->Undo();
  }
}

void MarkdownWindow::OnRedo(wxCommandEvent& event)
{
  if(this->styledWindows[CURRENT_TAB]->CanRedo())
  {
    this->styledWindows[CURRENT_TAB]->Redo();
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
  this->styledWindows[CURRENT_TAB]->SetSearchFlags(resultFlags);
  
  wxEventType eventType = event.GetEventType();
  if(type == wxEVT_FIND || type == wxEVT_FIND_NEXT)
  {
    long startingPosition = this->styledWindows[CURRENT_TAB]->GetCurrentPos();
    long endingPosition = this->styledWindows[CURRENT_TAB]->GetLastPosition();

    //If we are searching upwards, then make start the current position
    if(!(flags & wxFR_DOWN)) 
    {
      endingPosition = 0;
      startingPosition = this->styledWindows[CURRENT_TAB]->GetAnchor();
    }

    this->styledWindows[CURRENT_TAB]->SetTargetStart(startingPosition);
    this->styledWindows[CURRENT_TAB]->SetTargetEnd(endingPosition);
    int finalPosition = this->styledWindows[CURRENT_TAB]->SearchInTarget(findString);
    if(finalPosition != wxNOT_FOUND)
    {
      this->styledWindows[CURRENT_TAB]->SetSelection(this->styledWindows[CURRENT_TAB]->GetTargetStart(), this->styledWindows[CURRENT_TAB]->GetTargetEnd());
      this->styledWindows[CURRENT_TAB]->EnsureCaretVisible();
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
    wxString selectedText = this->styledWindows[CURRENT_TAB]->GetStringSelection();
    // Ensure the user actually has the search string selected before replacing it
    bool match = (flags & wxFR_MATCHCASE) ? (selectedText == findString) : (selectedText.Lower() == findString.Lower());
    if (match)
    {
      this->styledWindows[CURRENT_TAB]->ReplaceSelection(replaceStr);
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
    this->styledWindows[CURRENT_TAB]->SetTargetStart(0);
    this->styledWindows[CURRENT_TAB]->SetTargetEnd(this->styledWindows[CURRENT_TAB]->GetLastPosition());
        
    // Loop through and replace every instance
    while (this->styledWindows[CURRENT_TAB]->SearchInTarget(findString) != wxNOT_FOUND)
    {
      this->styledWindows[CURRENT_TAB]->ReplaceTarget(replaceStr);
      replaceCount++;
            
      // Move target start forward so we don't get stuck in an infinite loop
      this->styledWindows[CURRENT_TAB]->SetTargetStart(this->styledWindows[CURRENT_TAB]->GetTargetEnd());
      this->styledWindows[CURRENT_TAB]->SetTargetEnd(this->styledWindows[CURRENT_TAB]->GetLastPosition());
    }
        
    wxMessageBox(wxString::Format("Replaced %d occurrences.", replaceCount), "Replace All", wxOK | wxICON_INFORMATION, this);
    }
}
