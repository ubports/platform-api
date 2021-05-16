/*
 * Copyright (C) 2014 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Ricardo Mendoza <ricardo.mendoza@canonical.com>
 */

#include <ubuntu/application/sensors/accelerometer.h>
#include <ubuntu/application/sensors/proximity.h>
#include <ubuntu/application/sensors/light.h>
#include <ubuntu/application/sensors/orientation.h>
#include <ubuntu/application/sensors/gyroscope.h>
#include <ubuntu/application/sensors/magnetic.h>
#include <ubuntu/application/sensors/temperature.h>
#include <ubuntu/application/sensors/pressure.h>

#include <stddef.h>

// Ubuntu Application Sensors. Null desktop implementation

// Acceleration Sensor
UASensorsAccelerometer* ua_sensors_accelerometer_new()
{
    return NULL;
}

UStatus ua_sensors_accelerometer_enable(UASensorsAccelerometer*)
{
    return (UStatus) 0;
}

UStatus ua_sensors_accelerometer_disable(UASensorsAccelerometer*)
{
    return (UStatus) 0;
}

uint32_t ua_sensors_accelerometer_get_min_delay(UASensorsAccelerometer*)
{
    return 0;
}

UStatus ua_sensors_accelerometer_get_min_value(UASensorsAccelerometer*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_accelerometer_get_max_value(UASensorsAccelerometer*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_accelerometer_get_resolution(UASensorsAccelerometer*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_accelerometer_set_event_rate(UASensorsAccelerometer* s, uint32_t rate)
{
    return U_STATUS_SUCCESS;
}

void ua_sensors_accelerometer_set_reading_cb(UASensorsAccelerometer*, on_accelerometer_event_cb, void*)
{
}

// Acceleration Sensor Event
uint64_t uas_accelerometer_event_get_timestamp(UASAccelerometerEvent*)
{
    return 0;
}

UStatus uas_accelerometer_event_get_acceleration_x(UASAccelerometerEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus uas_accelerometer_event_get_acceleration_y(UASAccelerometerEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus uas_accelerometer_event_get_acceleration_z(UASAccelerometerEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

// Proximity Sensor
UASensorsProximity* ua_sensors_proximity_new()
{
    return NULL;
}

UStatus ua_sensors_proximity_enable(UASensorsProximity*)
{
    return (UStatus) 0;
}

UStatus ua_sensors_proximity_disable(UASensorsProximity*)
{
    return (UStatus) 0;
}

uint32_t ua_sensors_proximity_get_min_delay(UASensorsProximity*)
{
    return 0;
}

UStatus ua_sensors_proximity_get_min_value(UASensorsProximity*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_proximity_get_max_value(UASensorsProximity*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_proximity_get_resolution(UASensorsProximity*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_proximity_set_event_rate(UASensorsProximity* s, uint32_t rate)
{
    return U_STATUS_SUCCESS;
}

void ua_sensors_proximity_set_reading_cb(UASensorsProximity*, on_proximity_event_cb, void*)
{
}

// Proximity Sensor Event
uint64_t uas_proximity_event_get_timestamp(UASProximityEvent*)
{
    return 0;
}

UASProximityDistance uas_proximity_event_get_distance(UASProximityEvent*)
{
    return (UASProximityDistance) 0;
}


// Ambient Light Sensor
UASensorsLight* ua_sensors_light_new()
{
    return NULL;
}

UStatus ua_sensors_light_enable(UASensorsLight*)
{
    return (UStatus) 0;
}

UStatus ua_sensors_light_disable(UASensorsLight*)
{
    return (UStatus) 0;
}

uint32_t ua_sensors_light_get_min_delay(UASensorsLight*)
{
    return 0;
}

UStatus ua_sensors_light_get_min_value(UASensorsLight*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_light_get_max_value(UASensorsLight*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_light_get_resolution(UASensorsLight*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_light_set_event_rate(UASensorsLight* s, uint32_t rate)
{
    return U_STATUS_SUCCESS;
}

void ua_sensors_light_set_reading_cb(UASensorsLight*, on_light_event_cb, void*)
{
}

// Ambient Light Sensor Event
uint64_t uas_light_event_get_timestamp(UASLightEvent*)
{
    return 0;
}

UStatus uas_light_event_get_light(UASLightEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

// Orientation Sensor
UASensorsOrientation* ua_sensors_orientation_new()
{
    return NULL;
}

UStatus ua_sensors_orientation_enable(UASensorsOrientation*)
{
    return (UStatus) 0;
}

UStatus ua_sensors_orientation_disable(UASensorsOrientation*)
{
    return (UStatus) 0;
}

uint32_t ua_sensors_orientation_get_min_delay(UASensorsOrientation*)
{
    return 0;
}

UStatus ua_sensors_orientation_get_min_value(UASensorsOrientation*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_orientation_get_max_value(UASensorsOrientation*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_orientation_get_resolution(UASensorsOrientation*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_orientation_set_event_rate(UASensorsOrientation* s, uint32_t rate)
{
    return U_STATUS_SUCCESS;
}

void ua_sensors_orientation_set_reading_cb(UASensorsOrientation*, on_orientation_event_cb, void*)
{
}

// Orientation Sensor Event
uint64_t uas_orientation_event_get_timestamp(UASOrientationEvent*)
{
    return 0;
}

UStatus uas_orientation_event_get_azimuth(UASOrientationEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus uas_orientation_event_get_pitch(UASOrientationEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus uas_orientation_event_get_roll(UASOrientationEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

// Gyroscope Sensor
UASensorsGyroscope* ua_sensors_gyroscope_new()
{
    return NULL;
}

UStatus ua_sensors_gyroscope_enable(UASensorsGyroscope*)
{
    return (UStatus) 0;
}

UStatus ua_sensors_gyroscope_disable(UASensorsGyroscope*)
{
    return (UStatus) 0;
}

uint32_t ua_sensors_gyroscope_get_min_delay(UASensorsGyroscope*)
{
    return 0;
}

UStatus ua_sensors_gyroscope_get_min_value(UASensorsGyroscope*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_gyroscope_get_max_value(UASensorsGyroscope*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_gyroscope_get_resolution(UASensorsGyroscope*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

void ua_sensors_gyroscope_set_reading_cb(UASensorsGyroscope*, on_gyroscope_event_cb, void *)
{
}

UStatus ua_sensors_gyroscope_set_event_rate(UASensorsGyroscope*, uint32_t)
{
    return U_STATUS_SUCCESS;
}

// Gyroscope Sensor Event
uint64_t uas_gyroscope_event_get_timestamp(UASGyroscopeEvent*)
{
    return 0;
}

UStatus uas_gyroscope_event_get_rate_of_rotation_around_x(UASGyroscopeEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus uas_gyroscope_event_get_rate_of_rotation_around_y(UASGyroscopeEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus uas_gyroscope_event_get_rate_of_rotation_around_z(UASGyroscopeEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

// Magnetic Field Sensor
UASensorsMagnetic* ua_sensors_magnetic_new()
{
    return NULL;
}

UStatus ua_sensors_magnetic_enable(UASensorsMagnetic*)
{
    return (UStatus) 0;
}

UStatus ua_sensors_magnetic_disable(UASensorsMagnetic*)
{
    return (UStatus) 0;
}

uint32_t ua_sensors_magnetic_get_min_delay(UASensorsMagnetic*)
{
    return 0;
}

UStatus ua_sensors_magnetic_get_min_value(UASensorsMagnetic*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_magnetic_get_max_value(UASensorsMagnetic*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_magnetic_get_resolution(UASensorsMagnetic*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_magnetic_set_event_rate(UASensorsMagnetic* s, uint32_t rate)
{
    return U_STATUS_SUCCESS;
}

void ua_sensors_magnetic_set_reading_cb(UASensorsMagnetic*, on_magnetic_event_cb, void*)
{
}

// Acceleration Sensor Event
uint64_t uas_magnetic_event_get_timestamp(UASMagneticEvent*)
{
    return 0;
}

UStatus uas_magnetic_event_get_magnetic_field_x(UASMagneticEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus uas_magnetic_event_get_magnetic_field_y(UASMagneticEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus uas_magnetic_event_get_magnetic_field_z(UASMagneticEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

// Temperature Sensor
UASensorsTemperature* ua_sensors_temperature_new()
{
    return NULL;
}

UStatus ua_sensors_temperature_enable(UASensorsTemperature*)
{
    return (UStatus) 0;
}

UStatus ua_sensors_temperature_disable(UASensorsTemperature*)
{
    return (UStatus) 0;
}

uint32_t ua_sensors_temperature_get_min_delay(UASensorsTemperature*)
{
    return 0;
}

UStatus ua_sensors_temperature_get_min_value(UASensorsTemperature*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_temperature_get_max_value(UASensorsTemperature*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_temperature_get_resolution(UASensorsTemperature*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

void ua_sensors_temperature_set_reading_cb(UASensorsTemperature*, on_temperature_event_cb, void *)
{
}

UStatus ua_sensors_temperature_set_event_rate(UASensorsTemperature*, uint32_t)
{
    return U_STATUS_SUCCESS;
}

// Temperature Sensor Event
uint64_t uas_temperature_event_get_timestamp(UASTemperatureEvent*)
{
    return 0;
}

UStatus uas_temperature_event_get_temperature(UASTemperatureEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

// Pressure Sensor
UASensorsPressure* ua_sensors_pressure_new()
{
    return NULL;
}

UStatus ua_sensors_pressure_enable(UASensorsPressure*)
{
    return (UStatus) 0;
}

UStatus ua_sensors_pressure_disable(UASensorsPressure*)
{
    return (UStatus) 0;
}

uint32_t ua_sensors_pressure_get_min_delay(UASensorsPressure*)
{
    return 0;
}

UStatus ua_sensors_pressure_get_min_value(UASensorsPressure*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_pressure_get_max_value(UASensorsPressure*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

UStatus ua_sensors_pressure_get_resolution(UASensorsPressure*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}

void ua_sensors_pressure_set_reading_cb(UASensorsPressure*, on_pressure_event_cb, void *)
{
}

UStatus ua_sensors_pressure_set_event_rate(UASensorsPressure*, uint32_t)
{
    return U_STATUS_SUCCESS;
}

// Pressure Sensor Event
uint64_t uas_pressure_event_get_timestamp(UASPressureEvent*)
{
    return 0;
}

UStatus uas_pressure_event_get_pressure(UASPressureEvent*, float* value)
{
    if (!value)
        return U_STATUS_ERROR;

    *value = 0.f;

    return U_STATUS_SUCCESS;
}


