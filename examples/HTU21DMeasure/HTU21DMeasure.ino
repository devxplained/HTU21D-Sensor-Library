#include <HTU21D.h>

HTU21D sensor;

void setup() {
  Serial.begin(9600);
  sensor.begin();
}

void loop() {
  if(sensor.measure()) {
    float temperature = sensor.getTemperature();
    float humidity = sensor.getHumidity();
    
    Serial.print("Temperature (°C): ");
    Serial.println(temperature);
    
    Serial.print("Humidity (%RH): ");
    Serial.println(humidity);
  }

  delay(5000);
}
