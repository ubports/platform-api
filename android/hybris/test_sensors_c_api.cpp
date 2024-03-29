/*
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Ricardo Mendoza <ricardo.mendoza@canonical.com>
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <ubuntu/application/sensors/accelerometer.h>
#include <ubuntu/application/sensors/proximity.h>
#include <ubuntu/application/sensors/light.h>
#include <ubuntu/application/sensors/orientation.h>
#include <ubuntu/application/sensors/gyroscope.h>
#include <ubuntu/application/sensors/magnetic.h>
#include <ubuntu/application/sensors/temperature.h>
#include <ubuntu/application/sensors/pressure.h>

void on_new_accelerometer_event(UASAccelerometerEvent* event, void* context)
{
    float x; uas_accelerometer_event_get_acceleration_x(event, &x);
    float y; uas_accelerometer_event_get_acceleration_y(event, &y);
    float z; uas_accelerometer_event_get_acceleration_z(event, &z);

    printf("%s \n", __PRETTY_FUNCTION__);
    printf("\ttime: %" PRIu64 "\n", uas_accelerometer_event_get_timestamp(event));
    printf("\tx: %f\n", x);
    printf("\ty: %f\n", y);
    printf("\tz: %f\n", z);
}

void on_new_proximity_event(UASProximityEvent* event, void* context)
{
    printf("%s \n", __PRETTY_FUNCTION__);
    printf("\ttime: %" PRIu64 "\n", uas_proximity_event_get_timestamp(event));

    switch (uas_proximity_event_get_distance(event))
    {
        case U_PROXIMITY_NEAR:
        {   
            printf("\tdistance: NEAR\n");
            break;
        }
        case U_PROXIMITY_FAR:
        {
            printf("\tdistance: FAR\n");
            break;
        }
    }
}

void on_new_light_event(UASLightEvent* event, void* context)
{
    float light = -1.f; uas_light_event_get_light(event, &light);

    printf("%s \n", __PRETTY_FUNCTION__);
    printf("\ttime: %" PRIu64 "\n", uas_light_event_get_timestamp(event));
    printf("\tlight: %f\n", light);
}

void on_new_orientation_event(UASOrientationEvent* event, void* context)
{
    float x; uas_orientation_event_get_azimuth(event, &x);
    float y; uas_orientation_event_get_pitch(event, &y);
    float z; uas_orientation_event_get_roll(event, &z);

    printf("%s \n", __PRETTY_FUNCTION__);
    printf("\ttime: %" PRIu64 "\n", uas_orientation_event_get_timestamp(event));
    printf("\tx: %f\n", x);
    printf("\ty: %f\n", y);
    printf("\tz: %f\n", z);
}

void on_new_gyroscope_event(UASGyroscopeEvent* event, void* context)
{
    float x; uas_gyroscope_event_get_rate_of_rotation_around_x(event, &x);
    float y; uas_gyroscope_event_get_rate_of_rotation_around_y(event, &y);
    float z; uas_gyroscope_event_get_rate_of_rotation_around_z(event, &z);

    printf("%s \n", __PRETTY_FUNCTION__);
    printf("\ttime: %" PRIu64 "\n", uas_gyroscope_event_get_timestamp(event));
    printf("\tomega_x: %f\n", x);
    printf("\tomega_y: %f\n", y);
    printf("\tomega_z: %f\n", z);
}

void on_new_magnetic_event(UASMagneticEvent* event, void* context)
{
    float x; uas_magnetic_event_get_magnetic_field_x(event, &x);
    float y; uas_magnetic_event_get_magnetic_field_y(event, &y);
    float z; uas_magnetic_event_get_magnetic_field_z(event, &z);

    printf("%s \n", __PRETTY_FUNCTION__);
    printf("\ttime: %" PRIu64 "\n", uas_magnetic_event_get_timestamp(event));
    printf("\tx: %f\n", x);
    printf("\ty: %f\n", y);
    printf("\tz: %f\n", z);
}

void on_new_temperature_event(UASTemperatureEvent* event, void* context)
{
    float temperature = -1.f; uas_temperature_event_get_temperature(event, &temperature);

    printf("%s \n", __PRETTY_FUNCTION__);
    printf("\ttime: %" PRIu64 "\n", uas_temperature_event_get_timestamp(event));
    printf("\ttemperature: %f\n", temperature);
}

void on_new_pressure_event(UASPressureEvent* event, void* context)
{
    float pressure = -1.f; uas_pressure_event_get_pressure(event, &pressure);

    printf("%s \n", __PRETTY_FUNCTION__);
    printf("\ttime: %" PRIu64 "\n", uas_pressure_event_get_timestamp(event));
    printf("\tpressure: %f\n", pressure);
}

int main(int argc, char** argv)
{
    UASensorsAccelerometer* accelerometer = ua_sensors_accelerometer_new();
    UASensorsProximity* proximity = ua_sensors_proximity_new();
    UASensorsLight* ambientlight = ua_sensors_light_new();
    UASensorsOrientation* orientation = ua_sensors_orientation_new();
    UASensorsGyroscope* gyroscope = ua_sensors_gyroscope_new();
    UASensorsMagnetic* magnetic = ua_sensors_magnetic_new();
    UASensorsTemperature* temperature = ua_sensors_temperature_new();
    UASensorsPressure* pressure = ua_sensors_pressure_new();

    if(accelerometer != NULL) {
        ua_sensors_accelerometer_set_reading_cb(accelerometer,
                                                on_new_accelerometer_event,
                                                NULL);
        ua_sensors_accelerometer_enable(accelerometer);
    } else {
        printf("WARN: accelerometer could not be allocated!\n");

    }

    if(proximity != NULL) {
        ua_sensors_proximity_set_reading_cb(proximity,
                                            on_new_proximity_event,
                                            NULL);
        ua_sensors_proximity_enable(proximity);
    } else {
        printf("WARN: proximity could not be allocated!\n");

    }

    if(ambientlight != NULL) {
        ua_sensors_light_set_reading_cb(ambientlight,
                                        on_new_light_event,
                                        NULL);
        ua_sensors_light_enable(ambientlight);
    } else {
        printf("WARN: ambientlight could not be allocated!\n");

    }

    if(orientation != NULL) {
        ua_sensors_orientation_set_reading_cb(orientation,
                                              on_new_orientation_event,
                                              NULL);
        ua_sensors_orientation_enable(orientation);
    } else {
        printf("WARN: orientation could not be allocated!\n");

    }

    if(gyroscope != NULL) {
        ua_sensors_gyroscope_set_reading_cb(gyroscope,
                                            on_new_gyroscope_event,
                                            NULL);
        ua_sensors_gyroscope_enable(gyroscope);
    } else {
        printf("WARN: gyroscope could not be allocated!\n");

    }

    if(magnetic != NULL) {
        ua_sensors_magnetic_set_reading_cb(magnetic,
                                           on_new_magnetic_event,
                                           NULL);
        ua_sensors_magnetic_enable(magnetic);
    } else {
        printf("WARN: magnetometer could not be allocated!\n");

    }

    if(temperature != NULL) {
        ua_sensors_temperature_set_reading_cb(temperature,
                                           on_new_temperature_event,
                                           NULL);
        ua_sensors_temperature_enable(temperature);
    } else {
        printf("WARN: temperature could not be allocated!\n");

    }

    if(pressure != NULL) {
        ua_sensors_pressure_set_reading_cb(pressure,
                                           on_new_pressure_event,
                                           NULL);
        ua_sensors_pressure_enable(pressure);
    } else {
        printf("WARN: pressure could not be allocated!\n");

    }

    while(true)
    {
    }
}
