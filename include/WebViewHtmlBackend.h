#pragma once
#include <wx/webview.h>
#include "HtmlBackend.h"

class WebViewHtmlBackend : public HtmlBackend
{
private:
  wxWebView* window;
  wxString contents;
public:
  WebViewHtmlBackend(wxWindow* parent);
  
  void SetHtmlText(const wxString &html) override;
  void SetZoom(float zoomLevel) override;
  wxWindow *GetControl() override;
  void SetCSSStyle(const wxString& style) override;
  void SetFontSizes(const std::array<int, 7> sizes) override;
  const std::array<int, 7>& GetFontSizes() const override; 
  const wxString GetHTMLContents() const override;
};
