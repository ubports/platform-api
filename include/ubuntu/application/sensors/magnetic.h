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

#ifndef UBUNTU_APPLICATION_SENSORS_MAGNETIC_H_
#define UBUNTU_APPLICATION_SENSORS_MAGNETIC_H_

#include <ubuntu/status.h>
#include <ubuntu/visibility.h>

#include <ubuntu/application/sensors/event/magnetic.h>

#ifdef __cplusplus
extern "C" {
#endif

    /**
     * \brief Opaque type that models the Magnetic field.
     * \ingroup sensor_access
     */
    typedef void UASensorsMagnetic;

    /**
     * \brief Callback type used by applications to subscribe to magnetic sensor events.
     * \ingroup sensor_access
     */
    typedef void (*on_magnetic_event_cb)(UASMagneticEvent* event,
                                         void* context);

    /**
     * \brief Create a new object for accessing the magnetic field sensor.
     * \ingroup sensor_access
     * \returns A new instance or NULL in case of errors.
     */
    UBUNTU_DLL_PUBLIC UASensorsMagnetic*
    ua_sensors_magnetic_new();

    /**
     * \brief Enables the supplied magnetic field sensor.
     * \ingroup sensor_access
     * \returns U_STATUS_SUCCESS if successful or U_STATUS_ERROR if an error occured.
     * \param[in] sensor The sensor instance to be enabled.
     */
    UBUNTU_DLL_PUBLIC UStatus
    ua_sensors_magnetic_enable(
        UASensorsMagnetic* sensor);

    /**
     * \brief Disables the supplied magnetic field sensor.
     * \ingroup sensor_access
     * \returns U_STATUS_SUCCESS if successful or U_STATUS_ERROR if an error occured.
     * \param[in] sensor The sensor instance to be disabled.
     */
    UBUNTU_DLL_PUBLIC UStatus
    ua_sensors_magnetic_disable(
        UASensorsMagnetic* sensor);

    /**
     * \brief Queries the minimum delay between two readings for the supplied sensor.
     * \ingroup sensor_access
     * \returns The minimum delay between two readings in [ms].
     * \param[in] sensor The sensor instance to be queried.
     */
    UBUNTU_DLL_PUBLIC uint32_t
    ua_sensors_magnetic_get_min_delay(
        UASensorsMagnetic* sensor);

    /**
     * \brief Queries the minimum value that can be reported by the sensor.
     * \ingroup sensor_access
     * \returns The minimum value that can be reported by the sensor.
     * \param[in] sensor The sensor instance to be queried.
     */
    UBUNTU_DLL_PUBLIC UStatus
    ua_sensors_magnetic_get_min_value(
        UASensorsMagnetic* sensor,
        float* value);

    /**
     * \brief Queries the maximum value that can be reported by the sensor.
     * \ingroup sensor_access
     * \returns The maximum value that can be reported by the sensor.
     * \param[in] sensor The sensor instance to be queried.
     */
    UBUNTU_DLL_PUBLIC UStatus
    ua_sensors_magnetic_get_max_value(
        UASensorsMagnetic* sensor,
        float* value);

    /**
     * \brief Queries the numeric resolution supported by the sensor
     * \ingroup sensor_access
     * \returns The numeric resolution supported by the sensor.
     * \param[in] sensor The sensor instance to be queried.
     */
    UBUNTU_DLL_PUBLIC UStatus
    ua_sensors_magnetic_get_resolution(
        UASensorsMagnetic* sensor,
        float* value);

    /**
     * \brief Set the callback to be invoked whenever a new sensor reading is available.
     * \ingroup sensor_access
     * \param[in] sensor The sensor instance to associate the callback with.
     * \param[in] cb The callback to be invoked.
     * \param[in] ctx The context supplied to the callback invocation.
     */
    UBUNTU_DLL_PUBLIC void
    ua_sensors_magnetic_set_reading_cb(
        UASensorsMagnetic* sensor,
        on_magnetic_event_cb cb,
        void *ctx);

    /**
     * \brief Set the sensor event delivery rate in nanoseconds..
     * \ingroup sensor_access
     * \returns U_STATUS_SUCCESS if successful or U_STATUS_ERROR if an error occured.
     * \param[in] sensor The sensor instance to be modified.
     * \param[in] rate The new event delivery rate.
     */
    UBUNTU_DLL_PUBLIC UStatus
    ua_sensors_magnetic_set_event_rate(
        UASensorsMagnetic* sensor,
        uint32_t rate);

#ifdef __cplusplus
}
#endif

#endif /* UBUNTU_APPLICATION_SENSORS_MAGNETIC_H_ */
