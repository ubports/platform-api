/*
 * Copyright (C) 2015 Canonical, Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3, as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranties of MERCHANTABILITY,
 * SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Gerry Boland <gerry.boland@canonical.com>
 */

#ifndef UBUNTU_APPLICATION_SENSORS_GYROSCOPE_EVENT_H_
#define UBUNTU_APPLICATION_SENSORS_GYROSCOPE_EVENT_H_

#include <ubuntu/visibility.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * \brief Opaque type describing a gyroscope reading.
     * \ingroup sensor_access
     */
    typedef void UASGyroscopeEvent;

    /**
     * \brief Query the timestamp of the sensor reading.
     * \ingroup sensor_access
     * \returns The timestamp of the sensor reading in [µs], timebase: monotonic clock.
     * \param[in] event The reading to be queried.
     */
    UBUNTU_DLL_PUBLIC uint64_t
    uas_gyroscope_event_get_timestamp(
        UASGyroscopeEvent* event);

    /**
     * \brief Query the rate of rotation around the x-axis.
     * \ingroup sensor_access
     * \returns The rate of rotation around the x-axis in radians per second.
     *          Positive values indicate counter-clockwise rotation
     * \param[in] event The reading to be queried.
     */
    UBUNTU_DLL_PUBLIC UStatus
    uas_gyroscope_event_get_rate_of_rotation_around_x(
        UASGyroscopeEvent* event,
        float* value);

    /**
     * \brief Query the rate of rotation around the y-axis.
     * \ingroup sensor_access
     * \returns The rate of rotation around the y-axis in radians per second.
     *          Positive values indicate counter-clockwise rotation
     * \param[in] event The reading to be queried.
     */
    UBUNTU_DLL_PUBLIC UStatus
    uas_gyroscope_event_get_rate_of_rotation_around_y(
        UASGyroscopeEvent* event,
        float* value);

    /**
     * \brief Query the rate of rotation around the z-axis.
     * \ingroup sensor_access
     * \returns The rate of rotation around the z-axis in radians per second.
     *          Positive values indicate counter-clockwise rotation
     * \param[in] event The reading to be queried.
     */
    UBUNTU_DLL_PUBLIC UStatus
    uas_gyroscope_event_get_rate_of_rotation_around_z(
        UASGyroscopeEvent* event,
        float* value);

#ifdef __cplusplus
}
#endif

#endif // UBUNTU_APPLICATION_SENSORS_GYROSCOPE_EVENT_H_
