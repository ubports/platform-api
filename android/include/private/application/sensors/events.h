/*
 * Copyright © 2013 Canonical Ltd.
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

#ifndef UBUNTU_APPLICATION_SENSORS_EVENTS_H_
#define UBUNTU_APPLICATION_SENSORS_EVENTS_H

#include <private/platform/shared_ptr.h>
#include <utils/Log.h>

namespace ubuntu
{
namespace application
{
namespace sensors
{
class OrientationEvent
{
public:
    OrientationEvent(uint64_t timestamp, float azimuth, float pitch, float roll)
        : timestamp(timestamp),
          azimuth(azimuth),
          pitch(pitch),
          roll(roll)
    {}

    uint64_t get_timestamp() const
    {
        return this->timestamp;
    }

    float get_azimuth() const { return this->azimuth; }
    float get_pitch() const { return this->pitch; }
    float get_roll() const { return this->roll; }

private:
    uint64_t timestamp;
    float azimuth;
    float pitch;
    float roll;

protected:
    OrientationEvent(const OrientationEvent&) = delete;
    OrientationEvent& operator=(const OrientationEvent&) = delete;
};

class AccelerometerEvent
{
public:
    AccelerometerEvent(uint64_t timestamp, float x, float y, float z)
        : timestamp(timestamp),
          x(x),
          y(y),
          z(z)
    {}

    uint64_t get_timestamp() const
    {
        return this->timestamp;
    }

    float get_x() const { return this->x; }
    float get_y() const { return this->y; }
    float get_z() const { return this->z; }

private:
    uint64_t timestamp;
    float x;
    float y;
    float z;

protected:
    AccelerometerEvent(const AccelerometerEvent&) = delete;
    AccelerometerEvent& operator=(const AccelerometerEvent&) = delete;
};

class ProximityEvent
{
public:
    ProximityEvent(uint64_t timestamp, float distance) : timestamp(timestamp),
                                                         distance(distance)
    {}

    uint64_t get_timestamp() const
    {
        return this->timestamp;
    }

    float get_distance() const { return this->distance; }

private:
    uint64_t timestamp;
    float distance;

protected:
    ProximityEvent(const ProximityEvent&) = delete;
    ProximityEvent& operator=(const ProximityEvent&) = delete;
};

class LightEvent
{
public:
    LightEvent(uint64_t timestamp, float light) : timestamp(timestamp),
                                                  light(light)
    {}

    uint64_t get_timestamp()
    {
        return this->timestamp;
    }

    float get_light() { return this->light; }

private:
    uint64_t timestamp;
    float light;

protected:
    LightEvent(const LightEvent&) = delete;
    LightEvent& operator=(const LightEvent&) = delete;
};

class GyroscopeEvent
{
public:
    GyroscopeEvent(uint64_t timestamp, float x_rate, float y_rate, float z_rate)
        : timestamp(timestamp),
          x_rate(x_rate),
          y_rate(y_rate),
          z_rate(z_rate)
    {}

    uint64_t get_timestamp() const
    {
        return this->timestamp;
    }

    float get_x_rotation_rate() const { return this->x_rate; }
    float get_y_rotation_rate() const { return this->y_rate; }
    float get_z_rotation_rate() const { return this->z_rate; }

private:
    uint64_t timestamp;
    float x_rate;
    float y_rate;
    float z_rate;

protected:
    GyroscopeEvent(const GyroscopeEvent&) = delete;
    GyroscopeEvent& operator=(const GyroscopeEvent&) = delete;
};

class MagneticEvent
{
public:
    MagneticEvent(uint64_t timestamp, float x, float y, float z)
        : timestamp(timestamp),
          x(x),
          y(y),
          z(z)
    {}

    uint64_t get_timestamp() const
    {
        return this->timestamp;
    }

    float get_x() const { return this->x; }
    float get_y() const { return this->y; }
    float get_z() const { return this->z; }

private:
    uint64_t timestamp;
    float x;
    float y;
    float z;

protected:
    MagneticEvent(const MagneticEvent&) = delete;
    MagneticEvent& operator=(const MagneticEvent&) = delete;
};

class TemperatureEvent
{
public:
    TemperatureEvent(uint64_t timestamp, float temperature) : timestamp(timestamp),
                                                  temperature(temperature)
    {}

    uint64_t get_timestamp()
    {
        return this->timestamp;
    }

    float get_temperature() { return this->temperature; }

private:
    uint64_t timestamp;
    float temperature;

protected:
    TemperatureEvent(const TemperatureEvent&) = delete;
    TemperatureEvent& operator=(const TemperatureEvent&) = delete;
};

class PressureEvent
{
public:
    PressureEvent(uint64_t timestamp, float pressure) : timestamp(timestamp),
                                                  pressure(pressure)
    {}

    uint64_t get_timestamp()
    {
        return this->timestamp;
    }

    float get_pressure() { return this->pressure; }

private:
    uint64_t timestamp;
    float pressure;

protected:
    PressureEvent(const PressureEvent&) = delete;
    PressureEvent& operator=(const PressureEvent&) = delete;
};

}
}
}

#endif /* UBUNTU_APPLICATION_SENSORS_EVENTS_H_ */
