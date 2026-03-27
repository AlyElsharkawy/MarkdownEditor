#include <wx/event.h>
#include <wx/filedlg.h>
#include <wx/log.h>
#include <wx/utils.h>
#include <fstream>
#include "MarkdownWindow.h"

void MarkdownWindow::OnNewFile(wxCommandEvent& event)
{
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
        RenderMarkdown();
        SetStatusText("Opened file: " + openDialog.GetFilename(), 0);
    }
    else 
    {
      SetStatusText("Failed to open file " + filePath + '.', 0);
      wxLogError("Cannot open file '%s'.", filePath);
    }
  }
}

void MarkdownWindow::OnSaveFile(wxCommandEvent& event)
{
  OnSaveAsFile(event);
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
