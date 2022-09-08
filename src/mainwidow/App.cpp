#include "App.h"
#include "handler/PageHandler.h"
#include "include/cef_browser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"
#include "delegate/WindowDelegate.h"
#include "Scheme/HttpSchemeFactory.h"
#include "delegate/ViewDelegate.h"


void App::OnContextInitialized() {
	CEF_REQUIRE_UI_THREAD(); 
	CefRegisterSchemeHandlerFactory("my", "liulun", new HttpSchemeFactory());   
	std::string url = "www.baidu.com";
	std::string url2 = "my://liulun/title.html?a=123";
	CefBrowserSettings settings;
	CefRefPtr<PageHandler> pageHandler(new PageHandler());
	CefRefPtr<PageHandler> pageHandler2(new PageHandler());
	CefRefPtr<ViewDelegate> viewDelegate(new ViewDelegate());
	CefRefPtr<CefBrowserView> browserView = CefBrowserView::CreateBrowserView(pageHandler, url, settings, nullptr, nullptr, viewDelegate);
	CefRefPtr<CefBrowserView> titleView = CefBrowserView::CreateBrowserView(pageHandler, url2, settings, nullptr, nullptr, nullptr);

    CefWindow::CreateTopLevelWindow(new WindowDelegate(browserView, titleView,false));
}
void App::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {	
	registrar->AddCustomScheme("my", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_CORS_ENABLED);
}
