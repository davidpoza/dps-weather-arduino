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

int pinDigital = 7;
float rpm;
int tiempo_medicion_inicio = 0;
int tiempo_medicion_fin = 0;
int duracion_medicion = 0;
int duracion_medicion_max = 10; /*en segundos*/
float velocidad_maxima = 0; //guarda la velocidad maxima registra en el periodo de tiempo indicado por "duracion_medicion_max"

volatile unsigned long tiempo_anterior;
volatile int tiempo_pulso;

float transformacionLineal(int rpm){
  if(rpm>0)
    return(0.036*rpm+3.258); // de momento me la invento
  else
    return(0);
}

void setup() {
  rpm = 0;
  tiempo_pulso = 0;  
  tiempo_anterior = millis();  
  attachInterrupt(pinDigital, acumulador, FALLING);
  Serial.begin(9600);

}

void loop() {
  int temp = 0;
  duracion_medicion = tiempo_medicion_fin - tiempo_medicion_inicio;
  delay(2); //tan solo para evitar que coincidan inicio y fin
  tiempo_medicion_fin = millis();
  
  if(duracion_medicion > (duracion_medicion_max*1000)) {
    //en este bloque tomamos la medida y la enviamos al servidor y la reseteamos a 0 para el siguiente ciclo
    Serial.println("==== CICLO MEDICION COMPLETADO =====");
    Serial.print("velocidad maxima: ");
    Serial.println(velocidad_maxima);
    tiempo_medicion_inicio = millis();
    duracion_medicion = 0;
    velocidad_maxima = 0;
  }
  
  
  delay(500); /*para poder visualizar los datos*/
  
  detachInterrupt(0);
  //------- no queremos que se interrumpa el cálculo
  
  rpm = 1000*60  / (tiempo_pulso*3) ; /*pasamos de ms por pulso a vueltas por minuto*/

  Serial.print("tiempo pulso: ");
  Serial.println(tiempo_pulso);
  Serial.print("velocidad: ");
  Serial.println(transformacionLineal(rpm));
  Serial.print("rpm: ");
  Serial.println(rpm);
  temp = transformacionLineal(rpm);
  if(temp>0 && temp>velocidad_maxima){
    velocidad_maxima = temp; 
  }
  tiempo_pulso = 0;
  
  //-------
  attachInterrupt(pinDigital, acumulador, FALLING);
}

void acumulador(){
    tiempo_pulso = millis() - tiempo_anterior;
    tiempo_anterior = millis();
}
