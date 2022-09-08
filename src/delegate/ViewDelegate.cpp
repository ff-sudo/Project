#include "ViewDelegate.h"
#include "WindowDelegate.h"

bool ViewDelegate::OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browserView, CefRefPtr<CefBrowserView> popupBrowserView, bool isDevtools)
{
    CefWindow::CreateTopLevelWindow(new WindowDelegate(popupBrowserView,isDevtools));
    return true;
}
