/*
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/xe_hpg_core/hw_cmds_dg2.h"

#include "platforms.h"

namespace NEO {
template <>
uint64_t CompilerProductHelperHw<IGFX_DG2>::getHwInfoConfig(const HardwareInfo &hwInfo) const {
    if (DG2::isG10(hwInfo)) {
        return 0x800040010;
    }
    return 0x200040010;
}

template <>
uint32_t CompilerProductHelperHw<IGFX_DG2>::getProductConfigFromHwInfo(const HardwareInfo &hwInfo) const {
    if (DG2::isG10(hwInfo)) {
        switch (hwInfo.platform.usRevId) {
        case 0x0:
            return AOT::DG2_G10_A0;
        case 0x1:
            return AOT::DG2_G10_A1;
        case 0x4:
            return AOT::DG2_G10_B0;
        case 0x8:
            return AOT::DG2_G10_C0;
        }
    } else if (DG2::isG11(hwInfo)) {
        switch (hwInfo.platform.usRevId) {
        case 0x0:
            return AOT::DG2_G11_A0;
        case 0x4:
            return AOT::DG2_G11_B0;
        case 0x5:
            return AOT::DG2_G11_B1;
        }
    } else if (DG2::isG12(hwInfo)) {
        return AOT::DG2_G12_A0;
    }
    return AOT::UNKNOWN_ISA;
}
} // namespace NEO
