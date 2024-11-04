/*
 * Copyright 2024 Intel Corporation.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef INC_DEVICES_H
#define INC_DEVICES_H

#include <stdbool.h>

#define SANDSTONE_NO_DEVICE (-1)

#ifdef __cplusplus
// Base device type
class DeviceBase {
public:
    DeviceBase(int index) : index{index} {}
    virtual ~DeviceBase() {}
    int index;
};

/// @brief Generic device discovery and initialization function. Need to be
/// implemented by the device-specific code. The function is called from the
/// framework's main() function.
extern DeviceBase* device_init();

/// @brief Called from sandstone_main(). The default implementation performs no
/// checks, they just return. Feel free to implement a strong version elsewhere
/// if you prefer the framework to check for additional system or device criteria.
extern template<typename... Args>
void device_specific_init(Args... args);

extern "C"
{
#endif
    /// C-style device API functions that need to be implemented by 
    /// the device-specific code.

    /// @brief Represents a contiguous range of devices.
    typedef struct {
        int starting_device;
        int device_count;
    } DeviceRange;

    /// @brief Returns the number of physical instances of a device available for use
    /// by tests (e.g., number of CPU packages, number of compute accelerator
    /// devices, etc.).
    extern int num_devices() __attribute__((pure));

    /// Returns the number of logical compute execution units (e.g., CPU cores)
    /// available to a test. Normally, this value is equal to the total number of
    /// execution units in the device under test but the value can be lower
    /// if --cpuset option is used, the tests specifies a value for test.max_threads
    /// or the OS/other software restricts the number of visible devices.
    extern int num_units() __attribute__((pure));

    /// @brief Restricts the devices available to a test.
    void restrict_devices(DeviceRange range);

#ifdef __cplusplus
}
#endif
#endif // INC_DEVICES_H
