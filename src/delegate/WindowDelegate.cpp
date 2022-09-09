#include "WindowDelegate.h"
#include "include/cef_app.h"
#include "include/views/cef_display.h"
#include "include/views/cef_box_layout.h"

bool WindowDelegate::IsFrameless(CefRefPtr<CefWindow> window) {
    if (isDevTool) {
        return false;
    }
    return true;
}
bool WindowDelegate::CanResize(CefRefPtr<CefWindow> window)
{
    return true;
}
void WindowDelegate::OnWindowCreated(CefRefPtr<CefWindow> window) {

      // Use a vertical box layout for |window|.
    CefBoxLayoutSettings window_layout_settings;
    window_layout_settings.horizontal = false;
    window_layout_settings.between_child_spacing = 0;
    window_layout_settings.main_axis_alignment = CEF_MAIN_AXIS_ALIGNMENT_START;

    CefRefPtr<CefBoxLayout> window_layout = window->SetToBoxLayout(window_layout_settings);

    window->AddChildView(titleView);
    window->AddChildView(browserView);

    // Allow |browser_view_| to grow and fill any remaining space.
    window_layout->SetFlexForView(browserView, 1);
    window_layout->SetFlexForView(titleView, 14);

    CefRefPtr<CefDisplay> display = CefDisplay::GetPrimaryDisplay();
    CefRect rect = display->GetBounds();
    browserView->SetSize(CefSize(rect.width,rect.height));

    window->Show();
    browserView->RequestFocus();
    window->SetTitle(L"安");
    window->SetFullscreen(true);
    //window->CenterWindow(CefSize(1366, 768));
}
void WindowDelegate::OnWindowDestroyed(CefRefPtr<CefWindow> window) {
    browserView = nullptr;
    titleView = nullptr;
}

void WindowDelegate::OnWindowChanged(CefRefPtr<CefView> view, bool added)
{
    int cc = 0;
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