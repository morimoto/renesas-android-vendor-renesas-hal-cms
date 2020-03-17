#ifndef VENDOR_RENESAS_GRAPHICS_CMS_V1_0_CMS_H
#define VENDOR_RENESAS_GRAPHICS_CMS_V1_0_CMS_H

#include <vendor/renesas/graphics/cms/1.0/ICms.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <vendor/renesas/graphics/composer/2.0/IComposer.h>

#include "libcmsbcm/cms_basic.h"
#include "libcmsdgc/cms_types.h"
#include "libcmsdgc/cms_dgc_api.h"
#include "libcmsblc/cms_blc_api.h"

namespace vendor {
namespace renesas {
namespace graphics {
namespace cms {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::android::sp;

using vendor::renesas::graphics::composer::V2_0::IComposer;

struct Cms : public ICms {

    Cms();
    // Methods from ::vendor::renesas::graphics::cms::V1_0::ICms follow.
    Return<::vendor::renesas::graphics::cms::V1_0::Error> cmsReset() override;
    Return<::vendor::renesas::graphics::cms::V1_0::Error> cmsSetLut(const hidl_vec<uint32_t>& buff) override;
    Return<::vendor::renesas::graphics::cms::V1_0::Error> cmsSetClu(const hidl_vec<uint32_t>& buff) override;
    Return<void> cmsGetHgo(cmsGetHgo_cb _hidl_cb) override;
    Return<::vendor::renesas::graphics::cms::V1_0::Error> cmsSetContrast(float contrast) override;
    Return<::vendor::renesas::graphics::cms::V1_0::Error> cmsSetBrightness(float brightness) override;
    Return<::vendor::renesas::graphics::cms::V1_0::Error> cmsSetTint(float angle) override;
    Return<::vendor::renesas::graphics::cms::V1_0::Error> cmsSetColor(float gain) override;
private:
    sp<IComposer> mComposer;
    CMS_BAS_PARAM sParam;

    Return<::vendor::renesas::graphics::cms::V1_0::Error> update();
    void init(uint32_t* table1d, uint32_t* table3d);
    void reset_param();
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace cms
}  // namespace graphics
}  // namespace renesas
}  // namespace vendor

#endif  // VENDOR_RENESAS_GRAPHICS_CMS_V1_0_CMS_H
