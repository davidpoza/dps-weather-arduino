
#include "Arduino.h"
#include "Wire.h"
#include "uRTCLib.h"

uRTCLib rtc;
uint8_t position;

void setup() {
  delay (2000);
  Serial.begin(9600);
  Serial.println("Serial OK");
  //  Max position: 32767

  #ifdef ARDUINO_ARCH_ESP8266
    Wire.begin(0, 2); // D3 and D4 on ESP8266
  #else
    Wire.begin();
  #endif
  rtc.set_rtc_address(0x68);
  rtc.set_model(URTCLIB_MODEL_DS3232);

  // Only used once, then disabled
  rtc.set(0, 42, 16, 1, 2, 5, 15);
  //  RTCLib::set(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year)

  Serial.print("Lost power status: ");
  if (rtc.lostPower()) {
    Serial.print("POWER FAILED. Clearing flag...");
    rtc.lostPowerClear();
    Serial.println(" done.");
  } else {
    Serial.println("POWER OK");
  }
  rtc.alarmClearFlag(URTCLIB_ALARM_1);
  rtc.alarmClearFlag(URTCLIB_ALARM_2);
  rtc.alarmSet(URTCLIB_ALARM_TYPE_1_FIXED_S, 30, 0, 0, 1); // Each minute, at just :10 seconds
}

void loop() {
  rtc.refresh();
  Serial.print("RTC DateTime: ");

  Serial.print(rtc.year());
  Serial.print('/');
  Serial.print(rtc.month());
  Serial.print('/');
  Serial.print(rtc.day());

  Serial.print(' ');

  Serial.print(rtc.hour());
  Serial.print(':');
  Serial.print(rtc.minute());
  Serial.print(':');
  Serial.print(rtc.second());

  Serial.print(" DOW: ");
  Serial.print(rtc.dayOfWeek());

  Serial.println();
  delay(62000);
  rtc.alarmClearFlag(URTCLIB_ALARM_1);
  rtc.alarmClearFlag(URTCLIB_ALARM_2);
  rtc.alarmSet(URTCLIB_ALARM_TYPE_1_FIXED_S, 30, 0, 0, 1); // Each minute, at just :10 seconds
  // RTCLib::alarmSet(uint8_t type, uint8_t second, uint8_t minute, uint8_t hour, uint8_t day_dow);

}
