/*
 * Copyright (C) 2014-2015 Canonical Ltd
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

// C APIs
#include <ubuntu/application/id.h>
#include <ubuntu/application/description.h>
#include <ubuntu/application/instance.h>
#include <ubuntu/application/options.h>
#include <ubuntu/application/lifecycle_delegate.h>
#include <ubuntu/application/sensors/accelerometer.h>
#include <ubuntu/application/sensors/proximity.h>
#include <ubuntu/application/sensors/light.h>
#include <ubuntu/application/sensors/orientation.h>
#include <ubuntu/application/sensors/haptic.h>
#include <ubuntu/application/sensors/gyroscope.h>
#include <ubuntu/application/sensors/magnetic.h>
#include <ubuntu/application/sensors/temperature.h>
#include <ubuntu/application/sensors/pressure.h>

#include <ubuntu/application/location/service.h>
#include <ubuntu/application/location/heading_update.h>
#include <ubuntu/application/location/position_update.h>
#include <ubuntu/application/location/velocity_update.h>

#include <ubuntu/application/url_dispatcher/service.h>

#include <ubuntu/application/init.h>

#include "base_module.h"

#ifdef __cplusplus
extern "C" {
#endif

// Application Module Config
IMPLEMENT_VOID_FUNCTION3(init, u_application_module_version, uint32_t*, uint32_t*, uint32_t*);
IMPLEMENT_VOID_FUNCTION1(init, u_application_init, void*);
IMPLEMENT_VOID_FUNCTION0(init, u_application_finish);

// Lifecycle helpers
IMPLEMENT_CTOR0(lifecycle, UApplicationLifecycleDelegate*, u_application_lifecycle_delegate_new);
IMPLEMENT_VOID_FUNCTION2(lifecycle, u_application_lifecycle_delegate_set_context, UApplicationLifecycleDelegate*, void*);
IMPLEMENT_VOID_FUNCTION1(lifecycle, u_application_lifecycle_delegate_ref, UApplicationLifecycleDelegate*);
IMPLEMENT_VOID_FUNCTION1(lifecycle, u_application_lifecycle_delegate_unref, UApplicationLifecycleDelegate*);
IMPLEMENT_VOID_FUNCTION2(lifecycle, u_application_lifecycle_delegate_set_application_resumed_cb, UApplicationLifecycleDelegate*, u_on_application_resumed);
IMPLEMENT_VOID_FUNCTION2(lifecycle, u_application_lifecycle_delegate_set_application_about_to_stop_cb, UApplicationLifecycleDelegate*, u_on_application_about_to_stop);

// Application Instance Helpers

// UApplicationId
IMPLEMENT_FUNCTION2(instance, UApplicationId*, u_application_id_new_from_stringn, const char*, size_t);
IMPLEMENT_VOID_FUNCTION1(instance, u_application_id_destroy, UApplicationId*);
IMPLEMENT_FUNCTION2(instance, int, u_application_id_compare, UApplicationId*, UApplicationId*);

// UApplicationDescription
IMPLEMENT_FUNCTION0(instance, UApplicationDescription*, u_application_description_new);
IMPLEMENT_VOID_FUNCTION1(instance, u_application_description_destroy, UApplicationDescription*);
IMPLEMENT_VOID_FUNCTION2(instance, u_application_description_set_application_id, UApplicationDescription*, UApplicationId*);
IMPLEMENT_VOID_FUNCTION2(instance, u_application_description_set_application_lifecycle_delegate, UApplicationDescription*, UApplicationLifecycleDelegate*);

// UApplicationOptions
IMPLEMENT_FUNCTION2(instance, UApplicationOptions*, u_application_options_new_from_cmd_line, int, char**);
IMPLEMENT_VOID_FUNCTION1(instance, u_application_options_destroy, UApplicationOptions*);

// UApplicationInstance
IMPLEMENT_FUNCTION2(instance, UApplicationInstance*, u_application_instance_new_from_description_with_options, UApplicationDescription*, UApplicationOptions*);
IMPLEMENT_FUNCTION1(connection, MirConnection*, u_application_instance_get_mir_connection, UApplicationInstance*);

// Ubuntu Application Sensors

// Acceleration Sensor
IMPLEMENT_CTOR0(sensors, UASensorsAccelerometer*, ua_sensors_accelerometer_new);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_accelerometer_enable, UASensorsAccelerometer*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_accelerometer_disable, UASensorsAccelerometer*);
IMPLEMENT_FUNCTION1(sensors, uint32_t, ua_sensors_accelerometer_get_min_delay, UASensorsAccelerometer*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_accelerometer_get_min_value, UASensorsAccelerometer*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_accelerometer_get_max_value, UASensorsAccelerometer*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_accelerometer_get_resolution, UASensorsAccelerometer*, float*);
IMPLEMENT_VOID_FUNCTION3(sensors, ua_sensors_accelerometer_set_reading_cb, UASensorsAccelerometer*, on_accelerometer_event_cb, void*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_accelerometer_set_event_rate, UASensorsAccelerometer*, uint32_t);

// Acceleration Sensor Event
IMPLEMENT_FUNCTION1(sensors, uint64_t, uas_accelerometer_event_get_timestamp, UASAccelerometerEvent*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_accelerometer_event_get_acceleration_x, UASAccelerometerEvent*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_accelerometer_event_get_acceleration_y, UASAccelerometerEvent*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_accelerometer_event_get_acceleration_z, UASAccelerometerEvent*, float*);

// Proximity Sensor
IMPLEMENT_CTOR0(sensors, UASensorsProximity*, ua_sensors_proximity_new);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_proximity_enable, UASensorsProximity*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_proximity_disable, UASensorsProximity*);
IMPLEMENT_FUNCTION1(sensors, uint32_t, ua_sensors_proximity_get_min_delay, UASensorsProximity*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_proximity_get_min_value, UASensorsProximity*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_proximity_get_max_value, UASensorsProximity*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_proximity_get_resolution, UASensorsProximity*, float*);
IMPLEMENT_VOID_FUNCTION3(sensors, ua_sensors_proximity_set_reading_cb, UASensorsProximity*, on_proximity_event_cb, void*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_proximity_set_event_rate, UASensorsProximity*, uint32_t);

// Proximity Sensor Event
IMPLEMENT_FUNCTION1(sensors, uint64_t, uas_proximity_event_get_timestamp, UASProximityEvent*);
IMPLEMENT_FUNCTION1(sensors, UASProximityDistance, uas_proximity_event_get_distance, UASProximityEvent*);

// Ambient Light Sensor
IMPLEMENT_CTOR0(sensors, UASensorsLight*, ua_sensors_light_new);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_light_enable, UASensorsLight*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_light_disable, UASensorsLight*);
IMPLEMENT_FUNCTION1(sensors, uint32_t, ua_sensors_light_get_min_delay, UASensorsLight*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_light_get_min_value, UASensorsLight*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_light_get_max_value, UASensorsLight*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_light_get_resolution, UASensorsLight*, float*);
IMPLEMENT_VOID_FUNCTION3(sensors, ua_sensors_light_set_reading_cb, UASensorsLight*, on_light_event_cb, void*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_light_set_event_rate, UASensorsLight*, uint32_t);

// Ambient Light Sensor Event
IMPLEMENT_FUNCTION1(sensors, uint64_t, uas_light_event_get_timestamp, UASLightEvent*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_light_event_get_light, UASLightEvent*, float*);

// Haptic Sensor
IMPLEMENT_CTOR0(sensors, UASensorsHaptic*, ua_sensors_haptic_new);
IMPLEMENT_VOID_FUNCTION1(sensors, ua_sensors_haptic_destroy, UASensorsHaptic*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_haptic_enable, UASensorsHaptic*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_haptic_disable, UASensorsHaptic*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_haptic_vibrate_once, UASensorsHaptic*, uint32_t);
IMPLEMENT_FUNCTION3(sensors, UStatus, ua_sensors_haptic_vibrate_with_pattern, UASensorsHaptic*, uint32_t*, uint32_t);

// Orientation Sensor
IMPLEMENT_CTOR0(sensors, UASensorsOrientation*, ua_sensors_orientation_new);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_orientation_enable, UASensorsOrientation*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_orientation_disable, UASensorsOrientation*);
IMPLEMENT_FUNCTION1(sensors, uint32_t, ua_sensors_orientation_get_min_delay, UASensorsOrientation*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_orientation_get_min_value, UASensorsOrientation*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_orientation_get_max_value, UASensorsOrientation*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_orientation_get_resolution, UASensorsOrientation*, float*);
IMPLEMENT_VOID_FUNCTION3(sensors, ua_sensors_orientation_set_reading_cb, UASensorsOrientation*, on_orientation_event_cb, void*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_orientation_set_event_rate, UASensorsOrientation*, uint32_t);

// Orientation Sensor Event
IMPLEMENT_FUNCTION1(sensors, uint64_t, uas_orientation_event_get_timestamp, UASOrientationEvent*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_orientation_event_get_azimuth, UASOrientationEvent*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_orientation_event_get_pitch, UASOrientationEvent*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_orientation_event_get_roll, UASOrientationEvent*, float*);

// Gyroscope Sensor Event
IMPLEMENT_CTOR0(sensors, UASensorsGyroscope*, ua_sensors_gyroscope_new);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_gyroscope_enable, UASensorsGyroscope*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_gyroscope_disable, UASensorsGyroscope*);
IMPLEMENT_FUNCTION1(sensors, uint32_t, ua_sensors_gyroscope_get_min_delay, UASensorsGyroscope*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_gyroscope_get_min_value, UASensorsGyroscope*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_gyroscope_get_max_value, UASensorsGyroscope*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_gyroscope_get_resolution, UASensorsGyroscope*, float*);
IMPLEMENT_VOID_FUNCTION3(sensors, ua_sensors_gyroscope_set_reading_cb, UASensorsGyroscope*, on_gyroscope_event_cb, void*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_gyroscope_set_event_rate, UASensorsGyroscope*, uint32_t);

// Gyroscope Sensor Event
IMPLEMENT_FUNCTION1(sensors, uint64_t, uas_gyroscope_event_get_timestamp, UASGyroscopeEvent*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_gyroscope_event_get_rate_of_rotation_around_x, UASGyroscopeEvent*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_gyroscope_event_get_rate_of_rotation_around_y, UASGyroscopeEvent*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_gyroscope_event_get_rate_of_rotation_around_z, UASGyroscopeEvent*, float*);

// Magnetic Field Sensor
IMPLEMENT_CTOR0(sensors, UASensorsMagnetic*, ua_sensors_magnetic_new);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_magnetic_enable, UASensorsMagnetic*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_magnetic_disable, UASensorsMagnetic*);
IMPLEMENT_FUNCTION1(sensors, uint32_t, ua_sensors_magnetic_get_min_delay, UASensorsMagnetic*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_magnetic_get_min_value, UASensorsMagnetic*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_magnetic_get_max_value, UASensorsMagnetic*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_magnetic_get_resolution, UASensorsMagnetic*, float*);
IMPLEMENT_VOID_FUNCTION3(sensors, ua_sensors_magnetic_set_reading_cb, UASensorsMagnetic*, on_magnetic_event_cb, void*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_magnetic_set_event_rate, UASensorsMagnetic*, uint32_t);

// Magnetic Field Sensor Event
IMPLEMENT_FUNCTION1(sensors, uint64_t, uas_magnetic_event_get_timestamp, UASMagneticEvent*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_magnetic_event_get_magnetic_field_x, UASMagneticEvent*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_magnetic_event_get_magnetic_field_y, UASMagneticEvent*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_magnetic_event_get_magnetic_field_z, UASMagneticEvent*, float*);

// Ambient Temperature Sensor
IMPLEMENT_CTOR0(sensors, UASensorsTemperature*, ua_sensors_temperature_new);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_temperature_enable, UASensorsTemperature*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_temperature_disable, UASensorsTemperature*);
IMPLEMENT_FUNCTION1(sensors, uint32_t, ua_sensors_temperature_get_min_delay, UASensorsTemperature*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_temperature_get_min_value, UASensorsTemperature*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_temperature_get_max_value, UASensorsTemperature*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_temperature_get_resolution, UASensorsTemperature*, float*);
IMPLEMENT_VOID_FUNCTION3(sensors, ua_sensors_temperature_set_reading_cb, UASensorsTemperature*, on_temperature_event_cb, void*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_temperature_set_event_rate, UASensorsTemperature*, uint32_t);

// Ambient Temperature Sensor Event
IMPLEMENT_FUNCTION1(sensors, uint64_t, uas_temperature_event_get_timestamp, UASTemperatureEvent*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_temperature_event_get_temperature, UASTemperatureEvent*, float*);

// Ambient Pressure Sensor
IMPLEMENT_CTOR0(sensors, UASensorsPressure*, ua_sensors_pressure_new);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_pressure_enable, UASensorsPressure*);
IMPLEMENT_FUNCTION1(sensors, UStatus, ua_sensors_pressure_disable, UASensorsPressure*);
IMPLEMENT_FUNCTION1(sensors, uint32_t, ua_sensors_pressure_get_min_delay, UASensorsPressure*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_pressure_get_min_value, UASensorsPressure*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_pressure_get_max_value, UASensorsPressure*, float*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_pressure_get_resolution, UASensorsPressure*, float*);
IMPLEMENT_VOID_FUNCTION3(sensors, ua_sensors_pressure_set_reading_cb, UASensorsPressure*, on_pressure_event_cb, void*);
IMPLEMENT_FUNCTION2(sensors, UStatus, ua_sensors_pressure_set_event_rate, UASensorsPressure*, uint32_t);

// Ambient Pressure Sensor Event
IMPLEMENT_FUNCTION1(sensors, uint64_t, uas_pressure_event_get_timestamp, UASPressureEvent*);
IMPLEMENT_FUNCTION2(sensors, UStatus, uas_pressure_event_get_pressure, UASPressureEvent*, float*);


// Location

IMPLEMENT_VOID_FUNCTION1(location, ua_location_service_controller_ref, UALocationServiceController*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_service_controller_unref, UALocationServiceController*);
IMPLEMENT_VOID_FUNCTION3(location, ua_location_service_controller_set_status_changed_handler, UALocationServiceController*, UALocationServiceStatusChangedHandler, void*);
IMPLEMENT_FUNCTION2(location, UStatus, ua_location_service_controller_query_status, UALocationServiceController*, UALocationServiceStatusFlags*);
IMPLEMENT_FUNCTION1(location, UStatus, ua_location_service_controller_enable_service, UALocationServiceController*);
IMPLEMENT_FUNCTION1(location, UStatus, ua_location_service_controller_disable_service, UALocationServiceController*);
IMPLEMENT_FUNCTION1(location, UStatus, ua_location_service_controller_enable_gps, UALocationServiceController*);
IMPLEMENT_FUNCTION1(location, UStatus, ua_location_service_controller_disable_gps, UALocationServiceController*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_heading_update_ref, UALocationHeadingUpdate*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_heading_update_unref, UALocationHeadingUpdate*);
IMPLEMENT_FUNCTION1(location, uint64_t, ua_location_heading_update_get_timestamp, UALocationHeadingUpdate*);
IMPLEMENT_FUNCTION1(location, double, ua_location_heading_update_get_heading_in_degree, UALocationHeadingUpdate*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_position_update_ref, UALocationPositionUpdate*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_position_update_unref, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, uint64_t, ua_location_position_update_get_timestamp, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, double, ua_location_position_update_get_latitude_in_degree, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, double, ua_location_position_update_get_longitude_in_degree, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, bool, ua_location_position_update_has_altitude, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, double, ua_location_position_update_get_altitude_in_meter, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, bool, ua_location_position_update_has_horizontal_accuracy, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, double, ua_location_position_update_get_horizontal_accuracy_in_meter, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, bool, ua_location_position_update_has_vertical_accuracy, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, double, ua_location_position_update_get_vertical_accuracy_in_meter, UALocationPositionUpdate*);
IMPLEMENT_FUNCTION1(location, UALocationServiceSession*, ua_location_service_create_session_for_low_accuracy, UALocationServiceRequirementsFlags);
IMPLEMENT_FUNCTION2(location, UALocationServiceSession*, ua_location_service_try_create_session_for_low_accuracy, UALocationServiceRequirementsFlags, UALocationServiceError*);
IMPLEMENT_FUNCTION1(location, UALocationServiceSession*, ua_location_service_create_session_for_high_accuracy, UALocationServiceRequirementsFlags);
IMPLEMENT_FUNCTION2(location, UALocationServiceSession*, ua_location_service_try_create_session_for_high_accuracy, UALocationServiceRequirementsFlags, UALocationServiceError*);
IMPLEMENT_CTOR0(location, UALocationServiceController*, ua_location_service_create_controller);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_service_session_ref, UALocationServiceSession*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_service_session_unref, UALocationServiceSession*);
IMPLEMENT_VOID_FUNCTION3(location, ua_location_service_session_set_position_updates_handler, UALocationServiceSession*, UALocationServiceSessionPositionUpdatesHandler, void*);
IMPLEMENT_VOID_FUNCTION3(location, ua_location_service_session_set_heading_updates_handler, UALocationServiceSession*, UALocationServiceSessionHeadingUpdatesHandler, void*);
IMPLEMENT_VOID_FUNCTION3(location, ua_location_service_session_set_velocity_updates_handler, UALocationServiceSession*, UALocationServiceSessionVelocityUpdatesHandler, void*);
IMPLEMENT_FUNCTION1(location, UStatus, ua_location_service_session_start_position_updates, UALocationServiceSession*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_service_session_stop_position_updates, UALocationServiceSession*);
IMPLEMENT_FUNCTION1(location, UStatus, ua_location_service_session_start_heading_updates, UALocationServiceSession*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_service_session_stop_heading_updates, UALocationServiceSession*);
IMPLEMENT_FUNCTION1(location, UStatus, ua_location_service_session_start_velocity_updates, UALocationServiceSession*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_service_session_stop_velocity_updates, UALocationServiceSession*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_velocity_update_ref, UALocationVelocityUpdate*);
IMPLEMENT_VOID_FUNCTION1(location, ua_location_velocity_update_unref, UALocationVelocityUpdate*);
IMPLEMENT_FUNCTION1(location, uint64_t, ua_location_velocity_update_get_timestamp, UALocationVelocityUpdate*);
IMPLEMENT_FUNCTION1(location, double, ua_location_velocity_update_get_velocity_in_meters_per_second, UALocationVelocityUpdate*);

// URL Dispatcher

IMPLEMENT_CTOR0(url_dispatcher, UAUrlDispatcherSession*, ua_url_dispatcher_session);
IMPLEMENT_VOID_FUNCTION4(url_dispatcher, ua_url_dispatcher_session_open, UAUrlDispatcherSession*, const char*, UAUrlDispatcherSessionDispatchHandler, void*);

#ifdef __cplusplus
}
#endif
