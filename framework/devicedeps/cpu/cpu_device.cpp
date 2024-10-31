/*
 * Copyright 2024 Intel Corporation.
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef SANDSTONE_DEVICE_CPU
#include "devicedeps/cpu/cpu_features.h"
#include "devicedeps/devices.h"
#include "devicedeps/cpu/cpu_device.h"
#include "devicedeps/cpu/topology.h"

#include <stdio.h>
#include <stdbool.h>
#include <string>
#include <inttypes.h>

/// Device specific APIs for CPU devices.
/// Declared in cpu_device.h and used by CPU specific code because it has
/// semantic meaning for CPU devices.

struct cpu_info *cpu_info = nullptr;

#ifdef __llvm__
thread_local int thread_num __attribute__((tls_model("initial-exec")));
#else
thread_local int thread_num = 0;
#endif

uint64_t CpuDevice::features = _compilerCpuFeatures;

bool CpuDevice::has_feature(uint64_t f) {
    return ((_compilerCpuFeatures & (f)) == (f) || (CpuDevice::features & (f)) == (f));
}

std::string CpuDevice::features_to_string(uint64_t f)
{
    std::string result;
    const char *comma = "";
    for (size_t i = 0; i < std::size(x86_locators); ++i) {
        if (f & (UINT64_C(1) << i)) {
            result += comma;
            result += features_string + features_indices[i] + 1;
            comma = ",";
        }
    }
    return result;
}

void dump_cpu_info(int verbosity)
{
    int i;

    // find the best matching CPU
    const char *detected = "<unknown>";
    for (const auto &arch : x86_architectures) {
        if ((arch.features & CpuDevice::features) == arch.features) {
            detected = arch.name;
            break;
        }
        if (verbosity > 1)
            printf("CPU is not %s: missing %s\n", arch.name,
                  CpuDevice::features_to_string(arch.features & ~CpuDevice::features).c_str());
    }
    printf("Detected CPU: %s; family-model-stepping (hex): %02x-%02x-%02x; CPU features: %s\n",
           detected, cpu_info[0].family, cpu_info[0].model, cpu_info[0].stepping,
          CpuDevice::features_to_string(CpuDevice::features).c_str());
    printf("# CPU\tPkgID\tCoreID\tThrdID\tMicrocode\tPPIN\n");
    for (i = 0; i < num_cpus(); ++i) {
        printf("%d\t%d\t%d\t%d\t0x%" PRIx64, cpu_info[i].cpu_number,
               cpu_info[i].package_id, cpu_info[i].core_id, cpu_info[i].thread_id,
               cpu_info[i].microcode);
        if (cpu_info[i].ppin)
            printf("\t%016" PRIx64, cpu_info[i].ppin);
        puts("");
    }
}

/// Implementation of device.h API for the framework.
int num_devices() {
    return num_cpus();
}
int num_units() {
    return num_cpus();
}
bool device_has_feature(unsigned long f) {
    return CpuDevice::has_feature(f);
}
/// CPU devices are directly initialized in the framework's main() function.
/// Once CPU initialization is untied from shmem initialization, this function
/// will be called from the framework's main() function.
__attribute__((unused)) void device_init() {
    return;
}
__attribute__((unused, noinline)) void device_specific_init() {}

void restrict_devices(DeviceRange range) {
    restrict_topology(range);
}

#endif
