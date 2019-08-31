
#include "functions.h"

void setup() {
  Serial.begin(9600);
  connect_to_wifi();
  String token = sendAuth("hello@davidinformatico.com", "12345");
  getTasks(token);
}

void loop() {
  // put your main code here, to run repeatedly:

}
