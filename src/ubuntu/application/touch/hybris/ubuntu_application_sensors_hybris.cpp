/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Authored by: Thomas Voss <thomas.voss@canonical.com>
 *              Ricardo Mendoza <ricardo.mendoza@canonical.com>
 */

#include <ubuntu/application/sensors/accelerometer.h>
#include <ubuntu/application/sensors/proximity.h>
#include <ubuntu/application/sensors/light.h>
#include <ubuntu/application/sensors/orientation.h>
#include <ubuntu/application/sensors/gyroscope.h>
#include <ubuntu/application/sensors/magnetic.h>
#include <ubuntu/application/sensors/temperature.h>
#include <ubuntu/application/sensors/pressure.h>

#include "hybris_module.h"

// Ubuntu Application Sensors

// Acceleration Sensor
IMPLEMENT_CTOR0(UASensorsAccelerometer*, ua_sensors_accelerometer_new);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_accelerometer_enable, UASensorsAccelerometer*);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_accelerometer_disable, UASensorsAccelerometer*);
IMPLEMENT_FUNCTION1(uint32_t, ua_sensors_accelerometer_get_min_delay, UASensorsAccelerometer*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_accelerometer_get_min_value, UASensorsAccelerometer*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_accelerometer_get_max_value, UASensorsAccelerometer*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_accelerometer_get_resolution, UASensorsAccelerometer*, float*);
IMPLEMENT_VOID_FUNCTION3(ua_sensors_accelerometer_set_reading_cb, UASensorsAccelerometer*, on_accelerometer_event_cb, void*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_accelerometer_set_event_rate, UASensorsAccelerometer*, uint32_t);

// Acceleration Sensor Event
IMPLEMENT_FUNCTION1(uint64_t, uas_accelerometer_event_get_timestamp, UASAccelerometerEvent*);
IMPLEMENT_FUNCTION2(UStatus, uas_accelerometer_event_get_acceleration_x, UASAccelerometerEvent*, float*);
IMPLEMENT_FUNCTION2(UStatus, uas_accelerometer_event_get_acceleration_y, UASAccelerometerEvent*, float*);
IMPLEMENT_FUNCTION2(UStatus, uas_accelerometer_event_get_acceleration_z, UASAccelerometerEvent*, float*);

// Proximity Sensor
IMPLEMENT_CTOR0(UASensorsProximity*, ua_sensors_proximity_new);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_proximity_enable, UASensorsProximity*);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_proximity_disable, UASensorsProximity*);
IMPLEMENT_FUNCTION1(uint32_t, ua_sensors_proximity_get_min_delay, UASensorsProximity*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_proximity_get_min_value, UASensorsProximity*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_proximity_get_max_value, UASensorsProximity*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_proximity_get_resolution, UASensorsProximity*, float*);
IMPLEMENT_VOID_FUNCTION3(ua_sensors_proximity_set_reading_cb, UASensorsProximity*, on_proximity_event_cb, void*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_proximity_set_event_rate, UASensorsProximity*, uint32_t);

// Proximity Sensor Event
IMPLEMENT_FUNCTION1(uint64_t, uas_proximity_event_get_timestamp, UASProximityEvent*);
IMPLEMENT_FUNCTION1(UASProximityDistance, uas_proximity_event_get_distance, UASProximityEvent*);

// Ambient Light Sensor
IMPLEMENT_CTOR0(UASensorsLight*, ua_sensors_light_new);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_light_enable, UASensorsLight*);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_light_disable, UASensorsLight*);
IMPLEMENT_FUNCTION1(uint32_t, ua_sensors_light_get_min_delay, UASensorsLight*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_light_get_min_value, UASensorsLight*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_light_get_max_value, UASensorsLight*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_light_get_resolution, UASensorsLight*, float*);
IMPLEMENT_VOID_FUNCTION3(ua_sensors_light_set_reading_cb, UASensorsLight*, on_light_event_cb, void*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_light_set_event_rate, UASensorsLight*, uint32_t);

// Ambient Light Sensor Event
IMPLEMENT_FUNCTION1(uint64_t, uas_light_event_get_timestamp, UASLightEvent*);
IMPLEMENT_FUNCTION2(UStatus, uas_light_event_get_light, UASLightEvent*, float*);

// Orientation Sensor
IMPLEMENT_CTOR0(UASensorsOrientation*, ua_sensors_orientation_new);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_orientation_enable, UASensorsOrientation*);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_orientation_disable, UASensorsOrientation*);
IMPLEMENT_FUNCTION1(uint32_t, ua_sensors_orientation_get_min_delay, UASensorsOrientation*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_orientation_get_min_value, UASensorsOrientation*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_orientation_get_max_value, UASensorsOrientation*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_orientation_get_resolution, UASensorsOrientation*, float*);
IMPLEMENT_VOID_FUNCTION3(ua_sensors_orientation_set_reading_cb, UASensorsOrientation*, on_orientation_event_cb, void*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_orientation_set_event_rate, UASensorsOrientation*, uint32_t);

// Orientation Sensor Event
IMPLEMENT_FUNCTION1(uint64_t, uas_orientation_event_get_timestamp, UASOrientationEvent*);
IMPLEMENT_FUNCTION2(UStatus, uas_orientation_event_get_azimuth, UASOrientationEvent*, float*);
IMPLEMENT_FUNCTION2(UStatus, uas_orientation_event_get_pitch, UASOrientationEvent*, float*);
IMPLEMENT_FUNCTION2(UStatus, uas_orientation_event_get_roll, UASOrientationEvent*, float*);

// Gyroscope Sensor Event
IMPLEMENT_CTOR0(UASensorsGyroscope*, ua_sensors_gyroscope_new);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_gyroscope_enable, UASensorsGyroscope*);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_gyroscope_disable, UASensorsGyroscope*);
IMPLEMENT_FUNCTION1(uint32_t, ua_sensors_gyroscope_get_min_delay, UASensorsGyroscope*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_gyroscope_get_min_value, UASensorsGyroscope*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_gyroscope_get_max_value, UASensorsGyroscope*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_gyroscope_get_resolution, UASensorsGyroscope*, float*);
IMPLEMENT_VOID_FUNCTION3(ua_sensors_gyroscope_set_reading_cb, UASensorsGyroscope*, on_gyroscope_event_cb, void*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_gyroscope_set_event_rate, UASensorsGyroscope*, uint32_t);

// Gyroscope Sensor Event
IMPLEMENT_FUNCTION1(uint64_t, uas_gyroscope_event_get_timestamp, UASGyroscopeEvent*);
IMPLEMENT_FUNCTION2(UStatus, uas_gyroscope_event_get_rate_of_rotation_around_x, UASGyroscopeEvent*, float*);
IMPLEMENT_FUNCTION2(UStatus, uas_gyroscope_event_get_rate_of_rotation_around_y, UASGyroscopeEvent*, float*);
IMPLEMENT_FUNCTION2(UStatus, uas_gyroscope_event_get_rate_of_rotation_around_z, UASGyroscopeEvent*, float*);

// Magnetic Field Sensor
IMPLEMENT_CTOR0(UASensorsMagnetic*, ua_sensors_magnetic_new);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_magnetic_enable, UASensorsMagnetic*);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_magnetic_disable, UASensorsMagnetic*);
IMPLEMENT_FUNCTION1(uint32_t, ua_sensors_magnetic_get_min_delay, UASensorsMagnetic*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_magnetic_get_min_value, UASensorsMagnetic*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_magnetic_get_max_value, UASensorsMagnetic*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_magnetic_get_resolution, UASensorsMagnetic*, float*);
IMPLEMENT_VOID_FUNCTION3(ua_sensors_magnetic_set_reading_cb, UASensorsMagnetic*, on_magnetic_event_cb, void*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_magnetic_set_event_rate, UASensorsMagnetic*, uint32_t);

// Magnetic Field Sensor Event
IMPLEMENT_FUNCTION1(uint64_t, uas_magnetic_event_get_timestamp, UASMagneticEvent*);
IMPLEMENT_FUNCTION2(UStatus, uas_magnetic_event_get_magnetic_field_x, UASMagneticEvent*, float*);
IMPLEMENT_FUNCTION2(UStatus, uas_magnetic_event_get_magnetic_field_y, UASMagneticEvent*, float*);
IMPLEMENT_FUNCTION2(UStatus, uas_magnetic_event_get_magnetic_field_z, UASMagneticEvent*, float*);

// Ambient Temperature Sensor
IMPLEMENT_CTOR0(UASensorsTemperature*, ua_sensors_temperature_new);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_temperature_enable, UASensorsTemperature*);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_temperature_disable, UASensorsTemperature*);
IMPLEMENT_FUNCTION1(uint32_t, ua_sensors_temperature_get_min_delay, UASensorsTemperature*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_temperature_get_min_value, UASensorsTemperature*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_temperature_get_max_value, UASensorsTemperature*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_temperature_get_resolution, UASensorsTemperature*, float*);
IMPLEMENT_VOID_FUNCTION3(ua_sensors_temperature_set_reading_cb, UASensorsTemperature*, on_temperature_event_cb, void*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_temperature_set_event_rate, UASensorsTemperature*, uint32_t);

// Ambient Temperature Sensor Event
IMPLEMENT_FUNCTION1(uint64_t, uas_temperature_event_get_timestamp, UASTemperatureEvent*);
IMPLEMENT_FUNCTION2(UStatus, uas_temperature_event_get_temperature, UASTemperatureEvent*, float*);

// Ambient Pressure Sensor
IMPLEMENT_CTOR0(UASensorsPressure*, ua_sensors_pressure_new);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_pressure_enable, UASensorsPressure*);
IMPLEMENT_FUNCTION1(UStatus, ua_sensors_pressure_disable, UASensorsPressure*);
IMPLEMENT_FUNCTION1(uint32_t, ua_sensors_pressure_get_min_delay, UASensorsPressure*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_pressure_get_min_value, UASensorsPressure*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_pressure_get_max_value, UASensorsPressure*, float*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_pressure_get_resolution, UASensorsPressure*, float*);
IMPLEMENT_VOID_FUNCTION3(ua_sensors_pressure_set_reading_cb, UASensorsPressure*, on_pressure_event_cb, void*);
IMPLEMENT_FUNCTION2(UStatus, ua_sensors_pressure_set_event_rate, UASensorsPressure*, uint32_t);

// Ambient Pressure Sensor Event
IMPLEMENT_FUNCTION1(uint64_t, uas_pressure_event_get_timestamp, UASPressureEvent*);
IMPLEMENT_FUNCTION2(UStatus, uas_pressure_event_get_pressure, UASPressureEvent*, float*);

