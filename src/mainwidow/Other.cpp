#include "Other.h"
void Other::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
	registrar->AddCustomScheme("my", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_CORS_ENABLED);
}