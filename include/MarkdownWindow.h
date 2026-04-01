#pragma once
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/html/htmlwin.h>
#include <wx/position.h>
#include <wx/stc/stc.h>
#include <wx/htmllbox.h>
#include <wx/splitter.h>
#include <wx/timer.h>
#include <wx/stdpaths.h>
#include <wx/filehistory.h>
#include <wx/menu.h>
#include <wx/menuitem.h>
#include <array>

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
  void OnQuitApplication(wxCloseEvent& event);
  
  //File menu event handlers
  void OnNewFile(wxCommandEvent& event);
  void OnOpenFile(wxCommandEvent& event);
  void OnOpenRecentFile(wxCommandEvent& event);
  void OnSaveFile(wxCommandEvent& event);
  void OnSaveAsFile(wxCommandEvent& event);
  void OnQuit(wxCommandEvent& event);
  void OnCloseWindow(wxCommandEvent& event);

  //Edit menu event handlers
  void OnUndo(wxCommandEvent& event);
  void OnRedo(wxCommandEvent& event);
  void OnPreferences(wxCommandEvent& event);
  void OnFind(wxCommandEvent& event);
  void OnFindAndReplace(wxCommandEvent& event);

  //View menu event handlers
  void OnZoomIn(wxCommandEvent& event);
  void OnZoomOut(wxCommandEvent& event);
  void OnZoomFit(wxCommandEvent& event);
  void OnGoToNextWindow(wxCommandEvent& event);
  void OnGoToPreviousWindow(wxCommandEvent& event);
  void OnMaximizeSashMarkdown(wxCommandEvent& event);
  void OnRestoreSashMarkdown(wxCommandEvent& event);
  void OnMinimizeSashMarkdown(wxCommandEvent& event);

  //Help menu event handlers
  void OnHelpMarkdown(wxCommandEvent& event);
  void OnHelpShortcuts(wxCommandEvent& event);
  void OnViewCredits(wxCommandEvent& event);

  void CalculateTypingStatistics(const wxString& input);

  void OnTypingStatisticsTimer(wxTimerEvent& event);
  void OnMarkdownRefreshTimer(wxTimerEvent& event);
  void OnHTMLLinkClicked(wxHtmlLinkEvent& event);

  wxHtmlWindow* htmlWindow;
  wxSplitterWindow* splitter;
  wxStyledTextCtrl* textCtrl;
  wxTimer typingStatisticsTimer;
  float zoomLevel = 1.0f;
  int fontSize = 12;
  wxFont editorFont = wxFont(wxFontInfo(std::round(fontSize * zoomLevel))
                          .Family(wxFONTFAMILY_DEFAULT)
                          .Weight(wxFontWeight::wxFONTWEIGHT_SEMIBOLD));
  wxFont htmlFont;
  //std::array<int,7> htmlFontSizes = { 10, 12, 14, 16, 19, 24, 32 };
  std::array<int, 7> htmlFontSizes = {8, 9, 10, 12, 14, 20, 24};
  wxFileHistory recentFiles;
  wxMenu* recentFilesSubmenu;
  wxMenuItem* recentFilesMenuItem;
  wxString currentlyOpenedFile = "";
};
