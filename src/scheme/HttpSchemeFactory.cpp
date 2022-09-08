#include "HttpSchemeFactory.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_stream_resource_handler.h"
#include <fstream>
#include <filesystem>
#include <iostream>

CefRefPtr<CefResourceHandler> HttpSchemeFactory::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)
{
    CEF_REQUIRE_IO_THREAD();
    std::string url = request->GetURL().ToString();
    std::string urlPrefix = "my://liulun/";
    url.erase(0, urlPrefix.size()); 
    size_t paramIndex = url.find_first_of('?');
    if (paramIndex != std::string::npos) {
        url.erase(paramIndex);
    }
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::filesystem::path targetPath(buffer);
    targetPath = targetPath.parent_path().append("html").append(url);
    if (!std::filesystem::exists(targetPath)) {
        DLOG(INFO) << L"试图加载：" << targetPath.generic_wstring() << L"，但找不到这个文件";
    }
    std::string ext = targetPath.extension().generic_string();
    std::string mime_type_ = ext == ".html"? "text/html":"*";
    auto stream = CefStreamReader::CreateForFile(targetPath.generic_wstring());
    return new CefStreamResourceHandler(mime_type_, stream);
};
