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
  wxString configDir = wxGetHomeDir() + "/." + wxTheApp->GetAppName();
  if (!wxDirExists(configDir)) {
    wxMkdir(configDir);
  }

  // Create the config file INSIDE the directory
  // Parameters: appName, vendorName, localFilename, globalFilename, style
  wxConfig::Set(new wxFileConfig(
    wxTheApp->GetAppName(), 
    "", 
    configDir + "/config.ini", // Specific file path
    "", 
    wxCONFIG_USE_LOCAL_FILE
  ));
  std::cout << "App Name: " << wxTheApp->GetAppName() << '\n';

  MarkdownWindow* window = new MarkdownWindow("Simple Markdown Editor", wxDefaultPosition, wxDefaultSize);
  window->Show(true);
  window->SetSize(800, 600);
  return true;
}

wxIMPLEMENT_APP(MarkdownEditor);
