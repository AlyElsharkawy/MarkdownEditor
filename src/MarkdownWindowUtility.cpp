#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/regex.h>
#include <wx/textdlg.h>
#include "MarkdownWindow.h"

#define wxID_MINIMIZE_FRAME 11000

void MarkdownWindow::CalculateTypingStatistics(const wxString& input) {
    long totalChars = input.Length();
    wxRegEx reWord("\\w+"); 
    long wordCount = 0;
    size_t start = 0;

    while (start < input.length() && reWord.Matches(input.Mid(start))) {
        wordCount++;
        size_t matchStart, matchLen;
        reWord.GetMatch(&matchStart, &matchLen);
        start += (matchStart + matchLen);
    }

    wxString statusMsg = wxString::Format("%ld Words, %ld Characters", wordCount, totalChars);
    SetStatusText(statusMsg, 1);
}

void MarkdownWindow::OnTypingStatisticsTimer(wxTimerEvent& event)
{
  int current = CURRENT_TAB;
  if(current != wxNOT_FOUND && current >= 0 && current < this->styledWindows.size())
  {
    CalculateTypingStatistics(this->styledWindows[CURRENT_TAB]->GetValue());
  }
  else 
  {
    SetStatusText("0 Words, 0 Characters", 1);
  }
}

void MarkdownWindow::OnHTMLLinkClicked(wxHtmlLinkEvent& event)
{
  const wxHtmlLinkInfo& info = event.GetLinkInfo();
  wxString href = info.GetHref();
  if (href.StartsWith("http://") || href.StartsWith("https://")) {
    wxLaunchDefaultBrowser(href);
  }
  else {
    event.Skip();
  }
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
  fileMenu->Append(wxID_CLOSE_ALL, "Close All Windows\tCtrl+Shift+W", "Close all currently opened windows");
  fileMenu->AppendSeparator();
  fileMenu->Append(wxID_EXIT, "&Quit\tCtrl+Shift+X", "Quit the program");
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
    int current = CURRENT_TAB;
    if(current != wxNOT_FOUND && current >= 0 && current < this->styledWindows.size())
    {
      event.Enable(this->styledWindows[CURRENT_TAB]->CanUndo());
    }
    else 
    {
      event.Enable(false);
    }
  }, wxID_UNDO);
  Bind(wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& event) {
    int current = CURRENT_TAB;
    if(current != wxNOT_FOUND && current >= 0 && current < this->styledWindows.size())
    {
      event.Enable(this->styledWindows[CURRENT_TAB]->CanUndo());
    }
    else 
    {
      event.Enable(false);
    }
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

void MarkdownWindow::OnNotebookTabDoubleClick(wxMouseEvent& event)
{
  long flags = 0;
  int tabIndex = this->notebook->HitTest(event.GetPosition(), &flags);
  if(tabIndex != wxNOT_FOUND)
  {
    wxString oldName = this->notebook->GetPageText(tabIndex);
    wxTextEntryDialog dialog(this, "Enter New Name: ", "Rename tab", oldName);

    if(dialog.ShowModal() == wxID_OK) 
    {
      this->notebook->SetPageText(tabIndex, dialog.GetValue());
    }
  }
  event.Skip();
}

void MarkdownWindow::OnNotebookTabRightClick(wxMouseEvent& event)
{
  long flags = 0;
  int tabIndex = this->notebook->HitTest(event.GetPosition(), &flags);
  if(tabIndex != wxNOT_FOUND)
  {
    this->targetTab = tabIndex;
    this->notebook->PopupMenu(this->notebookTabContextMenu, event.GetPosition());
    this->targetTab = wxNOT_FOUND;
  }
  event.Skip();
}

void MarkdownWindow::OnRenameTabContextMenu(wxCommandEvent& event)
{
  if(this->targetTab == wxNOT_FOUND) return;
  wxString oldName = this->notebook->GetPageText(this->targetTab);
  wxTextEntryDialog dialog(this, "Enter New Name: ", "Rename Tab", oldName);
  if(dialog.ShowModal() == wxID_OK)
  {
    this->notebook->SetPageText(this->targetTab, dialog.GetValue());
  }
}

void MarkdownWindow::InitializeNotebookTabContextMenu()
{
  this->notebookTabContextMenu = new wxMenu();
  this->notebookTabContextMenu->Append(MISC_IDS::RENAME_TAB, "Rename");
  this->notebookTabContextMenu->AppendSeparator();
  this->notebookTabContextMenu->Append(wxID_SAVE, "Save");
  this->notebookTabContextMenu->Append(wxID_SAVEAS, "Save As");
  this->notebookTabContextMenu->AppendSeparator();
  this->notebookTabContextMenu->Append(wxID_CLOSE, "Close");
  this->notebookTabContextMenu->Append(wxID_CLOSE_ALL, "Close All");

  Bind(wxEVT_MENU, &MarkdownWindow::OnSaveFile, this, wxID_SAVE);
  Bind(wxEVT_MENU, &MarkdownWindow::OnSaveAsFile, this, wxID_SAVEAS);
  Bind(wxEVT_MENU, &MarkdownWindow::OnCloseWindow, this, wxID_CLOSE);
  Bind(wxEVT_MENU, &MarkdownWindow::OnCloseAllWindows, this, wxID_CLOSE_ALL);
  Bind(wxEVT_MENU, &MarkdownWindow::OnRenameTabContextMenu, this, MISC_IDS::RENAME_TAB);
}
