#pragma once
#include "include/views/cef_window.h"
#include "include/views/cef_browser_view.h"
class WindowDelegate : public CefWindowDelegate
{
public:
	explicit WindowDelegate(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowserView> title_View, bool dev_tool) : browserView(browser_view),titleView(title_View), isDevTool(dev_tool) {};
	//bool CanResize(CefRefPtr<CefWindow> window) override { return false; }
	void OnWindowCreated(CefRefPtr<CefWindow> window) override;
	void OnWindowDestroyed(CefRefPtr<CefWindow> window) override;
	bool CanClose(CefRefPtr<CefWindow> window) override;
	CefRect GetInitialBounds(CefRefPtr<CefWindow> window) override;
	bool IsFrameless(CefRefPtr<CefWindow> window) override;
	WindowDelegate(const WindowDelegate&) = delete;
	WindowDelegate& operator=(const WindowDelegate&) = delete;
private:
	CefRefPtr<CefBrowserView> browserView;
	CefRefPtr<CefBrowserView> titleView;
	CefRefPtr<CefOverlayController> overlayController;
	bool isDevTool;
	IMPLEMENT_REFCOUNTING(WindowDelegate);
};

