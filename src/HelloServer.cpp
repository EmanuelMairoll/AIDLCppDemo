#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "aidl_cpp_server"

#include <stdlib.h>
#include <unistd.h>
#include <utils/RefBase.h>
#include <utils/Log.h>
#include <binder/TextOutput.h>
#include <binder/IInterface.h>
#include <binder/IBinder.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include "com/yuandaima/IHello.h"
#include "com/yuandaima/BnHello.h"
#include "com/yuandaima/MyStruct.h"
#include <utils/String8.h>
#include <utils/String16.h>

using namespace android;

class IHelloServer : public com::yuandaima::BnHello {
public:
    virtual ~IHelloServer() override {}

    binder::Status hello() override {
        ALOGI("server: hello() called");
        return binder::Status();
    }
    
    binder::Status sum(int32_t x, int32_t y, int32_t* _aidl_return) override {
        ALOGI("server: sum(%d, %d) called", x, y);
        *_aidl_return = x + y;
        return binder::Status();
    }
    
    binder::Status waitAndCallback(int32_t seconds,
                                   const sp<com::yuandaima::IHelloCallback>& callback) override {
        ALOGI("server: waitAndCallback(%d) called", seconds);
        sleep(seconds);
        if (callback != nullptr) {
            ALOGI("server: calling callback->onWaitFinished()");
            callback->onWaitFinished();
        }
        return binder::Status();
    }
    
    binder::Status printStruct(const com::yuandaima::MyStruct& data) override {
        ALOGI("server: printStruct() called");
        ALOGI("server: data: %d", data.data);
        ALOGI("server: majorVersion: %f, minorVersion: %f", data.majorVersion, data.minorVersion);
        
        if (data.binder != nullptr) {
            ALOGI("server: binder is valid");
        } else {
            ALOGI("server: binder is null");
        }
        
        // Print the array contents.
        String8 arrayStr;
        for (size_t i = 0; i < data.array.size(); ++i) {
            arrayStr.appendFormat("%d ", data.array[i]);
        }
        ALOGI("server: array: %s", arrayStr.c_str());
        
        // For greatString (String16), convert to String8 for printing.
        ALOGI("server: greatString: %s", String8(data.greatString).c_str());
        
        // For greaterString (std::string), print directly.
        ALOGI("server: greaterString: %s", data.greaterString.c_str());
        
        return binder::Status();
    }
};

int main() {
    sp<IHelloServer> service = new IHelloServer();
    sp<IServiceManager> sm = defaultServiceManager();
    sm->addService(String16("IHello"), service);
    ALOGI("server: IHello service registered");
    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
    return 0;
}