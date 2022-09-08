#pragma once
#include "include/cef_app.h"
#include "handler/V8Handler.h"
#include "include/wrapper/cef_message_router.h"
//--renderer-startup-dialog 
class Renderer:public CefApp, public CefRenderProcessHandler
{
public:
	Renderer() = default;
	void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
	bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }
	void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
private:	
	CefRefPtr<V8Handler> v8Handler;
	IMPLEMENT_REFCOUNTING(Renderer);
};

