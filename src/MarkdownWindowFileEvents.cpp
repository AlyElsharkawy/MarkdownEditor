#include <wx/event.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include <wx/stc/stc.h>
#include <wx/utils.h>
#include <wx/config.h>
#include <fstream>
#include "MarkdownWindow.h"

void MarkdownWindow::OnNewFile(wxCommandEvent& event)
{
  /*if(this->textCtrl->IsModified())
  {
    wxMessageDialog saveDialog(this, "Do you want to save your changes before opening a new file?",
                              "Unsaved changes", wxYES_NO | wxCANCEL | wxICON_QUESTION);
    int result = saveDialog.ShowModal();
    if(result == wxID_CANCEL) 
    {
      return;
    }
    else if(result == wxID_YES) {
      OnSaveFile(event);
    }
  }
  SetStatusText("Successfully created new file", 0);
  this->textCtrl->SetValue("");*/
  CreateTab("Unnamed.md",true);
  RenderMarkdown(CURRENT_TAB);
}

void MarkdownWindow::OnOpenFile(wxCommandEvent& event)
{
  wxString defaultDirectory = wxGetHomeDir();
  wxFileDialog openDialog(this, wxString("Open Markdown File"), 
                          defaultDirectory, "",
                          "Markdown files (*.md)|*.md|Text files (*.txt)|*.txt|All files (*.*)|*.*",
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if(openDialog.ShowModal() == wxID_OK)
  {
    wxString filePath = openDialog.GetPath();
    CreateTab(openDialog.GetFilename(), true);
    wxStyledTextCtrl* newTab = this->styledWindows.back();
    if(newTab->LoadFile(openDialog.GetPath())) {
      this->currentlyOpenedFiles[CURRENT_TAB] = openDialog.GetPath();
      newTab->SetModified(false);
      this->recentFiles.AddFileToHistory(openDialog.GetPath());
      this->recentFiles.Save(*wxConfig::Get());
      RenderMarkdown(CURRENT_TAB);
      SetStatusText("Opened file: " + openDialog.GetFilename(), 0);
      if(this->recentFiles.GetCount() > 0)
        this->recentFilesMenuItem->Enable(true);
    }
    else 
    {
      SetStatusText("Failed to open file " + filePath + '.', 0);
      wxLogError("Cannot open file '%s'.", filePath);
    }
  }
}

void MarkdownWindow::OnOpenRecentFile(wxCommandEvent& event)
{
  int index = event.GetId() - wxID_FILE1;
  wxString path = this->recentFiles.GetHistoryFile(index);
  wxFileName fileName(path);
  CreateTab(fileName.GetFullName(), true);
  wxStyledTextCtrl* newTab = this->styledWindows.back();
  if(newTab->LoadFile(path)) {
    this->currentlyOpenedFiles[CURRENT_TAB] = path;
    newTab->SetModified(false);
    this->recentFiles.AddFileToHistory(path);
    this->recentFiles.Save(*wxConfig::Get());
    RenderMarkdown(CURRENT_TAB);
    SetStatusText("Opened file: " + path, 0);
  }
  else 
  {
    SetStatusText("Failed to open file " + path + '.', 0);
    wxLogError("Cannot open file '%s'.", path);
  }
}

void MarkdownWindow::OnSaveFile(wxCommandEvent& event)
{
  int target = (this->targetTab != wxNOT_FOUND) ? this->targetTab : this->notebook->GetSelection();
  if(this->currentlyOpenedFiles[target] != "")
  {
    std::ofstream outputFile(this->currentlyOpenedFiles[target], std::ios::out | std::ios::trunc);
    if(outputFile.is_open())
    {
      outputFile << this->styledWindows[target]->GetValue();
      outputFile.close();
      this->styledWindows[CURRENT_TAB]->SetModified(false);
    }
    else 
    {
      SetStatusText("Failed to save file " + this->currentlyOpenedFiles[target] + '.', 0);
      wxLogError("Cannot save current contents to file '%s'.", this->currentlyOpenedFiles[target]);
    }
  }
  else
  {
    OnSaveAsFile(event);
  }
}

void MarkdownWindow::OnSaveAsFile(wxCommandEvent& event)
{
  wxString defaultDirectory = wxGetHomeDir();
  wxString defaultFile = "new markdown document.md";
  wxFileDialog saveDialog(this, wxString("Save Markdown File"), 
                          defaultDirectory, defaultFile,
                          "Markdown files (*.md)|*.md|Text files (*.txt)|*.txt|All files (*.*)|*.*",
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if(saveDialog.ShowModal() == wxID_CANCEL)
    return;
  wxString filePath = saveDialog.GetPath();
  std::ofstream outputMarkdownFile(filePath.ToStdString(), std::ios::out | std::ios::trunc);
  if(outputMarkdownFile.is_open())
  {
    int target = (this->targetTab != wxNOT_FOUND) ? this->targetTab : this->notebook->GetSelection();
    outputMarkdownFile << this->styledWindows[target]->GetValue();
    outputMarkdownFile.close();
    SetStatusText("Successfully saved file " + filePath + '.', 0);
    this->currentlyOpenedFiles[target] = saveDialog.GetPath();
    this->styledWindows[target]->SetModified(false);
    this->notebook->SetPageText(target, saveDialog.GetFilename());
  }
  else 
  {
    SetStatusText("Failed to save file " + filePath + '.', 0);
    wxLogError("Cannot save current contents to file '%s'.", filePath);
  }
}

void MarkdownWindow::OnQuit(wxCommandEvent& event)
{
  Close();
}

void MarkdownWindow::OnCloseWindow(wxCommandEvent& event)
{
  int toDestroy = (this->targetTab != wxNOT_FOUND) ? this->targetTab : this->notebook->GetSelection();;
  if(toDestroy == wxNOT_FOUND) return;
  if(this->styledWindows[toDestroy]->IsModified())
  {
      wxString promptMessage = wxString::Format("Do you want to save your changes to '%s' before closing?", 
                                                this->notebook->GetPageText(toDestroy));
      wxMessageDialog saveDialog(this, promptMessage, "Unsaved changes", wxYES_NO | wxCANCEL | wxICON_QUESTION);
      int result = saveDialog.ShowModal();
      if(result == wxID_CANCEL) 
      {
        return;
      }
      else if(result == wxID_YES) {
        wxCommandEvent dummyEvent(wxEVT_MENU);
        OnSaveFile(dummyEvent);
      }
  }

  //Delete pointers
  this->styledWindows.erase(this->styledWindows.begin() + toDestroy);
  this->htmlWindows.erase(this->htmlWindows.begin() + toDestroy);
  this->splitters.erase(this->splitters.begin() + toDestroy);
  this->currentlyOpenedFiles.erase(this->currentlyOpenedFiles.begin() + toDestroy);
  this->notebook->DeletePage(toDestroy);
}

void MarkdownWindow::OnCloseAllWindows(wxCommandEvent& event)
{
  for(int i = 0; i < this->styledWindows.size(); i++)
  {
    wxStyledTextCtrl* currentTab = this->styledWindows[i];
    if(currentTab->IsModified())
    {
      this->notebook->SetSelection(i);
      wxString promptMessage = wxString::Format("Do you want to save your changes to '%s' before closing?", 
                                                this->notebook->GetPageText(i));
      wxMessageDialog saveDialog(this, promptMessage, "Unsaved changes", wxYES_NO | wxCANCEL | wxICON_QUESTION);
      int result = saveDialog.ShowModal();
      if(result == wxID_CANCEL) 
      {
        return;
      }
      else if(result == wxID_YES) {
        //Yes, this is a terrible solution!
        //The saving functionality should be independent from the event handling logic!
        wxCommandEvent dummyEvent(wxEVT_MENU);
        OnSaveFile(dummyEvent);
      }
    }
  }

  this->styledWindows.clear();
  this->htmlWindows.clear();
  this->splitters.clear();
  this->currentlyOpenedFiles.clear();
  this->notebook->DeleteAllPages();
}
