/*
 * Copyright (C) 2018-2023 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "shared/source/execution_environment/root_device_environment.h"
#include "shared/source/os_interface/os_interface.h"
#include "shared/test/common/mocks/mock_execution_environment.h"
#include "shared/test/common/mocks/windows/mock_os_time_win.h"
#include "shared/test/common/os_interface/windows/wddm_fixture.h"

#include "gtest/gtest.h"

#include <memory>

using namespace NEO;

LARGE_INTEGER valueToSet = {0};

BOOL WINAPI QueryPerformanceCounterMock(
    _Out_ LARGE_INTEGER *lpPerformanceCount) {
    *lpPerformanceCount = valueToSet;
    return true;
};

struct OSTimeWinTest : public ::testing::Test {
  public:
    void SetUp() override {
        osTime = std::unique_ptr<MockOSTimeWin>(new MockOSTimeWin(nullptr));
    }

    void TearDown() override {
    }
    std::unique_ptr<MockOSTimeWin> osTime;
};

TEST_F(OSTimeWinTest, givenZeroFrequencyWhenGetHostTimerFuncIsCalledThenReturnsZero) {
    LARGE_INTEGER frequency;
    frequency.QuadPart = 0;
    osTime->setFrequency(frequency);
    auto retVal = osTime->getHostTimerResolution();
    EXPECT_EQ(0, retVal);
}

TEST_F(OSTimeWinTest, givenNonZeroFrequencyWhenGetHostTimerFuncIsCalledThenReturnsNonZero) {
    LARGE_INTEGER frequency;
    frequency.QuadPart = NSEC_PER_SEC;
    osTime->setFrequency(frequency);
    auto retVal = osTime->getHostTimerResolution();
    EXPECT_EQ(1.0, retVal);
}

TEST_F(OSTimeWinTest, givenOsTimeWinWhenGetCpuRawTimestampIsCalledThenReturnsNonZero) {
    auto retVal = osTime->getCpuRawTimestamp();
    EXPECT_NE(0ull, retVal);
}

TEST_F(OSTimeWinTest, givenHighValueOfCpuTimestampWhenItIsObtainedThenItHasProperValue) {
    osTime->overrideQueryPerformanceCounterFunction(QueryPerformanceCounterMock);
    LARGE_INTEGER frequency = {0};
    frequency.QuadPart = 190457;
    osTime->setFrequency(frequency);
    valueToSet.QuadPart = 700894514854;
    uint64_t timeStamp = 0;
    uint64_t expectedTimestamp = static_cast<uint64_t>((static_cast<double>(valueToSet.QuadPart) * static_cast<double>(NSEC_PER_SEC) / static_cast<double>(frequency.QuadPart)));
    osTime->getCpuTime(&timeStamp);
    EXPECT_EQ(expectedTimestamp, timeStamp);
}

TEST(OSTimeWinTests, givenNoOSInterfaceWhenGetCpuTimeThenReturnsSuccess) {
    uint64_t time = 0;
    auto osTime(OSTime::create(nullptr));
    auto error = osTime->getCpuTime(&time);
    EXPECT_TRUE(error);
    EXPECT_EQ(0u, time);
}

TEST(OSTimeWinTests, givenNoOSInterfaceWhenGetCpuGpuTimeThenReturnsSuccess) {
    TimeStampData CPUGPUTime = {0};
    auto osTime(OSTime::create(nullptr));
    auto success = osTime->getCpuGpuTime(&CPUGPUTime);
    EXPECT_TRUE(success);
    EXPECT_EQ(0u, CPUGPUTime.cpuTimeinNS);
    EXPECT_EQ(0u, CPUGPUTime.gpuTimeStamp);
}

TEST(OSTimeWinTests, givenOSInterfaceWhenGetCpuGpuTimeThenReturnsSuccess) {
    MockExecutionEnvironment executionEnvironment;
    RootDeviceEnvironment rootDeviceEnvironment(executionEnvironment);
    rootDeviceEnvironment.setHwInfoAndInitHelpers(defaultHwInfo.get());
    auto wddm = new WddmMock(rootDeviceEnvironment);
    TimeStampData CPUGPUTime01 = {0};
    TimeStampData CPUGPUTime02 = {0};
    std::unique_ptr<OSInterface> osInterface(new OSInterface());
    osInterface->setDriverModel(std::unique_ptr<DriverModel>(wddm));
    auto osTime = OSTime::create(osInterface.get());
    auto success = osTime->getCpuGpuTime(&CPUGPUTime01);
    EXPECT_TRUE(success);
    EXPECT_NE(0u, CPUGPUTime01.cpuTimeinNS);
    EXPECT_NE(0u, CPUGPUTime01.gpuTimeStamp);
    success = osTime->getCpuGpuTime(&CPUGPUTime02);
    EXPECT_TRUE(success);
    EXPECT_NE(0u, CPUGPUTime02.cpuTimeinNS);
    EXPECT_NE(0u, CPUGPUTime02.gpuTimeStamp);
    EXPECT_GT(CPUGPUTime02.gpuTimeStamp, CPUGPUTime01.gpuTimeStamp);
    EXPECT_GT(CPUGPUTime02.cpuTimeinNS, CPUGPUTime01.cpuTimeinNS);
}
