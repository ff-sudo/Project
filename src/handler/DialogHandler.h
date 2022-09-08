#pragma once
#include "include/wrapper/cef_message_router.h"
#include "include/cef_browser.h"
#include "Helper/json.hpp"
using nlohmann::json;
class DialogHandler : public CefRunFileDialogCallback
{
public:
	DialogHandler(std::string& msgName, CefRefPtr<CefFrame> frame) :msgName(msgName), frame(frame) {};
	void OnFileDialogDismissed(int selected_accept_filter, const std::vector<CefString>& file_paths) override {
		json result;
		result["success"] = true;
		result["data"] = {};
		for (size_t i = 0; i < file_paths.size(); i++)
		{
			result["data"].push_back(file_paths[i].ToString());
		}
		CefRefPtr<CefProcessMessage> msgBack = CefProcessMessage::Create(msgName);
		CefRefPtr<CefListValue> msgArgs = msgBack->GetArgumentList();
		std::string dataStr = result.dump();
		msgArgs->SetString(0, dataStr);
		frame->SendProcessMessage(PID_RENDERER, msgBack);
	}
	DialogHandler(const DialogHandler&) = delete;
	DialogHandler& operator=(const DialogHandler&) = delete;
private:
	std::string msgName;
	CefRefPtr<CefFrame> frame;
	IMPLEMENT_REFCOUNTING(DialogHandler);
};
