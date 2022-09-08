#include "WindowDelegate.h"
#include "include/cef_app.h"
#include "include/views/cef_display.h"
bool WindowDelegate::IsFrameless(CefRefPtr<CefWindow> window) {
    if (isDevTool) {
        return false;
    }
    return true;
}
void WindowDelegate::OnWindowCreated(CefRefPtr<CefWindow> window) {
    window->AddChildView(browserView);

    // 添加一个子窗口
    overlayController = window->AddOverlayView(titleView, CEF_DOCKING_MODE_CUSTOM);
    overlayController->SetVisible(true);
    overlayController->SetSize(CefSize(browserView->GetSize().width, 30));

    window->Show();
    //browserView->RequestFocus();
    window->SetTitle(L"安");
    //window->CenterWindow(CefSize(800, 600));
}
void WindowDelegate::OnWindowDestroyed(CefRefPtr<CefWindow> window) {
    browserView = nullptr;
}

bool WindowDelegate::CanClose(CefRefPtr<CefWindow> window) {
    bool result = true;
    CefRefPtr<CefBrowser> browser = browserView->GetBrowser();
    if (browser) {
        result = browser->GetHost()->TryCloseBrowser();
    }
    return result;
}

CefRect WindowDelegate::GetInitialBounds(CefRefPtr<CefWindow> window) {
    CefRefPtr<CefDisplay> display = CefDisplay::GetPrimaryDisplay();
    CefRect rect = display->GetBounds();
    rect.x = (rect.width - 800) / 2;
    rect.y = (rect.height - 600) / 2;
    rect.width = 800;
    rect.height = 600;
    return rect;
}