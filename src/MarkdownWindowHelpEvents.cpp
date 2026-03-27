#include <wx/gdicmn.h>
#include <wx/listbase.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include "CheatSheet.h"
#include "MarkdownWindow.h"

void MarkdownWindow::OnHelpMarkdown(wxCommandEvent& event)
{
  wxMessageDialog helpDialog(this, 
                           wxString("Would you like to view a markdown cheatsheet? This will overwrite your currently loaded document!"), 
                           wxString("Load Cheatsheet?"), 
                           wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
  if(helpDialog.ShowModal() == wxID_YES)
  {
    this->textCtrl->SetValue(wxString::FromUTF8(CHEAT_SHEET));
    RenderMarkdown();
  }
}

void MarkdownWindow::OnHelpShortcuts(wxCommandEvent& event)
{
  wxDialog dialog(this, wxID_ANY, "Keyboard Shortcuts", wxDefaultPosition, wxSize(700, 700), wxICON_INFORMATION);
  wxListView* list = new wxListView(&dialog, wxID_ANY, wxDefaultPosition, wxDefaultSize);
  list->InsertColumn(0, "Shortcut", wxLIST_FORMAT_LEFT, 150);
  list->InsertColumn(1, "Action", wxLIST_FORMAT_LEFT, 550);
  list->InsertItem(0, "Ctrl + N");
  list->SetItem(0, 1, "Creates a new empty markdown file");
  list->InsertItem(1, "Ctrl + S");
  list->SetItem(1, 1, "Saves the current file. Doesn't create a save dialog if file already exists");
  list->InsertItem(2, "Ctrl + Shift + S");
  list->SetItem(2, 1, "Saves the current file and always opens a new save dialog");
  list->InsertItem(3, "Ctrl + O");
  list->SetItem(3, 1, "Open a file from disk for editing");
  list->InsertItem(4, "Ctrl + Q");
  list->SetItem(4, 1, "Quit the program");
  list->InsertItem(5, "Ctrl + Z");
  list->SetItem(5, 1, "Undoes the last performed editing action");
  list->InsertItem(6, "Ctrl + Y");
  list->SetItem(6, 1, "Redoes the last performed editing action");
  list->InsertItem(7, "Ctrl + F");
  list->SetItem(7, 1, "Searches the markdown text for user-defined words");
  list->InsertItem(8, "Ctrl + P");
  list->SetItem(8, 1, "Opens the user preferences panel");
  list->InsertItem(9, "Ctrl + =");
  list->SetItem(9, 1, "Zooms in");
  list->InsertItem(10, "Ctrl + -");
  list->SetItem(10, 1, "Zooms out");
  list->InsertItem(11, "Ctrl + 0");
  list->SetItem(11, 1, "Restores zoom to default");
  list->InsertItem(12, "Ctrl + G");
  list->SetItem(12, 1, "View markdown cheat sheet");
  list->InsertItem(13, "Ctrl + H");
  list->SetItem(13, 1, "View shortcuts cheat sheet");
  list->InsertItem(14, "Ctrl + L");
  list->SetItem(14, 1, "View credits");
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(list, 1, wxEXPAND | wxALL, 10);
  wxSizer* buttonSizer = dialog.CreateButtonSizer(wxOK);
  sizer->Add(buttonSizer, 0, wxALIGN_RIGHT | wxBOTTOM | wxRIGHT, 10);
  dialog.SetSizer(sizer);
  dialog.Layout();
  dialog.ShowModal();
}

void MarkdownWindow::OnViewCredits(wxCommandEvent& event)
{
  wxString messageInfo = 
    L"• Created by Aly Elsharkawy 23/3/2025\n"
    L"• Original idea from https://onlyfastcode.com/posts/wxwidgets-cmark-markdown-editor/\n"
    L"• For suggestions, please email me on alyelsharkawyoct@gmail.com\n"
  ;
  wxMessageBox(messageInfo, "Credits", wxOK | wxICON_INFORMATION, this);
}
