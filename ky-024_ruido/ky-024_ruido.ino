float lectura;

void setup() {  
  Serial.begin(9600);
  pinMode(7, INPUT);
}

void loop() {

  lectura = analogRead(6);
  Serial.println(lectura);
}
