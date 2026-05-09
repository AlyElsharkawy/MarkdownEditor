#pragma once
#include <array>
#include <wx/window.h>

class HtmlBackend
{
protected:
  float zoomLevel = 1.0f;

public:
  virtual ~HtmlBackend() = default;
  HtmlBackend() = default;

  virtual void SetHtmlText(const wxString &html) = 0;
  virtual void SetZoom(float zoomLevel) = 0;
  virtual void SetFontSizes(const std::array<int, 7> sizes) = 0;
  virtual wxWindow *GetControl() = 0;
  virtual void SetCSSStyle(const wxString& style) = 0;
  virtual const wxString GetHTMLContents() const = 0;

  float GetZoom() const { return this->zoomLevel; }
  virtual const std::array<int, 7>& GetFontSizes() const = 0;
};
