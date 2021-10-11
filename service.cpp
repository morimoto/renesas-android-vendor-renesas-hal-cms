/*
 * Copyright (C) 2021 GlobalLogic
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
