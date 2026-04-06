#include <wx/event.h>
#include "MarkdownWindow.h"

#define UPDATE_STATUS_BAR \
wxString message = std::to_string(static_cast<int>(this->zoomLevel * 100)) + '%'; \
SetStatusText(message,2); \
this->editorFont.SetPointSize(this->zoomLevel * this->fontSize); \
this->styledWindows[CURRENT_TAB]->StyleSetFont(wxSTC_STYLE_DEFAULT, editorFont); \
this->styledWindows[CURRENT_TAB]->StyleClearAll(); \
int zoomedSizes[7]; \
for (int i = 0; i < 7; ++i) { \
  zoomedSizes[i] = static_cast<int>(this->htmlFontSizes[i] * this->zoomLevel); \
} \
for(int i = 0; i < this->htmlWindows.size(); i++) \
   {this->htmlWindows[i]->SetFonts("", "", zoomedSizes); } \
this->htmlWindows[CURRENT_TAB]->Refresh();

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
  this->htmlWindows[CURRENT_TAB]->SetFonts("", "", this->htmlFontSizes.data());
  UPDATE_STATUS_BAR;
}

void MarkdownWindow::OnMaximizeSashMarkdown(wxCommandEvent& event)
{
  this->splitters[CURRENT_TAB]->SetSashPosition(this->splitters[CURRENT_TAB]->GetClientSize().x -
                                  this->splitters[CURRENT_TAB]->GetMinimumPaneSize());
}

void MarkdownWindow::OnMinimizeSashMarkdown(wxCommandEvent& event)
{
  this->splitters[CURRENT_TAB]->SetSashPosition(this->splitters[CURRENT_TAB]->GetMinimumPaneSize());
}

void MarkdownWindow::OnRestoreSashMarkdown(wxCommandEvent& event)
{
  this->splitters[CURRENT_TAB]->SetSashPosition(this->splitters[CURRENT_TAB]->GetClientSize().x / 2);
}

void MarkdownWindow::OnGoToNextWindow(wxCommandEvent& event)
{
  if(this->styledWindows.size() == 1) return;
  int index = this->notebook->GetSelection();
  index++;
  if(index >= this->styledWindows.size())
    index = 0;
  this->notebook->SetSelection(index);
  this->styledWindows[index]->SetFocus();
  this->styledWindows[index]->GotoPos(0);
  SetStatusText("Switched to: " + this->currentlyOpenedFiles[index], 0);
}

void MarkdownWindow::OnGoToPreviousWindow(wxCommandEvent& event)
{
  if(this->styledWindows.size() == 1) return;
  int index = this->notebook->GetSelection();
  index--;
  if(index < 0)
    index = this->styledWindows.size() - 1;
  this->notebook->SetSelection(index);
  this->styledWindows[index]->SetFocus();
  this->styledWindows[index]->GotoPos(0);
  SetStatusText("Switched to: " + this->currentlyOpenedFiles[index], 0);
}
