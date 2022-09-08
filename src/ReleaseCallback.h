#pragma once
#include "handler/V8Handler.h"
class ReleaseCallback : public CefV8ArrayBufferReleaseCallback {
public:
    void ReleaseBuffer(void* buffer) override { 
        std::free(buffer); 
        DLOG(INFO) << "data release";
    }
    IMPLEMENT_REFCOUNTING(ReleaseCallback);
};
