#include <windows.h>
#include "mainwidow/App.h"
#include "mainwidow/Renderer.h"
#include "mainwidow/Other.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow) {
    CefEnableHighDPISupport();
    CefMainArgs main_args(hInstance);
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());
    CefRefPtr<CefApp> app;
    if (!command_line->HasSwitch("type"))
    {
        app = new App();
    }
    else if (command_line->GetSwitchValue("type").ToString() == "renderer")
    {
        DLOG(INFO) << "renderer:" << command_line->GetSwitchValue("type").ToString();
        app = new Renderer();
    }
    else
    {
        app = new Other();
    }
    int exit_code = CefExecuteProcess(main_args, app, nullptr);
    DLOG(INFO) << "process exit code" << exit_code << command_line->GetSwitchValue("type").ToString();
    if (exit_code >= 0) {        
        return exit_code;
    } 
    CefSettings settings;
    //std::wstring sLang = L"zh-CN";
    //cef_string_copy(sLang.c_str(), static_cast<int>(sLang.length()), &settings.locale);
    CefInitialize(main_args, settings, app.get(), nullptr);    
    CefRunMessageLoop();
    CefShutdown();
    DLOG(INFO) << "main process exit";
    return 0;
}