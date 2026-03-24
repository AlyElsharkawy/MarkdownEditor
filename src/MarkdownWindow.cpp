#include <wx/event.h>
#include <wx/splitter.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
#include "MarkdownWindow.h"
#include "cmark.h"

MarkdownWindow::MarkdownWindow(const wxString& title, const wxPoint& position, const wxSize& size) :
wxFrame(nullptr, wxID_ANY, title, position, size)
{
  wxSplitterWindow* splitter = new wxSplitterWindow(this, wxID_ANY);
  this->htmlWindow = new wxHtmlWindow(splitter, wxID_ANY);

  this->textCtrl = new wxTextCtrl(splitter, wxID_ANY, MarkdownExample, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
  this->textCtrl->SetFont(editorFont);
  this->textCtrl->SetValue(this->MarkdownExample);

  splitter->SetSashGravity(0.5);
  splitter->SplitVertically(this->textCtrl, this->htmlWindow);

  this->textCtrl->Bind(wxEVT_TEXT, 
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

  RenderMarkdown();
}

void MarkdownWindow::RenderMarkdown()
{
    auto text = this->textCtrl->GetValue();
    auto buffer = text.utf8_str();

    auto htmlText = cmark_markdown_to_html(buffer.data(), buffer.length(), CMARK_OPT_DEFAULT);

    this->htmlWindow->SetPage(htmlText);

    free(htmlText);

}

void MarkdownWindow::InitializeMenuBar()
{
  wxMenuBar* menuBar = new wxMenuBar();

  wxMenu* fileMenu = new wxMenu();
  fileMenu->Append(wxID_NEW, "&New file\tCtrl+N", "Create a new .md file");
  fileMenu->Append(wxID_OPEN, "&Open File\tCtrl+O", "Open a markdown (.md) or text (.txt) file for editing");
  fileMenu->Append(wxID_SAVE, "&Save\tCtrl+S", "Save the currently opened document");
  fileMenu->Append(wxID_SAVEAS, "Save As\tCtrl+Shift+S", "Save as the currently opened document");
  fileMenu->Append(wxID_EXIT, "&Quit\tCtrl+Q", "Quit the program");
  Bind(wxEVT_MENU, &MarkdownWindow::OnNewFile, this, wxID_NEW);
  Bind(wxEVT_MENU, &MarkdownWindow::OnOpenFile, this, wxID_OPEN);
  Bind(wxEVT_MENU, &MarkdownWindow::OnSaveFile, this, wxID_SAVE);
  Bind(wxEVT_MENU, &MarkdownWindow::OnSaveAsFile, this, wxID_SAVEAS);
  Bind(wxEVT_MENU, &MarkdownWindow::OnQuit, this, wxID_EXIT);

  wxMenu* editMenu = new wxMenu();
  editMenu->Append(wxID_UNDO, "&Undo\tCtrl+Z", "Undo the last performed action");
  editMenu->Append(wxID_REDO, "&Redo\tCtrl+Y", "Redo the last performed action");
  editMenu->Append(wxID_FIND, "&Find\tCtrl+F", "Search for text in the markdown document");
  editMenu->Append(wxID_REPLACE, "Find and &Replace\tCtrl+Shift+F", "Search for text in the markdown document and replace it");
  editMenu->Append(wxID_PREFERENCES, "&Preferences\tCtrl+P", "Edit user settings");
  Bind(wxEVT_MENU, &MarkdownWindow::OnUndo, this, wxID_UNDO);
  Bind(wxEVT_MENU, &MarkdownWindow::OnRedo, this, wxID_REDO);
  Bind(wxEVT_MENU, &MarkdownWindow::OnPreferences, this, wxID_PREFERENCES);

  wxMenu* viewMenu = new wxMenu();
  viewMenu->Append(wxID_ZOOM_IN, "Zoom &In\tCtrl+=", "Zoom in");
  viewMenu->Append(wxID_ZOOM_OUT, "Zoom &Out\tCtrl+-", "Zoom out");
  viewMenu->Append(wxID_ZOOM_FIT, "Zoom to &Fit\tCtrl+0", "Zoom to fit");
  Bind(wxEVT_MENU, &MarkdownWindow::OnZoomIn, this, wxID_ZOOM_IN);
  Bind(wxEVT_MENU, &MarkdownWindow::OnZoomOut, this, wxID_ZOOM_OUT);
  Bind(wxEVT_MENU, &MarkdownWindow::OnZoomFit, this, wxID_ZOOM_FIT);

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
