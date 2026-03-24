#pragma once
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/position.h>
#include <wx/textctrl.h>
#include <wx/htmllbox.h>
#include <wx/timer.h>

enum TIMER_IDS {
  TYPING_STATISTICS_TIMER = 10000,
  MARKDOWN_REFRESH_TIMER,
};

class MarkdownWindow : public wxFrame
{
public:
  MarkdownWindow(const wxString& title, const wxPoint& position, const wxSize& size);

private:
  void RenderMarkdown();
  void InitializeMenuBar(); 
  
  //File menu event handlers
  void OnNewFile(wxCommandEvent& event);
  void OnOpenFile(wxCommandEvent& event);
  void OnSaveFile(wxCommandEvent& event);
  void OnSaveAsFile(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);

  //Edit menu event handlers
  void OnUndo(wxCommandEvent& event);
  void OnRedo(wxCommandEvent& event);
  void OnPreferences(wxCommandEvent& event);

  //View menu event handlers
  void OnZoomIn(wxCommandEvent& event);
  void OnZoomOut(wxCommandEvent& event);
  void OnZoomFit(wxCommandEvent& event);

  //Help menu event handlers
  void OnHelpMarkdown(wxCommandEvent& event);
  void OnHelpShortcuts(wxCommandEvent& event);
  void OnViewCredits(wxCommandEvent& event);

  void CalculateTypingStatistics(const wxString& input);

  void OnTypingStatisticsTimer(wxTimerEvent& event);
  void OnMarkdownRefreshTimer(wxTimerEvent& event);

  wxHtmlWindow* htmlWindow;
  wxTextCtrl* textCtrl;
  wxTimer typingStatisticsTimer;
  float zoomLevel = 1.0f;
  int fontSize = 16;
  wxFont editorFont = wxFont(wxFontInfo(std::round(fontSize * zoomLevel))
                          .Family(wxFONTFAMILY_TELETYPE)
                          .Weight(wxFontWeight::wxFONTWEIGHT_SEMIBOLD));

  const wxString MarkdownExample = R"(
# Hello World
This is a simple wxWidgets application that displays a "Hello World!" message in a text control.

## Features
- wxFrame
- wxTextCtrl

# Learn More

Visit [wxWidgets Documentation](https://docs.wxwidgets.org/3.2) to learn more about wxWidgets.

# Created by Aly Elsharkawy 23/3/2026
    )";

};
