#include <wx/event.h>
#include "MarkdownWindow.h"
#include "yaml-cpp/mark.h"

#define UPDATE_STATUS_BAR \
wxString message = std::to_string(static_cast<int>(this->zoomLevel * 100)) + '%'; \
SetStatusText(message,2); \
this->editorFont.SetPointSize(this->zoomLevel * this->fontSize); \
this->textCtrl->SetFont(this->editorFont); \
int zoomedSizes[7]; \
for (int i = 0; i < 7; ++i) { \
  zoomedSizes[i] = static_cast<int>(this->htmlFontSizes[i] * this->zoomLevel); \
} \
this->htmlWindow->SetFonts("", "", zoomedSizes); \
this->htmlWindow->Refresh();

void MarkdownWindow::OnZoomIn(wxCommandEvent& event)
{
  this->zoomLevel += 0.25f;
  if(this->zoomLevel > 4.0f)
    zoomLevel = 4.0f;
  UPDATE_STATUS_BAR;
}

void MarkdownWindow::OnZoomOut(wxCommandEvent& event)
{
  this->zoomLevel -= 0.25f;
  if(this->zoomLevel < 0.5f)
    zoomLevel = 0.5f;
  UPDATE_STATUS_BAR;
}

void MarkdownWindow::OnZoomFit(wxCommandEvent& event)
{
  this->zoomLevel = 1.0f;
  this->htmlWindow->SetFonts("", "", this->htmlFontSizes.data());
  UPDATE_STATUS_BAR;
}

void MarkdownWindow::OnMaximizeSashMarkdown(wxCommandEvent& event)
{
  this->splitter->SetSashPosition(this->splitter->GetClientSize().x - this->splitter->GetMinimumPaneSize());
}

void MarkdownWindow::OnMinimizeSashMarkdown(wxCommandEvent& event)
{
  this->splitter->SetSashPosition(this->splitter->GetMinimumPaneSize());
}

void MarkdownWindow::OnRestoreSashMarkdown(wxCommandEvent& event)
{
  this->splitter->SetSashPosition(this->splitter->GetClientSize().x / 2);
}

void MarkdownWindow::OnGoToNextWindow(wxCommandEvent& event)
{
}

void MarkdownWindow::OnGoToPreviousWindow(wxCommandEvent& event)
{
}
