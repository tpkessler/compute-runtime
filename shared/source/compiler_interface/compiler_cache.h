/*
 * Copyright (C) 2019-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#pragma once

#include "shared/source/utilities/arrayref.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace NEO {
struct HardwareInfo;

struct CompilerCacheConfig {
    bool enabled = true;
    std::string cacheFileExtension;
    std::string cacheDir;
    size_t cacheSize = 0;
};

class CompilerCache {
  public:
    CompilerCache(const CompilerCacheConfig &config);
    virtual ~CompilerCache() = default;

    CompilerCache(const CompilerCache &) = delete;
    CompilerCache(CompilerCache &&) = delete;
    CompilerCache &operator=(const CompilerCache &) = delete;
    CompilerCache &operator=(CompilerCache &&) = delete;
    const CompilerCacheConfig &getConfig() {
        return config;
    }

    const std::string getCachedFileName(const HardwareInfo &hwInfo, ArrayRef<const char> input,
                                        ArrayRef<const char> options, ArrayRef<const char> internalOptions);

    MOCKABLE_VIRTUAL bool cacheBinary(const std::string &kernelFileHash, const char *pBinary, size_t binarySize);
    MOCKABLE_VIRTUAL std::unique_ptr<char[]> loadCachedBinary(const std::string &kernelFileHash, size_t &cachedBinarySize);

  protected:
    MOCKABLE_VIRTUAL bool evictCache();
    MOCKABLE_VIRTUAL bool renameTempFileBinaryToProperName(const std::string &oldName, const std::string &kernelFileHash);
    MOCKABLE_VIRTUAL bool createUniqueTempFileAndWriteData(char *tmpFilePathTemplate, const char *pBinary, size_t binarySize);
    MOCKABLE_VIRTUAL void lockConfigFileAndReadSize(const std::string &configFilePath, int &fd, size_t &directorySize);

    static std::mutex cacheAccessMtx;
    CompilerCacheConfig config;
};
} // namespace NEO
