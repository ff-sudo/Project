#include "WindowHandler.h"

bool WindowHandler::OnQuery(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int64 query_id, const CefString& request, bool persistent, CefRefPtr<Callback> callback) {
	return true;
}
