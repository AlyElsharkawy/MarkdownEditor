#pragma once
#include <wx/html/htmlwin.h>
#include <array>
#include "HtmlBackend.h"

class SimpleHtmlBackend : public HtmlBackend
{
private:
  wxHtmlWindow* window;
  constexpr static std::array<int, 7> fontSizes = {8, 9, 12, 14, 16, 18, 24};

public:
  SimpleHtmlBackend(wxWindow* parent);
  ~SimpleHtmlBackend() override;
  void SetHtmlText(const wxString &html) override;
  void SetZoom(float zoomLevel) override;
  void SetFontSizes(const std::array<int, 7> sizes) override;
  void SetCSSStyle(const wxString& style) override;
  const std::array<int, 7>& GetFontSizes() const override;
  const wxString GetHTMLContents() const override;
  wxWindow *GetControl() override;
};
