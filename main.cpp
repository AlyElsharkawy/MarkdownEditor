#include <wx/app.h>
#include <filesystem>
#include "MarkdownWindow.h"

class MarkdownEditor : public wxApp
{
public:
  virtual bool OnInit() override;
};

bool MarkdownEditor::OnInit()
{
  MarkdownWindow* window = new MarkdownWindow("Simple Markdown Editor", wxDefaultPosition, wxDefaultSize);
  window->Show(true);
  window->SetSize(800, 600);
  //App data directory initialization
  if(!std::filesystem::exists(window->recentlyOpenedFilesPath.ToStdString()))
    std::filesystem::create_directory(window->recentlyOpenedFilesPath.ToStdString());

  return true;
}

wxIMPLEMENT_APP(MarkdownEditor);
