#include "MarkdownWindow.h"
#include <string>

#define UPDATE_STATUS_BAR \
wxString message = std::to_string(static_cast<int>(this->zoomLevel * 100)) + '%'; \
SetStatusText(message,2); \
this->editorFont.SetPointSize(this->zoomLevel * this->fontSize); \
this->textCtrl->SetFont(this->editorFont);

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
  UPDATE_STATUS_BAR;
}
