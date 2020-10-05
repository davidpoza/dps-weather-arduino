/*
 * Medición de velocidad del viento con anemómetro casero usando interrupciones.
 * Autor: David Poza Suárez
 * Fecha: 26 Julio 2019
 * Version 1.0.0
 * 
 * Cambios
 * -------
 * Fecha: 31 Agosto 2019
 * Version: 1.0.1
 * Cambios: Se añade un bloque de tiempo en el cual se irán registrando todas las mediciones
 * y se almacena la máxima, que se envia y se resetea cada vez que se cumple el intervalo de 
 * tiempo configurado en la variable *duracion_medicion_maxima*.
 * De esta forma se logran mediciones mas reales, si el viento no es constante.
 *--------------
 * 
 * 
 * 
 * El color de los pines del sensor hall:
 * rojo: DO
 * blanco: GND
 * marron: 3.3V
 */

int digitalPin = 3;
int rpm;
float measurements[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int mIndex = 0; // current measurement index in array
volatile int ticks;
unsigned long time;

/**
 * Calculates average wind speed using last ten measurements
 */
float averageWindSpeed(float measurements[10]) {
  float acc = 0;
  for(int i=0; i<10; i++) {
    acc += measurements[i];
  }
  return acc / 10;
}

/**
 * Transforms ticks per second to Km/h
 */
float windLinearTransformation(float ticksSec){
  Serial.print("Ticks per sec = ");
  Serial.println(ticksSec);
  if(ticksSec>0)
    return(1.41*ticksSec+1.71);
  else
    return(0);
}


void setup() {
  rpm = 0;
  ticks = 0;
  time = millis();
  attachInterrupt(digitalPinToInterrupt(digitalPin), acumulador, FALLING);
  Serial.begin(9600);

}

void loop() {
  delay(3000); /*para poder hacer la media*/
  
  detachInterrupt(digitalPinToInterrupt(digitalPin));
  //------- no queremos que se interrumpa el cálculo
  
  rpm = ticks*1000*60 / (3000*3);
  measurements[mIndex] = windLinearTransformation(ticks * 1000 / (millis() - time));
  
  
  Serial.print("current wind speed km/h: ");
  Serial.println(measurements[mIndex]);
  Serial.print("rpm: ");
  Serial.println(rpm);
  Serial.print("ticks por cycle: ");
  Serial.println(ticks);
  Serial.print("average wind speed km/h: ");
  Serial.println(averageWindSpeed(measurements));
  time = millis();
  mIndex = (mIndex + 1) % 10;
  ticks = 0;
  //-------
  attachInterrupt(digitalPinToInterrupt(digitalPin), acumulador, FALLING);
}

void acumulador(){
  ticks++;
}
