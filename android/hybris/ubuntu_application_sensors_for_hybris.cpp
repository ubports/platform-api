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
 */
#include "event_loop.h"

#include <private/application/sensors/sensor_service.h>
#include <private/application/sensors/sensor_listener.h>
#include <private/application/sensors/sensor_reading.h>
#include <private/application/sensors/sensor_type.h>
#include <private/application/sensors/sensor.h>

#include <android/sensor.h>

#include <gui/Sensor.h>
#include <gui/SensorManager.h>
#include <utils/KeyedVector.h>
#include <utils/List.h>

namespace ubuntu
{
namespace application
{
namespace sensors
{
namespace hybris
{

typedef android::KeyedVector<
ubuntu::application::sensors::SensorType,
       uint32_t> ForwardSensorTypeLut;

typedef android::KeyedVector<
uint32_t,
ubuntu::application::sensors::SensorType> BackwardSensorTypeLut;

static ForwardSensorTypeLut init_forward_sensor_type_lut()
{
    static ForwardSensorTypeLut lut;

    lut.add(sensor_type_accelerometer, SENSOR_TYPE_ACCELEROMETER);
    lut.add(sensor_type_magnetic_field, SENSOR_TYPE_MAGNETIC_FIELD);
    lut.add(sensor_type_gyroscope, SENSOR_TYPE_GYROSCOPE);
    lut.add(sensor_type_light, SENSOR_TYPE_LIGHT);
    lut.add(sensor_type_proximity, SENSOR_TYPE_PROXIMITY);
    lut.add(sensor_type_orientation, SENSOR_TYPE_ORIENTATION);
    lut.add(sensor_type_linear_acceleration, SENSOR_TYPE_LINEAR_ACCELERATION);
    lut.add(sensor_type_rotation_vector, SENSOR_TYPE_ROTATION_VECTOR);
    lut.add(sensor_type_pressure, SENSOR_TYPE_PRESSURE);
    lut.add(sensor_type_temperature, SENSOR_TYPE_AMBIENT_TEMPERATURE);
    return lut;
}

static BackwardSensorTypeLut init_backward_sensor_type_lut()
{
    static BackwardSensorTypeLut lut;

    lut.add(SENSOR_TYPE_ACCELEROMETER, sensor_type_accelerometer);
    lut.add(SENSOR_TYPE_MAGNETIC_FIELD, sensor_type_magnetic_field);
    lut.add(SENSOR_TYPE_GYROSCOPE, sensor_type_gyroscope);
    lut.add(SENSOR_TYPE_LIGHT, sensor_type_light);
    lut.add(SENSOR_TYPE_PROXIMITY, sensor_type_proximity);
    lut.add(SENSOR_TYPE_ORIENTATION, sensor_type_orientation);
    lut.add(SENSOR_TYPE_LINEAR_ACCELERATION, sensor_type_linear_acceleration);
    lut.add(SENSOR_TYPE_ROTATION_VECTOR, sensor_type_rotation_vector);
    lut.add(SENSOR_TYPE_PRESSURE, sensor_type_pressure);
    lut.add(SENSOR_TYPE_AMBIENT_TEMPERATURE, sensor_type_temperature);

    return lut;
}

#if ANDROID_VERSION_MAJOR >= 7
static char* init_instance_package_name()
{
    const size_t MAX_PACKAGENAME_LEN = PATH_MAX;
    char* reallink = new char[MAX_PACKAGENAME_LEN];
    const ssize_t reallen = readlink("/proc/self/exe", reallink, MAX_PACKAGENAME_LEN);
    reallink[reallen - 1] = 0;
    return reallink;
}

static const char* instance_package_name = init_instance_package_name();
#endif

static const ForwardSensorTypeLut forward_sensor_type_lut = init_forward_sensor_type_lut();
static const BackwardSensorTypeLut backward_sensor_type_lut = init_backward_sensor_type_lut();


struct Sensor : public ubuntu::application::sensors::Sensor
{
    typedef ubuntu::platform::shared_ptr<Sensor> Ptr;

    Sensor(
        const android::Sensor* sensor,
        const android::sp<android::SensorEventQueue>& queue) : sensor(sensor),
        sensor_event_queue(queue)
    {
    };

    int32_t id()
    {
        return sensor->getHandle();
    }

    const char* name()
    {
        return sensor->getName().string();
    }

    const char* vendor()
    {
        return sensor->getVendor().string();
    }

    void register_listener(const SensorListener::Ptr& listener)
    {
        listeners.push_back(listener);
    }

    // Deprecated!
    const SensorListener::Ptr& registered_listener()
    {
        return *(listeners.begin());
    }

    const android::List<ubuntu::application::sensors::SensorListener::Ptr>& registered_listeners() const
    {
        return listeners;
    }

    int enable()
    {
        return sensor_event_queue->enableSensor(sensor);
    }

    int disable()
    {
        return sensor_event_queue->disableSensor(sensor);
    }

    SensorType type()
    {
        return backward_sensor_type_lut.valueFor(sensor->getType());
    }

    float min_value()
    {
        return sensor->getMinValue();
    }

    float max_value()
    {
        return sensor->getMaxValue();
    }

    float resolution()
    {
        return sensor->getResolution();
    }

    int32_t min_delay()
    {
        return sensor->getMinDelay();
    }

    float power_consumption()
    {
        return sensor->getPowerUsage();
    }

    int set_event_rate(uint32_t nsecs)
    {
        return sensor_event_queue->setEventRate(sensor, nsecs);
    }

    const android::Sensor* sensor;
    ubuntu::application::sensors::SensorListener::Ptr listener;
    android::List<ubuntu::application::sensors::SensorListener::Ptr> listeners;
    android::sp<android::SensorEventQueue> sensor_event_queue;
};

void print_vector(const ASensorVector& vec)
{
    printf("Status: %d \n", vec.status);
    printf("\t\t %f, %f, %f \n", vec.v[0], vec.v[1], vec.v[2]);
    printf("\t\t %f, %f, %f \n", vec.x, vec.y, vec.z);
    printf("\t\t %f, %f, %f \n", vec.azimuth, vec.pitch, vec.roll);
}

struct SensorService : public ubuntu::application::sensors::SensorService
{
    static int looper_callback(int receiveFd, int events, void* ctxt)
    {
        static const int success_and_continue = 1;
        static const int error_and_abort = 0;

        SensorService* thiz = static_cast<SensorService*>(ctxt);

        if (!thiz)
            return error_and_abort;

        if (thiz->sensor_event_queue->getFd() != receiveFd)
            return success_and_continue;

        static ASensorEvent event;
        if (1 != thiz->sensor_event_queue->read(&event, 1))
            return error_and_abort;

        ssize_t i = thiz->sensor_registry.indexOfKey(event.sensor);
        if (i < 0)
            return success_and_continue;

        Sensor::Ptr sensor = thiz->sensor_registry.valueAt(i);

        static ubuntu::application::sensors::SensorReading::Ptr reading(
            new ubuntu::application::sensors::SensorReading());

        reading->timestamp = event.timestamp;
        switch (event.type)
        {
        case SENSOR_TYPE_ACCELEROMETER:
            memcpy(
                reading->acceleration.v,
                event.acceleration.v,
                sizeof(reading->acceleration.v));
            break;
        case SENSOR_TYPE_MAGNETIC_FIELD:
            memcpy(
                reading->magnetic.v,
                event.magnetic.v,
                sizeof(reading->magnetic.v));
            break;
        case SENSOR_TYPE_GYROSCOPE:
                reading->gyroscopic.v[0] = event.data[0];
                reading->gyroscopic.v[1] = event.data[1];
                reading->gyroscopic.v[2] = event.data[2];
            break;
        case SENSOR_TYPE_LIGHT:
            reading->light = event.light;
            break;
        case SENSOR_TYPE_PROXIMITY:
            reading->distance = event.distance;
            break;
        case SENSOR_TYPE_ORIENTATION:
            reading->vector.v[0] = event.vector.azimuth;
            reading->vector.v[1] = event.vector.pitch;
            reading->vector.v[2] = event.vector.roll;
            break;
        case SENSOR_TYPE_LINEAR_ACCELERATION:
            memcpy(
                reading->acceleration.v,
                event.acceleration.v,
                sizeof(reading->acceleration.v));
            break;
        case SENSOR_TYPE_ROTATION_VECTOR:
            reading->vector.v[0] = event.data[0];
            reading->vector.v[1] = event.data[1];
            reading->vector.v[2] = event.data[2];
            break;
        case SENSOR_TYPE_PRESSURE:
            reading->pressure = event.pressure;
            break;
        case SENSOR_TYPE_AMBIENT_TEMPERATURE:
            reading->temperature = event.temperature;
            break;
        }

        // Call all of the registered listeners
        android::List<ubuntu::application::sensors::SensorListener::Ptr>::const_iterator it = sensor->registered_listeners().begin();
        while (it != sensor->registered_listeners().end())
        {
            (*it)->on_new_reading(reading);
            ++it;
        }

        return success_and_continue;
    }

    SensorService() :
#if ANDROID_VERSION_MAJOR >= 7
        sensor_event_queue(android::SensorManager::getInstanceForPackage(
                             android::String16(hybris::instance_package_name)).createEventQueue()),
#else
        sensor_event_queue(android::SensorManager::getInstance().createEventQueue()),
#endif
        looper(new android::Looper(false)),
        event_loop(new ubuntu::application::EventLoop(looper))
    {
        looper->addFd(
            sensor_event_queue->getFd(),
            0,
            ALOOPER_EVENT_INPUT,
            looper_callback,
            this);

#if ANDROID_VERSION_MAJOR >= 7
        event_loop->run(instance_package_name);
#else
        event_loop->run();
#endif
    }

    android::sp<android::SensorEventQueue> sensor_event_queue;
    android::sp<android::Looper> looper;
    android::sp<ubuntu::application::EventLoop> event_loop;
    android::KeyedVector<int32_t, Sensor::Ptr> sensor_registry;
};

ubuntu::platform::shared_ptr<SensorService> instance;
}

ubuntu::application::sensors::Sensor::Ptr ubuntu::application::sensors::SensorService::sensor_for_type(
    ubuntu::application::sensors::SensorType type)
{
    const android::Sensor* sensor =

#if ANDROID_VERSION_MAJOR >= 7
        android::SensorManager::getInstanceForPackage(
            android::String16(hybris::instance_package_name)).
            getDefaultSensor(hybris::forward_sensor_type_lut.valueFor(type));
#else
        android::SensorManager::getInstance().
            getDefaultSensor(hybris::forward_sensor_type_lut.valueFor(type));
#endif

    if (sensor == NULL)
        return Sensor::Ptr();

    if (hybris::instance == NULL)
        hybris::instance = new hybris::SensorService();

    hybris::Sensor::Ptr p(
        new hybris::Sensor(
            sensor,
            hybris::instance->sensor_event_queue));

    if (sensor)
        hybris::instance->sensor_registry.add(p->id(), p);

    return Sensor::Ptr(p.get());
}

}
}
}
