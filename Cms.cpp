#include "Cms.h"

// Macro
#define LUT_NUM 256
#define CLU_NUM (17 * 17 * 17)
#define HGO_NUM (64 * 3)

namespace vendor {
namespace renesas {
namespace graphics {
namespace cms {
namespace V1_0 {
namespace implementation {

using vendor::renesas::graphics::composer::V2_0::IComposer;
using vendor::renesas::graphics::cms::V1_0::Error;

Cms::Cms() {
    mComposer = IComposer::getService();
    reset_param();
    cmsReset();
}

// Methods from ::vendor::renesas::graphics::cms::V1_0::ICms follow.
Return<Error> Cms::cmsReset() {
    ::android::hardware::graphics::composer::V2_1::Error err = mComposer->cmsReset(0);
    reset_param();
    return static_cast<Error>(err);
}

Return<Error> Cms::cmsSetLut(const hidl_vec<uint32_t>& buff) {
    ::android::hardware::graphics::composer::V2_1::Error err = mComposer->cmsSetLut(buff, 0);
    return static_cast<Error>(err);
}

Return<Error> Cms::cmsSetClu(const hidl_vec<uint32_t>& buff) {

    ::android::hardware::graphics::composer::V2_1::Error err = mComposer->cmsSetClu(buff, 0);
    return static_cast<Error>(err);
}

Return<void> Cms::cmsGetHgo(cmsGetHgo_cb _hidl_cb) {
    mComposer->cmsGetHgo(0, _hidl_cb);
    return Void();
}

Return<Error> Cms::cmsSetContrast(float contrast) {
    if (contrast <= 50) {
        contrast = 0.01f + 0.02f * contrast;
    } else {
        contrast = 1 + 0.06f * (contrast - 50);
    }
    sParam.bas.contrast = (CMS_BAS_U16) (contrast * 512.0f) - 1; // 0..2047; contrast: 0..4; 0x1ff - default (511)
    return update();
}

Return<Error> Cms::cmsSetBrightness(float brightness) {
    sParam.bas.brightness = (CMS_BAS_S16)(5.10f * brightness - 255.0f);               // -255..255
    return update();
}

Return<Error> Cms::cmsSetTint(float angle) {
    sParam.bas.tint = (CMS_BAS_S16)((0.9f * angle - 45.0f) * 255.0f/45.0f);                     // -255..255; tint = anglee*256/45 - 1
    return update();
}

Return<Error> Cms::cmsSetColor(float gain) {
    if (gain <= 50) {
        gain = 0.01f + 0.02f * gain;
    } else {
        gain = 1 + 0.06f * (gain - 50);
    }
    sParam.bas.color = (CMS_BAS_U16) (gain * 512.0f) - 1; // 0..2047; color = gain*512 - 1
    return update();
}

void Cms::init(uint32_t* table1d, uint32_t* table3d) {
    // set default LUT
    for (int i = 0; i < LUT_NUM; ++i) {
        table1d[i] = ((i << 16) | (i << 8) | (i << 0));
    }

    // set default CLU
    for (int i = 0; i < CLU_NUM; ++i) {
        int index = i; int r = index % 17; r = (r << 20); if (r > (255 << 16)) r = (255 << 16);
        index /= 17;   int g = index % 17; g = (g << 12); if (g > (255 <<  8)) g = (255 <<  8);
        index /= 17;   int b = index % 17; b = (b <<  4); if (b > (255 <<  0)) b = (255 <<  0);
        table3d[i] = r | g | b;
    }
}

Return<Error> Cms::update() {

    // create buff
    uint32_t table1d[LUT_NUM];
    uint32_t table3d[CLU_NUM];
    init(table1d, table3d);

    CMS_BAS_LUT_BUF buff;
    buff.tbl_form = CMS_BAS_LUT_FORM_DL0;
    buff.tbl1d_data = (CMS_BAS_U32*) table1d;
    buff.tbl1d_size = LUT_NUM * 4;
    buff.tbl3d_data = (CMS_BAS_U32*) table3d;
    buff.tbl3d_size = CLU_NUM * 4;

    // call to bcm
    cms_basic_adjust(&sParam, &buff);

    // push to composer
    {
        android::hardware::hidl_vec<uint32_t> buff;
        buff.setToExternal(table3d, CLU_NUM);
        mComposer->cmsSetClu(buff, 0);
    }

    {
        android::hardware::hidl_vec<uint32_t> buff;
        buff.setToExternal(table1d, LUT_NUM);
        mComposer->cmsSetLut(buff, 0);
    }
    return Error::NONE;
}

void Cms::reset_param() {
    // param
    sParam.input_form = CMS_BAS_INPUT_FORM_RGB;
    sParam.input_level = CMS_BAS_INPUT_LEVEL_0_255;

    sParam.bas.contrast = 0x1ff;             // 0..2047; contrast: 0..4; (contrast*512.0f) - 1; 0x1ff - default (511)
    sParam.bas.brightness = 0;               // -255..255 (-219..219)
    sParam.bas.tint = 0;                     // -255..255; tint = anglee*256/45 - 1
    sParam.bas.color = 0x1ff;                // 0..2047; color = gain*512 - 1

    sParam.bas.cbcr_gain.gain_cr = 0xff;     // 0..511; 0xff - default (255); gain_cbr = gain*256 - 1
    sParam.bas.cbcr_gain.gain_cb = 0xff;     // 0..511

    sParam.bas.clu_rgb_gain.gain_r = 0x1ff;  // 0..2047; gain_rgb = gain*512 - 1
    sParam.bas.clu_rgb_gain.gain_g = 0x1ff;
    sParam.bas.clu_rgb_gain.gain_b = 0x1ff;
    sParam.bas.clu_rgb_offset.offset_r = 0;  // -255..255
    sParam.bas.clu_rgb_offset.offset_g = 0;
    sParam.bas.clu_rgb_offset.offset_b = 0;

    // RGB gain/offset used in both CLU & LUT
    // but calulation accuracy in LUT is higher
    sParam.bas.lut_rgb_gain.gain_r = 0x1ff;  // 0..2047
    sParam.bas.lut_rgb_gain.gain_g = 0x1ff;
    sParam.bas.lut_rgb_gain.gain_b = 0x1ff;
    sParam.bas.lut_rgb_offset.offset_r = 0;  // -255..255
    sParam.bas.lut_rgb_offset.offset_g = 0;
    sParam.bas.lut_rgb_offset.offset_b = 0;

    sParam.bas.lut_rgb_gamma.gamma_enable = CMS_BAS_FALSE;
    //param.bas.lut_rgb_gamma.gamma_rgb[i] = i (default) i: 0..255

    sParam.bas.matrix_num = CMS_BAS_MATRIX_NUM_SD; // HD
}


}  // namespace implementation
}  // namespace V1_0
}  // namespace cms
}  // namespace graphics
}  // namespace renesas
}  // namespace vendor
