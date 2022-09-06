# HTU21D Sensor Library
This is an Arduino library for the HTU21D temperature and humidity sensor.

It allows to set the measurement resolution and implements the temperature correction formula and the CRC algorithm given in the manufactures datasheet.

For details on the sensor please refer to the datasheet.

This library was written by Daniel Wiese (DevXplained).
Redistribution is possible under the terms of the MIT license.

# References
Manufacturer Page: [https://www.te.com/deu-de/product-CAT-HSC0004.html](https://www.te.com/deu-de/product-CAT-HSC0004.html)

# Platform Specifics
For ESP32 boards the i2c communication pattern deviates from the datasheet.
A delay during the i2c communication is not possible on the ESP32. For this reason the data is queried in two operations.

If your target platform has i2c communication problems and requires similar quirks please open an issue.
