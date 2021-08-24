set(
  UBUNTU_APPLICATION_SENSORS_HEADERS
  accelerometer.h
  gyroscope.h
  light.h
  magnetic.h
  proximity.h
  haptic.h
  orientation.h
  temperature.h
  pressure.h
)

install(
  FILES ${UBUNTU_APPLICATION_SENSORS_HEADERS}
  DESTINATION include/ubuntu/application/sensors
)

add_subdirectory(event)
