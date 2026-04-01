#include <wx/event.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/log.h>
#include <wx/utils.h>
#include <wx/config.h>
#include <fstream>
#include "MarkdownWindow.h"

void MarkdownWindow::OnNewFile(wxCommandEvent& event)
{
  if(this->textCtrl->IsModified())
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
  this->textCtrl->SetValue("");
  RenderMarkdown();
}

void MarkdownWindow::OnOpenFile(wxCommandEvent& event)
{
  wxString defaultDirectory = wxGetHomeDir();
  wxFileDialog openDialog(this, wxString("Open Markdown File"), 
                          defaultDirectory, "",
                          "Markdown files (*.md)|*.md|Text files (*.txt)|*.txt|All files (*.*)|*.*",
                          wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  wxString filePath = openDialog.GetPath();
  if(openDialog.ShowModal() == wxID_OK)
  {
    if(this->textCtrl->LoadFile(openDialog.GetPath())) {
      this->currentlyOpenedFile = openDialog.GetPath();
      this->textCtrl->SetModified(false);
      this->recentFiles.AddFileToHistory(openDialog.GetPath());
      this->recentFiles.Save(*wxConfig::Get());
      RenderMarkdown();
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
  if(this->textCtrl->LoadFile(path)) {
    this->currentlyOpenedFile = path;
    this->textCtrl->SetModified(false);
    this->recentFiles.AddFileToHistory(path);
    this->recentFiles.Save(*wxConfig::Get());
    RenderMarkdown();
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
  if(this->currentlyOpenedFile != "")
  {
    std::ofstream outputFile(this->currentlyOpenedFile, std::ios::out | std::ios::trunc);
    if(outputFile.is_open())
    {
      outputFile << this->textCtrl->GetValue();
      outputFile.close();
      this->textCtrl->SetModified(false);
    }
    else 
    {
      SetStatusText("Failed to save file " + this->currentlyOpenedFile + '.', 0);
      wxLogError("Cannot save current contents to file '%s'.", this->currentlyOpenedFile);
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
    outputMarkdownFile << this->textCtrl->GetValue();
    outputMarkdownFile.close();
    SetStatusText("Successfully saved file " + filePath + '.', 0);
    this->currentlyOpenedFile = saveDialog.GetPath();
    this->textCtrl->SetModified(false);
  }
  else 
  {
    SetStatusText("Failed to save file " + filePath + '.', 0);
    wxLogError("Cannot save current contents to file '%s'.", filePath);
  }
}

void MarkdownWindow::OnQuit(wxCommandEvent& event)
{
  if(this->textCtrl->IsModified())
  {
    wxMessageDialog saveDialog(this, "Do you want to save your changes before closing?",
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
  Close();
}

void MarkdownWindow::OnCloseWindow(wxCommandEvent& event)
{
}
