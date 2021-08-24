/*
 * Copyright © 2012 Canonical Ltd.
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
 * Authored by: Thomas Voß <thomas.voss@canonical.com>
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

#include <private/application/sensors/sensor.h>
#include <private/application/sensors/sensor_listener.h>
#include <private/application/sensors/sensor_service.h>
#include <private/application/sensors/sensor_type.h>
#include <private/application/sensors/events.h>

#include <cassert>
#include <cstdio>

namespace
{

enum sensor_value_t { MIN_DELAY, MIN_VALUE, MAX_VALUE, RESOLUTION };
template<ubuntu::application::sensors::SensorType sensor_type>
struct SensorListener : public ubuntu::application::sensors::SensorListener
{
    SensorListener() : on_accelerometer_event(NULL),
                       on_proximity_event(NULL),
                       on_light_event(NULL),
                       on_orientation_event(NULL),
                       on_gyroscope_event(NULL),
                       on_magnetic_event(NULL),
                       on_temperature_event(NULL),
                       on_pressure_event(NULL),
                       context(nullptr)
    {
    }

    void on_new_reading(const ubuntu::application::sensors::SensorReading::Ptr& reading)
    {
        switch(sensor_type)
        {
            case ubuntu::application::sensors::sensor_type_orientation:
            {
                if (!on_orientation_event)
                    return;

                ubuntu::application::sensors::OrientationEvent ev(
                        reading->timestamp,
                        reading->vector[0],
                        reading->vector[1],
                        reading->vector[2]
                        );

                on_orientation_event(
                    &ev, this->context
                    );

                break;
            }
            case ubuntu::application::sensors::sensor_type_accelerometer:
            {
                if (!on_accelerometer_event)
                    return;

                ubuntu::application::sensors::AccelerometerEvent ev(
                        reading->timestamp,
                        reading->acceleration[0],
                        reading->acceleration[1],
                        reading->acceleration[2]
                        );

                on_accelerometer_event(
                    &ev, this->context
                    );

                break;
            }
            case ubuntu::application::sensors::sensor_type_proximity:
            {
                if (!on_proximity_event)
                    return;

                ubuntu::application::sensors::ProximityEvent ev(
                        static_cast<uint64_t>(reading->timestamp),
                        reading->distance
                    );

                on_proximity_event(
                    &ev, this->context
                    );

                break;
            }
            case ubuntu::application::sensors::sensor_type_light:
            {
                if (!on_light_event)
                    return;

                ubuntu::application::sensors::LightEvent ev(
                        reading->timestamp,
                        reading->light
                    );

                on_light_event(
                    &ev, this->context
                    );

                break;
            }
            case ubuntu::application::sensors::sensor_type_gyroscope:
            {
                if (!on_gyroscope_event)
                    return;

                ubuntu::application::sensors::GyroscopeEvent ev(
                        reading->timestamp,
                        reading->gyroscopic[0],
                        reading->gyroscopic[1],
                        reading->gyroscopic[2]
                        );

                on_gyroscope_event(
                    &ev, this->context
                    );

                break;
            }
            case ubuntu::application::sensors::sensor_type_magnetic_field:
            {
                if (!on_magnetic_event)
                    return;

                ubuntu::application::sensors::MagneticEvent ev(
                        reading->timestamp,
                        reading->magnetic[0],
                        reading->magnetic[1],
                        reading->magnetic[2]
                        );

                on_magnetic_event(
                    &ev, this->context
                    );

                break;
            }
            case ubuntu::application::sensors::sensor_type_temperature:
            {
                if (!on_temperature_event)
                    return;

                ubuntu::application::sensors::TemperatureEvent ev(
                        reading->timestamp,
                        reading->temperature
                    );

                on_temperature_event(
                    &ev, this->context
                    );

                break;
            }
            case ubuntu::application::sensors::sensor_type_pressure:
            {
                if (!on_pressure_event)
                    return;

                ubuntu::application::sensors::PressureEvent ev(
                        reading->timestamp,
                        reading->pressure
                    );

                on_pressure_event(
                    &ev, this->context
                    );

                break;
            }
        }
    }

    on_accelerometer_event_cb on_accelerometer_event;
    on_proximity_event_cb on_proximity_event;
    on_light_event_cb on_light_event;
    on_orientation_event_cb on_orientation_event;
    on_gyroscope_event_cb on_gyroscope_event;
    on_magnetic_event_cb on_magnetic_event;
    on_temperature_event_cb on_temperature_event;
    on_pressure_event_cb on_pressure_event;
    void *context;
};

ubuntu::application::sensors::Sensor::Ptr orientation;
ubuntu::application::sensors::Sensor::Ptr accelerometer;
ubuntu::application::sensors::Sensor::Ptr proximity;
ubuntu::application::sensors::Sensor::Ptr light;
ubuntu::application::sensors::Sensor::Ptr gyroscope;
ubuntu::application::sensors::Sensor::Ptr magnetic;
ubuntu::application::sensors::Sensor::Ptr temperature;
ubuntu::application::sensors::Sensor::Ptr pressure;
ubuntu::application::sensors::SensorListener::Ptr orientation_listener;
ubuntu::application::sensors::SensorListener::Ptr accelerometer_listener;
ubuntu::application::sensors::SensorListener::Ptr proximity_listener;
ubuntu::application::sensors::SensorListener::Ptr light_listener;
ubuntu::application::sensors::SensorListener::Ptr gyroscope_listener;
ubuntu::application::sensors::SensorListener::Ptr magnetic_listener;
ubuntu::application::sensors::SensorListener::Ptr temperature_listener;
ubuntu::application::sensors::SensorListener::Ptr pressure_listener;
}

static int32_t toHz(int32_t microseconds)
{
    return 1 / (microseconds / 1e6);
}

/*
 * Proximity Sensor
 */

UASensorsProximity*
ua_sensors_proximity_new()
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);

    proximity =
        ubuntu::application::sensors::SensorService::sensor_for_type(
            ubuntu::application::sensors::sensor_type_proximity);

    return proximity.get();
}

UStatus
ua_sensors_proximity_enable(
    UASensorsProximity* sensor)
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);

    if (sensor == NULL)
        return U_STATUS_ERROR;

    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    int ret = s->enable();

    if (ret < 0)
        return U_STATUS_ERROR;

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_proximity_disable(
    UASensorsProximity* sensor)
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);

    if (sensor == NULL)
        return U_STATUS_ERROR;

    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    s->disable();

    return U_STATUS_SUCCESS;
}

uint32_t
ua_sensors_proximity_get_min_delay(
    UASensorsProximity* sensor)
{
    if (sensor == NULL)
        return -1;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    return toHz(s->min_delay());
}

UStatus
ua_sensors_proximity_get_min_value(
    UASensorsProximity* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->min_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_proximity_get_max_value(
    UASensorsProximity* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->max_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_proximity_get_resolution(
    UASensorsProximity* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->resolution();

    return U_STATUS_SUCCESS;
}

void
ua_sensors_proximity_set_reading_cb(
    UASensorsProximity* sensor,
    on_proximity_event_cb cb,
    void *ctx)
{
    if (sensor == NULL)
        return;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    SensorListener<ubuntu::application::sensors::sensor_type_proximity>* sl
        = new SensorListener<ubuntu::application::sensors::sensor_type_proximity>();

    sl->on_proximity_event = cb;
    sl->context = ctx;

    proximity_listener = sl;
    s->register_listener(proximity_listener);
}

UStatus
ua_sensors_proximity_set_event_rate(
    UASensorsProximity* sensor,
    uint32_t rate)
{
    if (sensor == NULL)
       return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    if (!s->set_event_rate(rate))
        return U_STATUS_ERROR;

    return U_STATUS_SUCCESS;
}

uint64_t
uas_proximity_event_get_timestamp(
    UASProximityEvent* event)
{
    auto ev = static_cast<ubuntu::application::sensors::ProximityEvent*>(event);

    return ev->get_timestamp();
}

UASProximityDistance
uas_proximity_event_get_distance(
    UASProximityEvent* event)
{
    auto ev = static_cast<ubuntu::application::sensors::ProximityEvent*>(event);

    if (ev->get_distance() == proximity->max_value())
        return U_PROXIMITY_FAR;

    return U_PROXIMITY_NEAR;
}

/*
 * Ambient Light Sensor
 */

UASensorsLight*
ua_sensors_light_new()
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    light =
        ubuntu::application::sensors::SensorService::sensor_for_type(
            ubuntu::application::sensors::sensor_type_light);

    return light.get();
}

UStatus
ua_sensors_light_enable(
    UASensorsLight* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    s->enable();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_light_disable(
    UASensorsLight* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    s->disable();

    return U_STATUS_SUCCESS;
}

uint32_t
ua_sensors_light_get_min_delay(
    UASensorsLight* sensor)
{
    if (sensor == NULL)
        return -1;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    return toHz(s->min_delay());
}

UStatus
ua_sensors_light_get_min_value(
    UASensorsLight* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->min_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_light_get_max_value(
    UASensorsLight* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->max_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_light_get_resolution(
    UASensorsLight* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->resolution();

    return U_STATUS_SUCCESS;
}

void
ua_sensors_light_set_reading_cb(
    UASensorsLight* sensor,
    on_light_event_cb cb,
    void *ctx)
{
    if (sensor == NULL)
        return;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    SensorListener<ubuntu::application::sensors::sensor_type_light>* sl
        = new SensorListener<ubuntu::application::sensors::sensor_type_light>();

    sl->on_light_event = cb;
    sl->context = ctx;

    light_listener = sl;
    s->register_listener(light_listener);
}

UStatus
ua_sensors_light_set_event_rate(
    UASensorsLight* sensor,
    uint32_t rate)
{
    if (sensor == NULL)
       return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    if (!s->set_event_rate(rate))
        return U_STATUS_ERROR;

    return U_STATUS_SUCCESS;
}

uint64_t
uas_light_event_get_timestamp(
    UASLightEvent* event)
{
    auto ev = static_cast<ubuntu::application::sensors::LightEvent*>(event);
    return ev->get_timestamp();
}

UStatus
uas_light_event_get_light(
    UASLightEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::LightEvent*>(event);
    *value = ev->get_light();

    return U_STATUS_SUCCESS;
}

/*
 * Acceleration Sensor
 */

UASensorsAccelerometer*
ua_sensors_accelerometer_new()
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    accelerometer =
        ubuntu::application::sensors::SensorService::sensor_for_type(
            ubuntu::application::sensors::sensor_type_accelerometer);

    return accelerometer.get();
}

UStatus
ua_sensors_accelerometer_enable(
    UASensorsAccelerometer* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    s->enable();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_accelerometer_disable(
    UASensorsAccelerometer* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    s->disable();

    return U_STATUS_SUCCESS;
}

uint32_t
ua_sensors_accelerometer_get_min_delay(
    UASensorsAccelerometer* sensor)
{
    if (sensor == NULL)
        return -1;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    return toHz(s->min_delay());
}

UStatus
ua_sensors_accelerometer_get_min_value(
    UASensorsAccelerometer* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->min_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_accelerometer_get_max_value(
    UASensorsAccelerometer* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->max_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_accelerometer_get_resolution(
    UASensorsAccelerometer* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->resolution();

    return U_STATUS_SUCCESS;
}

void
ua_sensors_accelerometer_set_reading_cb(
    UASensorsAccelerometer* sensor,
    on_accelerometer_event_cb cb,
    void *ctx)
{
    if (sensor == NULL)
        return;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    SensorListener<ubuntu::application::sensors::sensor_type_accelerometer>* sl
        = new SensorListener<ubuntu::application::sensors::sensor_type_accelerometer>();

    sl->on_accelerometer_event = cb;
    sl->context = ctx;

    accelerometer_listener = sl;
    s->register_listener(accelerometer_listener);
}

UStatus
ua_sensors_accelerometer_set_event_rate(
    UASensorsAccelerometer* sensor,
    uint32_t rate)
{
    if (sensor == NULL)
       return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    if (!s->set_event_rate(rate))
        return U_STATUS_ERROR;

    return U_STATUS_SUCCESS;
}

uint64_t
uas_accelerometer_event_get_timestamp(
    UASAccelerometerEvent* event)
{
    auto ev = static_cast<ubuntu::application::sensors::AccelerometerEvent*>(event);
    return ev->get_timestamp();
}

UStatus
uas_accelerometer_event_get_acceleration_x(
    UASAccelerometerEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::AccelerometerEvent*>(event);
    *value = ev->get_x();

    return U_STATUS_SUCCESS;
}

UStatus
uas_accelerometer_event_get_acceleration_y(
    UASAccelerometerEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::AccelerometerEvent*>(event);
    *value = ev->get_y();

    return U_STATUS_SUCCESS;
}

UStatus
uas_accelerometer_event_get_acceleration_z(
    UASAccelerometerEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::AccelerometerEvent*>(event);
    *value = ev->get_z();

    return U_STATUS_SUCCESS;
}

/*
 * Orientation Sensor
 */

UASensorsOrientation*
ua_sensors_orientation_new()
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    orientation =
        ubuntu::application::sensors::SensorService::sensor_for_type(
            ubuntu::application::sensors::sensor_type_orientation);

    return orientation.get();
}

UStatus
ua_sensors_orientation_enable(
    UASensorsOrientation* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    s->enable();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_orientation_disable(
    UASensorsOrientation* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    s->disable();

    return U_STATUS_SUCCESS;
}

uint32_t
ua_sensors_orientation_get_min_delay(
    UASensorsOrientation* sensor)
{
    if (sensor == NULL)
        return -1;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    return toHz(s->min_delay());
}

UStatus
ua_sensors_orientation_get_min_value(
    UASensorsOrientation* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->min_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_orientation_get_max_value(
    UASensorsOrientation* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->max_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_orientation_get_resolution(
    UASensorsOrientation* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->resolution();

    return U_STATUS_SUCCESS;
}

void
ua_sensors_orientation_set_reading_cb(
    UASensorsOrientation* sensor,
    on_orientation_event_cb cb,
    void *ctx)
{
    if (sensor == NULL)
        return;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    SensorListener<ubuntu::application::sensors::sensor_type_orientation>* sl
        = new SensorListener<ubuntu::application::sensors::sensor_type_orientation>();

    sl->on_orientation_event = cb;
    sl->context = ctx;

    orientation_listener = sl;
    s->register_listener(orientation_listener);
}

UStatus
ua_sensors_orientation_set_event_rate(
    UASensorsOrientation* sensor,
    uint32_t rate)
{
    if (sensor == NULL)
       return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    if (!s->set_event_rate(rate))
        return U_STATUS_ERROR;

    return U_STATUS_SUCCESS;
}

uint64_t
uas_orientation_event_get_timestamp(
    UASOrientationEvent* event)
{
    auto ev = static_cast<ubuntu::application::sensors::OrientationEvent*>(event);
    return ev->get_timestamp();
}

UStatus
uas_orientation_event_get_azimuth(
    UASOrientationEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::OrientationEvent*>(event);
    *value = ev->get_azimuth();

    return U_STATUS_SUCCESS;
}

UStatus
uas_orientation_event_get_pitch(
    UASOrientationEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::OrientationEvent*>(event);
    *value = ev->get_pitch();

    return U_STATUS_SUCCESS;
}

UStatus
uas_orientation_event_get_roll(
    UASOrientationEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::OrientationEvent*>(event);
    *value = ev->get_roll();

    return U_STATUS_SUCCESS;
}

/*
 * Gyroscopic Sensor
 */

UASensorsGyroscope*
ua_sensors_gyroscope_new()
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    gyroscope =
        ubuntu::application::sensors::SensorService::sensor_for_type(
            ubuntu::application::sensors::sensor_type_gyroscope);

    return gyroscope.get();
}

UStatus
ua_sensors_gyroscope_enable(
    UASensorsGyroscope* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    s->enable();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_gyroscope_disable(
    UASensorsGyroscope* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    s->disable();

    return U_STATUS_SUCCESS;
}

uint32_t
ua_sensors_gyroscope_get_min_delay(
    UASensorsGyroscope* sensor)
{
    if (sensor == NULL)
        return -1;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    return toHz(s->min_delay());
}

UStatus
ua_sensors_gyroscope_get_min_value(
    UASensorsGyroscope* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->min_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_gyroscope_get_max_value(
    UASensorsGyroscope* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->max_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_gyroscope_get_resolution(
    UASensorsGyroscope* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->resolution();

    return U_STATUS_SUCCESS;
}

void
ua_sensors_gyroscope_set_reading_cb(
    UASensorsGyroscope* sensor,
    on_gyroscope_event_cb cb,
    void *ctx)
{
    if (sensor == NULL)
        return;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    SensorListener<ubuntu::application::sensors::sensor_type_gyroscope>* sl
        = new SensorListener<ubuntu::application::sensors::sensor_type_gyroscope>();

    sl->on_gyroscope_event = cb;
    sl->context = ctx;

    gyroscope_listener = sl;
    s->register_listener(gyroscope_listener);
}

UStatus
ua_sensors_gyroscope_set_event_rate(
    UASensorsGyroscope* sensor,
    uint32_t rate)
{
    if (sensor == NULL)
       return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    if (!s->set_event_rate(rate))
        return U_STATUS_ERROR;

    return U_STATUS_SUCCESS;
}

uint64_t
uas_gyroscope_event_get_timestamp(
    UASGyroscopeEvent* event)
{
    auto ev = static_cast<ubuntu::application::sensors::GyroscopeEvent*>(event);
    return ev->get_timestamp();
}

UStatus
uas_gyroscope_event_get_rate_of_rotation_around_x(
    UASGyroscopeEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::GyroscopeEvent*>(event);
    *value = ev->get_x_rotation_rate();

    return U_STATUS_SUCCESS;
}

UStatus
uas_gyroscope_event_get_rate_of_rotation_around_y(
    UASGyroscopeEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::GyroscopeEvent*>(event);
    *value = ev->get_y_rotation_rate();

    return U_STATUS_SUCCESS;
}

UStatus
uas_gyroscope_event_get_rate_of_rotation_around_z(
    UASGyroscopeEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::GyroscopeEvent*>(event);
    *value = ev->get_z_rotation_rate();

    return U_STATUS_SUCCESS;
}


/*
 * Magnetic Field Sensor
 */

UASensorsMagnetic*
ua_sensors_magnetic_new()
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    magnetic =
        ubuntu::application::sensors::SensorService::sensor_for_type(
            ubuntu::application::sensors::sensor_type_magnetic_field);

    return magnetic.get();
}

UStatus
ua_sensors_magnetic_enable(
    UASensorsMagnetic* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    s->enable();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_magnetic_disable(
    UASensorsMagnetic* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    s->disable();

    return U_STATUS_SUCCESS;
}

uint32_t
ua_sensors_magnetic_get_min_delay(
    UASensorsMagnetic* sensor)
{
    if (sensor == NULL)
        return -1;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    return toHz(s->min_delay());
}

UStatus
ua_sensors_magnetic_get_min_value(
    UASensorsMagnetic* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->min_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_magnetic_get_max_value(
    UASensorsMagnetic* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->max_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_magnetic_get_resolution(
    UASensorsMagnetic* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->resolution();

    return U_STATUS_SUCCESS;
}

void
ua_sensors_magnetic_set_reading_cb(
    UASensorsMagnetic* sensor,
    on_magnetic_event_cb cb,
    void *ctx)
{
    if (sensor == NULL)
        return;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    SensorListener<ubuntu::application::sensors::sensor_type_magnetic_field>* sl
        = new SensorListener<ubuntu::application::sensors::sensor_type_magnetic_field>();

    sl->on_magnetic_event = cb;
    sl->context = ctx;

    magnetic_listener = sl;
    s->register_listener(magnetic_listener);
}

UStatus
ua_sensors_magnetic_set_event_rate(
    UASensorsMagnetic* sensor,
    uint32_t rate)
{
    if (sensor == NULL)
       return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    if (!s->set_event_rate(rate))
        return U_STATUS_ERROR;

    return U_STATUS_SUCCESS;
}

uint64_t
uas_magnetic_event_get_timestamp(
    UASMagneticEvent* event)
{
    auto ev = static_cast<ubuntu::application::sensors::MagneticEvent*>(event);
    return ev->get_timestamp();
}

UStatus
uas_magnetic_event_get_magnetic_field_x(
    UASMagneticEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::MagneticEvent*>(event);
    *value = ev->get_x();

    return U_STATUS_SUCCESS;
}

UStatus
uas_magnetic_event_get_magnetic_field_y(
    UASMagneticEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::MagneticEvent*>(event);
    *value = ev->get_y();

    return U_STATUS_SUCCESS;
}

UStatus
uas_magnetic_event_get_magnetic_field_z(
    UASMagneticEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::MagneticEvent*>(event);
    *value = ev->get_z();

    return U_STATUS_SUCCESS;
}

/*
 * Temperature Sensor
 */

UASensorsTemperature*
ua_sensors_temperature_new()
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    temperature =
        ubuntu::application::sensors::SensorService::sensor_for_type(
            ubuntu::application::sensors::sensor_type_temperature);

    return temperature.get();
}

UStatus
ua_sensors_temperature_enable(
    UASensorsTemperature* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    s->enable();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_temperature_disable(
    UASensorsTemperature* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    s->disable();

    return U_STATUS_SUCCESS;
}

uint32_t
ua_sensors_temperature_get_min_delay(
    UASensorsTemperature* sensor)
{
    if (sensor == NULL)
        return -1;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    return toHz(s->min_delay());
}

UStatus
ua_sensors_temperature_get_min_value(
    UASensorsTemperature* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->min_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_temperature_get_max_value(
    UASensorsTemperature* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->max_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_temperature_get_resolution(
    UASensorsTemperature* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->resolution();

    return U_STATUS_SUCCESS;
}

void
ua_sensors_temperature_set_reading_cb(
    UASensorsTemperature* sensor,
    on_temperature_event_cb cb,
    void *ctx)
{
    if (sensor == NULL)
        return;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    SensorListener<ubuntu::application::sensors::sensor_type_temperature>* sl
        = new SensorListener<ubuntu::application::sensors::sensor_type_temperature>();

    sl->on_temperature_event = cb;
    sl->context = ctx;

    temperature_listener = sl;
    s->register_listener(temperature_listener);
}

UStatus
ua_sensors_temperature_set_event_rate(
    UASensorsTemperature* sensor,
    uint32_t rate)
{
    if (sensor == NULL)
       return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    if (!s->set_event_rate(rate))
        return U_STATUS_ERROR;

    return U_STATUS_SUCCESS;
}

uint64_t
uas_temperature_event_get_timestamp(
    UASTemperatureEvent* event)
{
    auto ev = static_cast<ubuntu::application::sensors::TemperatureEvent*>(event);
    return ev->get_timestamp();
}

UStatus
uas_temperature_event_get_temperature(
    UASTemperatureEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::TemperatureEvent*>(event);
    *value = ev->get_temperature();

    return U_STATUS_SUCCESS;
}

/*
 * Pressure Sensor
 */

UASensorsPressure*
ua_sensors_pressure_new()
{
    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    pressure =
        ubuntu::application::sensors::SensorService::sensor_for_type(
            ubuntu::application::sensors::sensor_type_pressure);

    return pressure.get();
}

UStatus
ua_sensors_pressure_enable(
    UASensorsPressure* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    s->enable();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_pressure_disable(
    UASensorsPressure* sensor)
{
    if (sensor == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    s->disable();

    return U_STATUS_SUCCESS;
}

uint32_t
ua_sensors_pressure_get_min_delay(
    UASensorsPressure* sensor)
{
    if (sensor == NULL)
        return -1;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    return toHz(s->min_delay());
}

UStatus
ua_sensors_pressure_get_min_value(
    UASensorsPressure* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->min_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_pressure_get_max_value(
    UASensorsPressure* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->max_value();

    return U_STATUS_SUCCESS;
}

UStatus
ua_sensors_pressure_get_resolution(
    UASensorsPressure* sensor,
    float* value)
{
    if (sensor == NULL || value == NULL)
        return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    *value = s->resolution();

    return U_STATUS_SUCCESS;
}

void
ua_sensors_pressure_set_reading_cb(
    UASensorsPressure* sensor,
    on_pressure_event_cb cb,
    void *ctx)
{
    if (sensor == NULL)
        return;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);

    SensorListener<ubuntu::application::sensors::sensor_type_pressure>* sl
        = new SensorListener<ubuntu::application::sensors::sensor_type_pressure>();

    sl->on_pressure_event = cb;
    sl->context = ctx;

    pressure_listener = sl;
    s->register_listener(pressure_listener);
}

UStatus
ua_sensors_pressure_set_event_rate(
    UASensorsPressure* sensor,
    uint32_t rate)
{
    if (sensor == NULL)
       return U_STATUS_ERROR;

    ALOGI("%s():%d", __PRETTY_FUNCTION__, __LINE__);
    auto s = static_cast<ubuntu::application::sensors::Sensor*>(sensor);
    if (!s->set_event_rate(rate))
        return U_STATUS_ERROR;

    return U_STATUS_SUCCESS;
}

uint64_t
uas_pressure_event_get_timestamp(
    UASPressureEvent* event)
{
    auto ev = static_cast<ubuntu::application::sensors::PressureEvent*>(event);
    return ev->get_timestamp();
}

UStatus
uas_pressure_event_get_pressure(
    UASPressureEvent* event,
    float* value)
{
    if (event == NULL || value == NULL)
        return U_STATUS_ERROR;

    auto ev = static_cast<ubuntu::application::sensors::PressureEvent*>(event);
    *value = ev->get_pressure();

    return U_STATUS_SUCCESS;
}
