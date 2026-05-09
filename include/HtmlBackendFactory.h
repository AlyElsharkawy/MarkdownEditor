#pragma once
#include "HtmlBackend.h"

enum class BACKEND_TYPES
{
  SIMPLE_HTML_WINDOW,
  WEB_VIEW_HTML_WINDOW
};

class BackendFactory
{
public:
    static HtmlBackend* CreateBackend(wxWindow* parent, BACKEND_TYPES type);
};
