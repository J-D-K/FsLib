#pragma once
#include "Path.hpp"

namespace fslib
{
    /**
     * @brief Attempts to get the free space available on Device passed.
     *
     * @param deviceRoot Root of device.
     * @param sizeOut The size retrieved if successful.
     * @return Size on success. -1 on failure.
     * @note This function requires a path to work. DeviceRoot should be `sdmc:/` instead of `sdmc`, for example.
     */
    int64_t get_device_free_space(const fslib::Path &deviceRoot);

    /**
     * @brief Attempts to get the total space of Device passed.
     *
     * @param deviceRoot Root of device.
     * @param sizeOut The size retrieved if successful.
     * @return Size on success. -1 on failure.
     * @note This function requires a path to work. DeviceRoot should be `sdmc:/` instead of `sdmc`, for example.
     */
    int64_t get_device_total_space(const fslib::Path &deviceRoot);
}