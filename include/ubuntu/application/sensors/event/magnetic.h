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
 */

#ifndef UBUNTU_APPLICATION_SENSORS_MAGNETIC_EVENT_H_
#define UBUNTU_APPLICATION_SENSORS_MAGNETIC_EVENT_H_

#include <ubuntu/visibility.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * \brief Opaque type describing a magnetic field reading.
     * \ingroup sensor_access
     */
    typedef void UASMagneticEvent;

    /**
     * \brief Query the timestamp of the sensor reading.
     * \ingroup sensor_access
     * \returns The timestamp of the sensor reading in [µs], timebase: monotonic clock.
     * \param[in] event The reading to be queried.
     */
    UBUNTU_DLL_PUBLIC uint64_t
    uas_magnetic_event_get_timestamp(
        UASMagneticEvent* event);

    /**
     * \brief Query the intensity of the magnetic field in the x-axis.
     * \ingroup sensor_access
     * \returns The intensity of the magnetic field in the x-axis in micro-Tesla (uT)
     * \param[in] event The reading to be queried.
     */
    UBUNTU_DLL_PUBLIC UStatus
    uas_magnetic_event_get_magnetic_field_x(
        UASMagneticEvent* event,
        float* value);

    /**
     * \brief Query the intensity of the magnetic field in the x-axis.
     * \ingroup sensor_access
     * \returns The intensity of the magnetic field in the x-axis in micro-Tesla (uT)
     * \param[in] event The reading to be queried.
     */
    UBUNTU_DLL_PUBLIC UStatus
    uas_magnetic_event_get_magnetic_field_y(
        UASMagneticEvent* event,
        float* value);

    /**
     * \brief Query the intensity of the magnetic field in the x-axis.
     * \ingroup sensor_access
     * \returns The intensity of the magnetic field in the x-axis in micro-Tesla (uT)
     * \param[in] event The reading to be queried.
     */
    UBUNTU_DLL_PUBLIC UStatus
    uas_magnetic_event_get_magnetic_field_z(
        UASMagneticEvent* event,
        float* value);

#ifdef __cplusplus
}
#endif

#endif // UBUNTU_APPLICATION_SENSORS_MAGNETIC_EVENT_H_
