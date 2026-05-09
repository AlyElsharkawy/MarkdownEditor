#include "HtmlBackendFactory.h"
#include "SimpleHtmlBackend.h"
#include "WebViewHtmlBackend.h"

HtmlBackend *BackendFactory::CreateBackend(wxWindow *parent, BACKEND_TYPES type)
{
  switch (type)
  {
    case BACKEND_TYPES::SIMPLE_HTML_WINDOW:
      return new SimpleHtmlBackend(parent);
    case BACKEND_TYPES::WEB_VIEW_HTML_WINDOW:
      return new WebViewHtmlBackend(parent);
  }
}
