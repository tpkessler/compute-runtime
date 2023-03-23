/*
 * Copyright (C) 2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once
#include "shared/source/gmm_helper/gmm_lib.h"

namespace NEO {
class Device;
class Gmm;
class GmmHelper;
class GraphicsAllocation;
class IndirectHeap;
class LinearStream;
struct EncodeSurfaceStateArgs;
struct HardwareInfo;
struct PipeControlArgs;
struct PipelineSelectArgs;
struct RootDeviceEnvironment;

template <typename GfxFamily>
struct EncodeSurfaceState {
    using R_SURFACE_STATE = typename GfxFamily::RENDER_SURFACE_STATE;
    using SURFACE_FORMAT = typename R_SURFACE_STATE::SURFACE_FORMAT;
    using AUXILIARY_SURFACE_MODE = typename R_SURFACE_STATE::AUXILIARY_SURFACE_MODE;
    using COHERENCY_TYPE = typename R_SURFACE_STATE::COHERENCY_TYPE;

    static void encodeBuffer(EncodeSurfaceStateArgs &args);
    static void encodeExtraBufferParams(EncodeSurfaceStateArgs &args);
    static void encodeImplicitScalingParams(const EncodeSurfaceStateArgs &args);
    static void encodeExtraCacheSettings(R_SURFACE_STATE *surfaceState, const EncodeSurfaceStateArgs &args);
    static void appendBufferSurfaceState(EncodeSurfaceStateArgs &args);

    static constexpr uintptr_t getSurfaceBaseAddressAlignmentMask() {
        return ~(getSurfaceBaseAddressAlignment() - 1);
    }

    static constexpr uintptr_t getSurfaceBaseAddressAlignment() { return 4; }

    static void getSshAlignedPointer(uintptr_t &ptr, size_t &offset);
    static bool doBindingTablePrefetch();
    static bool isBindingTablePrefetchPreferred();

    static size_t pushBindingTableAndSurfaceStates(IndirectHeap &dstHeap,
                                                   const void *srcKernelSsh, size_t srcKernelSshSize,
                                                   size_t numberOfBindingTableStates, size_t offsetOfBindingTable);

    static void appendImageCompressionParams(R_SURFACE_STATE *surfaceState, GraphicsAllocation *allocation, GmmHelper *gmmHelper,
                                             bool imageFromBuffer, GMM_YUV_PLANE_ENUM plane);
    static void setCoherencyType(R_SURFACE_STATE *surfaceState, COHERENCY_TYPE coherencyType);
    static void setBufferAuxParamsForCCS(R_SURFACE_STATE *surfaceState);
    static void setImageAuxParamsForCCS(R_SURFACE_STATE *surfaceState, Gmm *gmm);
    static bool isAuxModeEnabled(R_SURFACE_STATE *surfaceState, Gmm *gmm);
    static void setAuxParamsForMCSCCS(R_SURFACE_STATE *surfaceState);
    static void setClearColorParams(R_SURFACE_STATE *surfaceState, Gmm *gmm);
    static void setFlagsForMediaCompression(R_SURFACE_STATE *surfaceState, Gmm *gmm);
    static void disableCompressionFlags(R_SURFACE_STATE *surfaceState);
    static void appendParamsForImageFromBuffer(R_SURFACE_STATE *surfaceState);
};

} // namespace NEO