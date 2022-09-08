#pragma once
#include "include/cef_app.h"
class HttpSchemeFactory :public CefSchemeHandlerFactory
{
public:
    HttpSchemeFactory() = default;
    CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request) override;
    HttpSchemeFactory(const HttpSchemeFactory&) = delete;
    HttpSchemeFactory& operator=(const HttpSchemeFactory&) = delete;
private:
    IMPLEMENT_REFCOUNTING(HttpSchemeFactory);
};

