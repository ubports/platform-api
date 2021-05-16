set(
  UBUNTU_APPLICATION_SENSORS_EVENT_HEADERS
  accelerometer.h
  gyroscope.h
  light.h
  magnetic.h
  proximity.h
  orientation.h
  temperature.h
  pressure.h
)

install(
  FILES ${UBUNTU_APPLICATION_SENSORS_EVENT_HEADERS}
  DESTINATION include/ubuntu/application/sensors/event
)
