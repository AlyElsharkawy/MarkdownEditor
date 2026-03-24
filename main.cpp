#include <wx/app.h>
#include <wx/gdicmn.h>
#include "MarkdownWindow.h"

class MarkdownEditor : public wxApp
{
public:
  virtual bool OnInit() override;
};

bool MarkdownEditor::OnInit()
{
  MarkdownWindow* window = new MarkdownWindow("Markdown Editor", wxDefaultPosition, wxDefaultSize);
  window->Show(true);
  return true;
}

wxIMPLEMENT_APP(MarkdownEditor);
