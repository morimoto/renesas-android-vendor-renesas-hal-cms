#define LOG_TAG "CMS_HAL"

#include <android-base/logging.h>
#include <android-base/macros.h>

#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include <vendor/renesas/graphics/cms/1.0/ICms.h>

#include "Cms.h"

using ::android::sp;
using ::android::hardware::joinRpcThreadpool;
using ::android::hardware::configureRpcThreadpool;
using ::vendor::renesas::graphics::cms::V1_0::ICms;
using ::vendor::renesas::graphics::cms::V1_0::implementation::Cms;

int main(void) {
    sp<ICms> cms_hal = new Cms;
    configureRpcThreadpool(1, true);

    const auto status = cms_hal->registerAsService();
    CHECK_EQ(status, android::OK) << "Failed to register ICms";

    joinRpcThreadpool();
}
