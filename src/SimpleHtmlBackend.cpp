#include "SimpleHtmlBackend.h"

SimpleHtmlBackend::SimpleHtmlBackend(wxWindow* parent)
{
  this->window = new wxHtmlWindow(parent, wxID_ANY);
  this->window->SetFonts("", "", this->fontSizes.data());
}

void SimpleHtmlBackend::SetFontSizes(const std::array<int, 7> newSizes)
{
  this->window->SetFonts("", "", newSizes.data());
}

void SimpleHtmlBackend::SetZoom(float newSize)
{
  if(newSize > 0.0f)
  {
    this->zoomLevel = newSize;
    std::array<int, 7> temp;
    for(int i = 0; i < 7; i++)
    {
      temp[i] = this->zoomLevel * this->fontSizes[i];
    }
    this->window->SetFonts("", "", temp.data());
  }
}

wxWindow* SimpleHtmlBackend::GetControl()
{
  return this->window;
}

void SimpleHtmlBackend::SetHtmlText(const wxString& html)
{
  this->window->SetPage(wxString::FromUTF8(html));
}

SimpleHtmlBackend::~SimpleHtmlBackend()
{
}


void SimpleHtmlBackend::SetCSSStyle(const wxString& style)
{
}

const std::array<int, 7>& SimpleHtmlBackend::GetFontSizes() const
{
  return this->fontSizes;
}

const wxString SimpleHtmlBackend::GetHTMLContents() const
{
  return this->window->ToText();
}
