#include <wx/regex.h>
#include <iostream>
#include "WebViewHtmlBackend.h"

wxString InjectCustomCSS(const wxString &htmlContent, const wxString &customCSS)
{
  wxString processedHtml = htmlContent;
  wxRegEx reStyle("<style.*?>.*?</style>", wxRE_ICASE | wxRE_ADVANCED);
  while (reStyle.Matches(processedHtml))
  {
    reStyle.Replace(&processedHtml, "");
  }

  wxString styleTag = wxString::Format("\n<style id='injected-css'>\n%s\n</style>\n", customCSS);

  wxRegEx reHead("<head.*?>", wxRE_ICASE | wxRE_ADVANCED);
  if (reHead.Matches(processedHtml))
  {
    size_t start, len;
    reHead.GetMatch(&start, &len, 0);
    processedHtml.insert(start + len, styleTag);
  }
  else
  {
    processedHtml.Prepend(styleTag);
  }
  return processedHtml;
}

void WebViewHtmlBackend::SetCSSStyle(const wxString& style)
{
  wxString newHtml = InjectCustomCSS(this->contents, style);
  newHtml += "</div>";
  this->contents = newHtml;
  this->window->SetPage(newHtml, "");
}

WebViewHtmlBackend::WebViewHtmlBackend(wxWindow *parent)
{
  this->window = wxWebView::New(parent, wxID_ANY);
}

void WebViewHtmlBackend::SetHtmlText(const wxString &text)
{
  this->contents = text;
  this->window->SetPage(this->contents, "");
}

void WebViewHtmlBackend::SetZoom(float zoomLevel)
{
  this->window->SetZoomFactor(zoomLevel);
}

wxWindow* WebViewHtmlBackend::GetControl()
{
  return this->window;
}

void WebViewHtmlBackend::SetFontSizes(const std::array<int, 7> sizes)
{

}

const std::array<int, 7>& WebViewHtmlBackend::GetFontSizes() const
{
}

const wxString WebViewHtmlBackend::GetHTMLContents() const
{
  return this->contents;
}
