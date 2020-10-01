int pinAnalogico = A1;
int pinDigital = 3;
void setup() {
  pinMode(pinAnalogico, INPUT);
  pinMode(pinDigital, INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.print("Intensidad de campo magnético(Pin A1): ");
  Serial.print(analogRead(pinAnalogico) * (5.0 / 1023.0));

  Serial.println("V");
  Serial.print("Detectando campo magnético(Pin D7): ");
  
  if(digitalRead(pinDigital) == HIGH){
      Serial.println("Detectado");
  }else{
      Serial.println("No detectado");
  }
  Serial.println("----------------------------------------------------------------");
  delay(500);

}
