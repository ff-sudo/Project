#pragma once
#include "include/wrapper/cef_message_router.h"
class WindowHandler:public CefMessageRouterBrowserSide::Handler
{
public: 
	WindowHandler() = default;
	virtual bool OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) override;
};

