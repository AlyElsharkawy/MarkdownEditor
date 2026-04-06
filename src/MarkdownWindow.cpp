#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/html/htmlwin.h>
#include <wx/statusbr.h>
#include <wx/stc/stc.h>
#include <wx/stdpaths.h>
#include <wx/msgdlg.h>
#include <wx/config.h>
#include <wx/menuitem.h>
#include <wx/settings.h>
#include "MarkdownWindow.h"
#include "cmark.h"
#include "MarkdownExample.h"

MarkdownWindow::MarkdownWindow(const wxString& title, const wxPoint& position, const wxSize& size) :
wxFrame(nullptr, wxID_ANY, title, position, size)
{
  editorFont = wxFont(wxFontInfo(std::round(fontSize * zoomLevel))
                          .Family(wxFONTFAMILY_DEFAULT)
                          .Weight(wxFontWeight::wxFONTWEIGHT_SEMIBOLD));
  this->notebook = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);
  this->notebook->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &MarkdownWindow::OnTabChange, this);
  this->notebook->Bind(wxEVT_RIGHT_UP, &MarkdownWindow::OnNotebookTabRightClick, this);
  this->notebook->Bind(wxEVT_LEFT_DCLICK, &MarkdownWindow::OnNotebookTabDoubleClick, this);
  //Manual dark-mode support
  bgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
  fgColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
  InitializeMenuBar();
  InitializeNotebookTabContextMenu();

  CreateStatusBar(3);
  int tempWidths[] = {-1, 250, 80};
  SetStatusWidths(3, tempWidths);
  SetStatusText("Successfully Loaded!", 0);
  SetStatusText("100%", 2);

  CreateTab("Hello.md",true);
  this->findData.SetFlags(wxFR_DOWN);
  this->Layout();
}

MarkdownWindow::~MarkdownWindow()
{
  delete this->notebookTabContextMenu;
}

void MarkdownWindow::CreateTab(const wxString& name, bool isFocused)
{
  wxSplitterWindow* splitter = new wxSplitterWindow(this->notebook, wxID_ANY);
  wxHtmlWindow* htmlWindow = new wxHtmlWindow(splitter, wxID_ANY);
  htmlWindow->Bind(wxEVT_HTML_LINK_CLICKED, &MarkdownWindow::OnHTMLLinkClicked, this);
  //this->htmlFont = htmlWindow->GetFont();

  wxStyledTextCtrl* textCtrl = new wxStyledTextCtrl(splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  textCtrl->SetWrapMode(wxSTC_WRAP_WORD);
  textCtrl->StyleSetFont(wxSTC_STYLE_DEFAULT, editorFont);

  textCtrl->StyleSetBackground(wxSTC_STYLE_DEFAULT, this->bgColor);
  textCtrl->StyleSetForeground(wxSTC_STYLE_DEFAULT, this->fgColor);
  textCtrl->SetCaretForeground(fgColor);
  textCtrl->StyleClearAll();
  textCtrl->SetValue(MarkdownExample);
  //Unbind default shortcuts
  //textCtrl->CmdKeyClearAll();
  textCtrl->EmptyUndoBuffer(); //To prevent deletion of initial markdown

  splitter->SetSashGravity(0.5);
  splitter->SetMinimumPaneSize(30);
  splitter->SplitVertically(textCtrl, htmlWindow, 0);
  
  //TO comment out
  textCtrl->Bind(wxEVT_STC_CHANGE, 
            [this](wxCommandEvent& event) {
                RenderMarkdown(CURRENT_TAB);
            });
  
  this->typingStatisticsTimer.SetOwner(this, TIMER_IDS::TYPING_STATISTICS_TIMER);
  Bind(wxEVT_TIMER, &MarkdownWindow::OnTypingStatisticsTimer, this, TIMER_IDS::TYPING_STATISTICS_TIMER);
  this->typingStatisticsTimer.Start(250);

  htmlWindow->SetFonts("", "", this->htmlFontSizes.data());
  htmlWindow->Refresh();
  CalculateTypingStatistics(textCtrl->GetValue());
  this->notebook->AddPage(splitter, name, isFocused);
  this->htmlWindows.push_back(htmlWindow);
  this->styledWindows.push_back(textCtrl);
  this->splitters.push_back(splitter);
  this->currentlyOpenedFiles.push_back(wxString(""));
  RenderMarkdown(CURRENT_TAB);
}

void MarkdownWindow::RenderMarkdown(int index)
{
    auto text = this->styledWindows[index]->GetValue();
    auto buffer = text.utf8_str();
    auto htmlText = cmark_markdown_to_html(buffer.data(), buffer.length(), CMARK_OPT_DEFAULT);
    this->htmlWindows[CURRENT_TAB]->SetPage(wxString::FromUTF8(htmlText));
    free(htmlText);
}

void MarkdownWindow::OnQuitApplication(wxCloseEvent& event)
{
  for(int i = 0; i < this->styledWindows.size(); i++)
  {
    wxStyledTextCtrl* currentTab = this->styledWindows[i];
    if(currentTab->IsModified())
    {
      this->notebook->SetSelection(i);
      wxString promptMessage = wxString::Format("Do you want to save your changes to '%s' before closing?", 
                                                this->notebook->GetPageText(i));
      wxMessageDialog saveDialog(this, promptMessage, "Unsaved changes", wxYES_NO | wxCANCEL | wxICON_QUESTION);
      int result = saveDialog.ShowModal();
      if(result == wxID_CANCEL) 
      {
        if (event.CanVeto()) 
        {
          event.Veto();
        }
        return;
      }
      else if(result == wxID_YES) {
        //Yes, this is a terrible solution!
        //The saving functionality should be independent from the event handling logic!
        wxCommandEvent dummyEvent(wxEVT_MENU);
        OnSaveFile(dummyEvent);
      }
    }
  }
  event.Skip();
}

void MarkdownWindow::OnTabChange(wxBookCtrlEvent& event)
{
  //Ah well, no longer needed!
  event.Skip();
}
