#include <wx/event.h>
#include <wx/html/htmlwin.h>
#include <wx/statusbr.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include <wx/config.h>
#include <wx/menuitem.h>
#include <wx/settings.h>
#include "MarkdownWindow.h"
#include "cmark.h"
#include "MarkdownExample.h"

#define wxID_MINIMIZE_FRAME 11000

MarkdownWindow::MarkdownWindow(const wxString& title, const wxPoint& position, const wxSize& size) :
wxFrame(nullptr, wxID_ANY, title, position, size)
{
  editorFont = wxFont(wxFontInfo(std::round(fontSize * zoomLevel))
                          .Family(wxFONTFAMILY_DEFAULT)
                          .Weight(wxFontWeight::wxFONTWEIGHT_SEMIBOLD));
  this->splitter = new wxSplitterWindow(this, wxID_ANY);
  this->htmlWindow = new wxHtmlWindow(splitter, wxID_ANY);
  this->htmlWindow->Bind(wxEVT_HTML_LINK_CLICKED, &MarkdownWindow::OnHTMLLinkClicked, this);
  this->htmlFont = this->htmlWindow->GetFont();

  this->textCtrl = new wxStyledTextCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  this->textCtrl->SetWrapMode(wxSTC_WRAP_WORD);
  this->textCtrl->StyleSetFont(wxSTC_STYLE_DEFAULT, editorFont);

  //Manual dark-mode support
  wxColour bgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
  wxColour fgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
  this->textCtrl->StyleSetBackground(wxSTC_STYLE_DEFAULT, bgColor);
  this->textCtrl->StyleSetForeground(wxSTC_STYLE_DEFAULT, fgColor);
  this->textCtrl->SetCaretForeground(fgColor);
  this->textCtrl->StyleClearAll();
  this->textCtrl->SetValue(MarkdownExample);
  //Unbind default shortcuts
  this->textCtrl->CmdKeyClearAll();
  this->textCtrl->EmptyUndoBuffer(); //To prevent deletion of initial markdown

  splitter->SetSashGravity(0.5);
  splitter->SetMinimumPaneSize(30);
  splitter->SplitVertically(this->textCtrl, this->htmlWindow, 0);
  //TO comment out
  this->textCtrl->Bind(wxEVT_STC_CHANGE, 
            [this](wxCommandEvent& event) {
                RenderMarkdown();
            });
  InitializeMenuBar();

  CreateStatusBar(3);
  int tempWidths[] = {-1, 250, 80};
  SetStatusWidths(3, tempWidths);
  SetStatusText("Successfully Loaded!", 0);
  //SetStatusText("421 Words, 3,222 Characters", 1);
  CalculateTypingStatistics(this->textCtrl->GetValue());
  SetStatusText("100%", 2);

  this->typingStatisticsTimer.SetOwner(this, TIMER_IDS::TYPING_STATISTICS_TIMER);
  Bind(wxEVT_TIMER, &MarkdownWindow::OnTypingStatisticsTimer, this, TIMER_IDS::TYPING_STATISTICS_TIMER);
  this->typingStatisticsTimer.Start(250);

  this->htmlWindow->SetFonts("", "", this->htmlFontSizes.data());
  this->htmlWindow->Refresh();
  RenderMarkdown();

  this->findData.SetFlags(wxFR_DOWN);
  this->Layout();
}

void MarkdownWindow::RenderMarkdown()
{
    auto text = this->textCtrl->GetValue();
    auto buffer = text.utf8_str();
    auto htmlText = cmark_markdown_to_html(buffer.data(), buffer.length(), CMARK_OPT_DEFAULT);
    this->htmlWindow->SetPage(wxString::FromUTF8(htmlText));
    free(htmlText);
}

void MarkdownWindow::InitializeMenuBar()
{
  this->recentFilesSubmenu = new wxMenu();
  this->recentFiles.UseMenu(this->recentFilesSubmenu);
  this->recentFiles.Load(*wxConfig::Get());
  wxMenuBar* menuBar = new wxMenuBar();

  wxMenu* fileMenu = new wxMenu();
  Bind(wxEVT_MENU, &MarkdownWindow::OnOpenRecentFile, this, wxID_FILE1, wxID_FILE9);

  fileMenu->Append(wxID_NEW, "&New file\tCtrl+N", "Create a new .md file");
  fileMenu->Append(wxID_OPEN, "&Open File\tCtrl+O", "Open a markdown (.md) for editing");
  this->recentFilesMenuItem = fileMenu->AppendSubMenu(this->recentFilesSubmenu, "Open Recent");
  if(this->recentFiles.GetCount() == 0)
    recentFilesMenuItem->Enable(false);
  fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S", "Save the currently opened document");
  fileMenu->Append(wxID_SAVEAS, "Save As\tCtrl+Shift+S", "Save as the currently opened document");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_CLOSE, "Close &window\tCtrl+W", "Close currently opened window");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, "&Quit\tCtrl+Q", "Quit the program");
  Bind(wxEVT_MENU, &MarkdownWindow::OnNewFile, this, wxID_NEW);
  Bind(wxEVT_MENU, &MarkdownWindow::OnOpenFile, this, wxID_OPEN);
  Bind(wxEVT_MENU, &MarkdownWindow::OnSaveFile, this, wxID_SAVE);
  Bind(wxEVT_MENU, &MarkdownWindow::OnSaveAsFile, this, wxID_SAVEAS);
  Bind(wxEVT_MENU, &MarkdownWindow::OnQuit, this, wxID_EXIT);
  Bind(wxEVT_MENU, &MarkdownWindow::OnCloseWindow, this, wxID_CLOSE);
  Bind(wxEVT_CLOSE_WINDOW, &MarkdownWindow::OnQuitApplication, this);

  wxMenu* editMenu = new wxMenu();
  editMenu->Append(wxID_UNDO, "&Undo\tCtrl+Z", "Undo the last performed action");
  editMenu->Append(wxID_REDO, "&Redo\tCtrl+Y", "Redo the last performed action");
  editMenu->AppendSeparator();
  editMenu->Append(wxID_FIND, "&Find\tCtrl+F", "Search for text in the markdown document");
  editMenu->Append(wxID_REPLACE, "Find and &Replace\tCtrl+Shift+F", "Search for text in the markdown document and replace it");
  editMenu->AppendSeparator();
  editMenu->Append(wxID_PREFERENCES, "&Preferences\tCtrl+P", "Edit user settings");
  Bind(wxEVT_MENU, &MarkdownWindow::OnUndo, this, wxID_UNDO);
  Bind(wxEVT_MENU, &MarkdownWindow::OnRedo, this, wxID_REDO);
  Bind(wxEVT_MENU, &MarkdownWindow::OnPreferences, this, wxID_PREFERENCES);
  Bind(wxEVT_MENU, &MarkdownWindow::OnFind, this, wxID_FIND);
  Bind(wxEVT_MENU, &MarkdownWindow::OnFindAndReplace, this, wxID_REPLACE);

  //Route all find and find & replace events to a central handling function
  Bind(wxEVT_FIND, &MarkdownWindow::OnFindReplaceDialogEvent, this);
  Bind(wxEVT_FIND_NEXT, &MarkdownWindow::OnFindReplaceDialogEvent, this);
  Bind(wxEVT_FIND_REPLACE, &MarkdownWindow::OnFindReplaceDialogEvent, this);
  Bind(wxEVT_FIND_REPLACE_ALL, &MarkdownWindow::OnFindReplaceDialogEvent, this);
  Bind(wxEVT_FIND_CLOSE, &MarkdownWindow::OnFindReplaceDialogEvent, this);

  Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
    event.Enable(this->textCtrl->CanUndo());
  }, wxID_UNDO);
  Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
    event.Enable(this->textCtrl->CanRedo());
  }, wxID_REDO);

  wxMenu* viewMenu = new wxMenu();
  viewMenu->Append(wxID_ZOOM_IN, "Zoom &In\tCtrl+=", "Zoom in");
  viewMenu->Append(wxID_ZOOM_OUT, "Zoom &Out\tCtrl+-", "Zoom out");
  viewMenu->Append(wxID_ZOOM_FIT, "Zoom to &Fit\tCtrl+0", "Zoom to fit");
  viewMenu->AppendSeparator();
  viewMenu->Append(wxID_FORWARD, "&Next Window\tCtrl+E");
  viewMenu->Append(wxID_BACKWARD, "&Previous Window\tCtrl+Q");
  viewMenu->AppendSeparator();
  viewMenu->Append(wxID_MAXIMIZE_FRAME, "Maximize Markdown Window\tCtrl+]", "Maximize Markdown windoww");
  viewMenu->Append(wxID_MINIMIZE_FRAME, "Minimize Markdown Window\tCtrl+[", "Minimize Markdown window");
  viewMenu->Append(wxID_RESTORE_FRAME, "Restore Markdown Window size\tCtrl+'", "Restore Markdown window size");
  Bind(wxEVT_MENU, &MarkdownWindow::OnZoomIn, this, wxID_ZOOM_IN);
  Bind(wxEVT_MENU, &MarkdownWindow::OnZoomOut, this, wxID_ZOOM_OUT);
  Bind(wxEVT_MENU, &MarkdownWindow::OnZoomFit, this, wxID_ZOOM_FIT);
  Bind(wxEVT_MENU, &MarkdownWindow::OnGoToNextWindow, this, wxID_FORWARD);
  Bind(wxEVT_MENU, &MarkdownWindow::OnGoToPreviousWindow, this, wxID_BACKWARD);
  Bind(wxEVT_MENU, &MarkdownWindow::OnMaximizeSashMarkdown, this, wxID_MAXIMIZE_FRAME);
  Bind(wxEVT_MENU, &MarkdownWindow::OnMinimizeSashMarkdown, this, wxID_MINIMIZE_FRAME);
  Bind(wxEVT_MENU, &MarkdownWindow::OnRestoreSashMarkdown, this, wxID_RESTORE_FRAME);

  wxMenu* helpMenu = new wxMenu();
  helpMenu->Append(wxID_HELP_CONTEXT, "&Markdown Guide\tCtrl+G", "View online guide about the markdown specification");
  helpMenu->Append(wxID_HELP_COMMANDS, "&Shortcuts Guide\tCtrl+H", "View list of keyboard shortcuts for this program");
  helpMenu->Append(wxID_ABOUT, "View &Credits\tCtrl+L", "View credits and attribution");
  Bind(wxEVT_MENU, &MarkdownWindow::OnHelpMarkdown, this, wxID_HELP_CONTEXT);
  Bind(wxEVT_MENU, &MarkdownWindow::OnHelpShortcuts, this, wxID_HELP_COMMANDS);
  Bind(wxEVT_MENU, &MarkdownWindow::OnViewCredits, this, wxID_ABOUT);

  menuBar->Append(fileMenu, "File");
  menuBar->Append(editMenu, "Edit");
  menuBar->Append(viewMenu, "View");
  menuBar->Append(helpMenu, "Help");
  SetMenuBar(menuBar);
}

void MarkdownWindow::OnQuitApplication(wxCloseEvent& event)
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
      //Yes, this is a terrible solution!
      //The saving functionality should be independent from the event handling logic!
      wxCommandEvent dummyEvent(wxEVT_MENU);
      OnSaveFile(dummyEvent);
    }
  }
  event.Skip();
}
