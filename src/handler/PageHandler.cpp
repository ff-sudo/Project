#include "PageHandler.h"
#include "include/wrapper/cef_helpers.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/base/cef_callback.h"
#include "include/wrapper/cef_closure_task.h"
#include "DialogHandler.h"
#include "resource.h"
#include <Windows.h>
#include <winternl.h>
#include <commdlg.h>
#include <string>
#include <vector>
#include <codecvt>
#include "Helper/json.hpp"
#include <iostream>
#include <fstream>
#include "SQLite/sqlite3.h"

namespace{
    LPWSTR infoValue;
    LPWSTR userInputValue;
    LPWSTR defaultValue;
    BOOL CALLBACK PromptProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam) {
        BOOL result = FALSE;
        switch (message)
        {
            case WM_INITDIALOG:{
                SetDlgItemText(hwndDlg, IDC_INPUT, defaultValue);
                SetDlgItemText(hwndDlg, IDC_LABEL, infoValue);
                HWND hwndInput = GetDlgItem(hwndDlg, IDC_INPUT);
                SetFocus(hwndInput);
                SendMessage(hwndInput, EM_SETSEL, 0, -1);
                break;
            }
            case WM_COMMAND: {
                switch (LOWORD(wParam))
                {
                    case IDOK: {
                        HWND hwndInput = GetDlgItem(hwndDlg, IDC_INPUT);
                        int outLength = GetWindowTextLength(hwndInput) + 1;
                        userInputValue = new TCHAR[outLength];
                        GetDlgItemText(hwndDlg, IDC_INPUT, userInputValue, outLength);
                        EndDialog(hwndDlg, wParam);
                        result = TRUE;
                        break;
                    }
                    case IDCANCEL: {
                        EndDialog(hwndDlg, wParam);
                        result = FALSE;
                        break;
                    }                        
                }
                break;
            }
        }
        return FALSE;
    }
}

void PageHandler::OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const std::vector<CefDraggableRegion>& regions)
{
    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::GetForBrowser(browser);
    if (browser_view)
    {
        CefRefPtr<CefWindow> window = browser_view->GetWindow();
        if (window) window->SetDraggableRegions(regions);
    }
}

void PageHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
    model->Clear();
    model->AddItem(MENU_ID_USER_FIRST, L"打开开发者调试工具");
    CefRefPtr<CefMenuModel> subModel = model->AddSubMenu(MENU_ID_USER_FIRST + 1, L"这是一个包含子菜单的测试菜单");
    subModel->AddItem(MENU_ID_USER_FIRST + 2, L"这是子菜单1");
    subModel->AddItem(MENU_ID_USER_FIRST + 3, L"这是子菜单2");
    model->AddSeparator();
    model->AddCheckItem(MENU_ID_USER_FIRST + 4, L"这是一个包含复选框的菜单");
    model->SetChecked(MENU_ID_USER_FIRST + 4,true);
    model->AddRadioItem(MENU_ID_USER_FIRST + 5, L"这是一个包含复选框的菜单",888);
    model->AddRadioItem(MENU_ID_USER_FIRST + 6, L"这是一个包含单选框的菜单", 888);
    model->SetChecked(MENU_ID_USER_FIRST + 6, true);
}
bool PageHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, int command_id, EventFlags event_flags)
{
    switch (command_id)
    {
        case MENU_ID_USER_FIRST: {
            CefBrowserSettings browserSettings;
            CefWindowInfo windowInfo;
            CefPoint mousePoint(params->GetXCoord(), params->GetYCoord());
            browser->GetHost()->ShowDevTools(windowInfo, this, browserSettings, mousePoint);
            break;
        }
        default: {
            std::wstring msg = L"你点击的标签ID：" + std::to_wstring(command_id);
            MessageBox(NULL, (LPWSTR)msg.c_str(), L"系统提示", MB_ICONEXCLAMATION | MB_OKCANCEL);
            break;
        }
    }
    return true;
}

void PageHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();    
    browsers.push_back(browser);
}
void PageHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
    CEF_REQUIRE_UI_THREAD();
    std::list<CefRefPtr<CefBrowser>>::iterator bit = browsers.begin();
    for (; bit != browsers.end(); ++bit) {
        if ((*bit)->IsSame(browser)) {
            browsers.erase(bit);
            break;
        }
    }
    if (browsers.empty()) {
        CefQuitMessageLoop();
    }
}
bool PageHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser, const CefString& message_text, bool is_reload, CefRefPtr<CefJSDialogCallback> callback) {
    //message_text.c_str() 的值是Is it OK to leave/refresh this page
    int msgboxID = MessageBox( NULL, L"您编辑的内容尚未保存.\n确定要关闭窗口吗?", L"系统提示", MB_ICONEXCLAMATION | MB_OKCANCEL);
    if (msgboxID == IDOK)
    {
        callback->Continue(true, CefString());
    }
    else
    {
        callback->Continue(false, CefString());
    }
    return true;
}
bool PageHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
    const CefString& origin_url,
    JSDialogType dialog_type,
    const CefString& message_text,
    const CefString& default_prompt_text,
    CefRefPtr<CefJSDialogCallback> callback,
    bool& suppress_message){
    suppress_message = false;
    HWND hwnd = browser->GetHost()->GetWindowHandle();
    if (dialog_type == JSDialogType::JSDIALOGTYPE_ALERT) {    
        //CefPostDelayedTask(TID_UI, base::BindOnce(&ShowAlert,hwnd,message_text,callback),2000);
        MessageBox(hwnd, message_text.c_str(), L"系统提示", MB_ICONEXCLAMATION | MB_OK);
        callback->Continue(true, CefString()); 
    }
    else if(dialog_type == JSDialogType::JSDIALOGTYPE_CONFIRM)
    {
        int msgboxID = MessageBox(hwnd, message_text.c_str(), L"系统提示", MB_ICONEXCLAMATION | MB_YESNO);
        callback->Continue(msgboxID == IDYES, CefString()); 
    }
    else if (dialog_type == JSDialogType::JSDIALOGTYPE_PROMPT)
    {
        HINSTANCE hinst = GetModuleHandle(NULL);
        defaultValue = (LPWSTR)default_prompt_text.c_str();
        infoValue = (LPWSTR)message_text.c_str();
        BOOL result = DialogBox(hinst, MAKEINTRESOURCE(IDD_PROMPT), hwnd, (DLGPROC)PromptProc);
        if (result) {
            
            callback->Continue(result, CefString(userInputValue));
            delete []userInputValue;
        }
        else
        {
            callback->Continue(result, CefString());
        }
    }
    
    return true;
}

std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> elems;
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

const std::string getOSVersion()
{
    //https://docs.microsoft.com/en-us/windows/win32/sysinfo/operating-system-version
    NTSTATUS(WINAPI* RtlGetVersion)(LPOSVERSIONINFOEXW);
    OSVERSIONINFOEXW osInfo;
    std::string result;
     *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandle(L"ntdll"), "RtlGetVersion");
    if (nullptr != RtlGetVersion) {
        osInfo.dwOSVersionInfoSize = sizeof osInfo;
        RtlGetVersion(&osInfo);
        result = std::to_string(osInfo.dwMajorVersion) +"." + std::to_string(osInfo.dwMinorVersion) +"." + std::to_string(osInfo.dwBuildNumber);
    }
    return result;
}

std::string convertStr(const std::wstring& wide_string)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.to_bytes(wide_string);
}

std::wstring convertStr(const std::string& str)
{
    static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
    return utf8_conv.from_bytes(str);
}

void ReadFileByBlocks(const std::string& filePath,const std::string& msgName, CefRefPtr<CefFrame> frame) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
    std::ifstream fileStream(filePath, std::ifstream::binary);
    if (fileStream) {
        fileStream.seekg(0, fileStream.end);
        long length = fileStream.tellg();
        fileStream.seekg(0, fileStream.beg);
        long size = 1024;
        long position = 0;
        while (position != length) {
            long leftSize = length - position;
            if (leftSize < size) {
                size = leftSize;
            }
            char* buffer = new char[size];
            fileStream.read(buffer, size);
            position = fileStream.tellg();
            CefRefPtr<CefBinaryValue> data = CefBinaryValue::Create(buffer, size);
            CefRefPtr<CefProcessMessage> msgBack = CefProcessMessage::Create(msgName+"_data");
            CefRefPtr<CefListValue> msgArgs = msgBack->GetArgumentList();
            msgArgs->SetBinary(0, data);
            frame->SendProcessMessage(PID_RENDERER, msgBack);
            delete[] buffer;
        }
        fileStream.close();
        CefRefPtr<CefProcessMessage> msgBack = CefProcessMessage::Create(msgName + "_finish");
        frame->SendProcessMessage(PID_RENDERER, msgBack);
    }
}

static sqlite3* db;

void ExecuteSql(const std::string& sqlStr, const std::string& msgName, CefRefPtr<CefFrame> frame) {
    json result;
    result["data"] = {};
    const char* zTail = sqlStr.c_str();
    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    while (strlen(zTail) != 0) {
        sqlite3_stmt* stmt = NULL;
        int prepareResult = sqlite3_prepare_v2(db, zTail, -1, &stmt, &zTail); //判断 prepareResult == SQLITE_OK
        int stepResult = sqlite3_step(stmt);
        while (stepResult == SQLITE_ROW) {
            json row;
            int columnCount = sqlite3_column_count(stmt);
            for (size_t i = 0; i < columnCount; i++) {
                std::string columnName = sqlite3_column_name(stmt, i);
                int type = sqlite3_column_type(stmt, i);
                if (type == SQLITE_INTEGER) {
                    row[columnName] = sqlite3_column_int(stmt, i);
                }
                else if (type == SQLITE3_TEXT)
                {
                    const unsigned char* val = sqlite3_column_text(stmt, i);
                    row[columnName] = reinterpret_cast<const char*>(val);
                }
            }
            result["data"].push_back(row);
            stepResult = sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    }
    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(msgName);
    CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();
    result["success"] = true;
    msgArgs->SetString(0, result.dump());
    frame->SendProcessMessage(PID_RENDERER, msg);
}

bool PageHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
    CEF_REQUIRE_UI_THREAD();
    std::string messageName = message->GetName();
    std::vector<std::string> arr = split(messageName, '_');
    if (arr.at(0) == "window"){
        CefRefPtr<CefBrowserView> browserView = CefBrowserView::GetForBrowser(browser);
        CefRefPtr<CefWindow> window = browserView->GetWindow();
        if (arr.at(1) == "minimize") {
            window->Minimize();
        }
        else if (arr.at(1) == "maximize") {
            window->Maximize();
        }
        else if (arr.at(1) == "close") {
            window->Close();
        }
        else if (arr.at(1) == "restore") {
            window->Restore();
        }
    }
    else if(arr.at(0) == "system") {
        CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(messageName);
        CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();
        if (arr.at(1) == "getOSVersion") {
            std::string version = getOSVersion();
            msgArgs->SetString(0, version);
        }
        frame->SendProcessMessage(PID_RENDERER, msg);
    }
    else if (arr.at(0) == "dialog") {
        CefRefPtr<CefListValue> msgBody = message->GetArgumentList();
        nlohmann::json param = nlohmann::json::parse(msgBody->GetString(0).ToString()); 
        CefRefPtr<CefRunFileDialogCallback> dcb = new DialogHandler(messageName, frame);
        CefBrowserHost::FileDialogMode mode;
        auto title = convertStr(param["title"].get<std::string>());
        auto defaultPath = convertStr(param["defaultPath"].get<std::string>());     
        std::vector<CefString> fileFilters;
        int filterIndex = 0;
        if (arr.at(1) == "openFile") {            
            for (const std::string& var : param["filters"]) {
                fileFilters.push_back(var);
            }
            filterIndex = param["filterIndex"].get<int>();
            mode = param["multiSelections"].get<bool>() ? FILE_DIALOG_OPEN_MULTIPLE : FILE_DIALOG_OPEN;
            browser->GetHost()->RunFileDialog(mode, title, defaultPath, fileFilters, filterIndex, dcb);
        }
        else if (arr.at(1) == "openFolder") {
            mode = FILE_DIALOG_OPEN_FOLDER;
            browser->GetHost()->RunFileDialog(mode, title, defaultPath, fileFilters, filterIndex, dcb);
        }
    }
    else if (arr.at(0) == "file") {
        if (arr.at(1) == "readFile") {
            CefRefPtr<CefListValue> msgBody = message->GetArgumentList();
            nlohmann::json param = nlohmann::json::parse(msgBody->GetString(0).ToString());
            std::string filePath = param["filePath"].get<std::string>();
            CefPostTask(TID_FILE_BACKGROUND, base::BindOnce(&ReadFileByBlocks, filePath,messageName, frame));
        }
    }
    else if (arr.at(0) == "db") {
        CefRefPtr<CefListValue> msgBody = message->GetArgumentList();       
        if (arr.at(1) == "open") {
            nlohmann::json param = nlohmann::json::parse(msgBody->GetString(0).ToString());
            std::string dbPath = param["dbPath"].get<std::string>();
            int rc = sqlite3_open(dbPath.c_str(), &db);
            CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(messageName);
            CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();
            json result;
            result["success"] = rc == 0;
            msgArgs->SetString(0, result.dump());
            frame->SendProcessMessage(PID_RENDERER, msg);
        }
        else if(arr.at(1) == "close") {
            int rc = sqlite3_close(db);
            CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(messageName);
            CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();
            json result;
            result["success"] = rc == 0;
            msgArgs->SetString(0, result.dump());
            frame->SendProcessMessage(PID_RENDERER, msg);
        }
        else if (arr.at(1) == "execute") {
            nlohmann::json param = nlohmann::json::parse(msgBody->GetString(0).ToString());
            std::string sqlStr = param["sql"].get<std::string>();
            CefPostTask(TID_FILE_BACKGROUND, base::BindOnce(&ExecuteSql, sqlStr, messageName, frame));
        }
    }
    return true;
    
}

