#include <ArduinoLowPower.h> //require tambien instalar la lib RTCZero
// PARA RESETEAR EL ARDUINO HAY QUE PULSAR DOS VECES EL BOTON RESET Y SE CARGA UN PROGRAMA FADE

void setup() {
  Serial.begin(9600);  
  //LowPower.attachInterruptWakeup(RTC_ALARM_WAKEUP, wakeup, CHANGE);
}

void loop() { 
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
  // Triggers a 2000 ms sleep (the device will be woken up only by the registered wakeup sources and by internal RTC)
  // The power consumption of the chip will drop consistently
  LowPower.sleep(2000);
}


void wakeup() {
  Serial.println("Ya estoy despierto!");
}
