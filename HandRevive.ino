/*
  ESP32 Remote WiFi Servo Control
  esp32-web-servo.ino
  Control servo motor from web page
  
  Based upon example from Rui Santos
  Random Nerd Tutorials
  https://randomnerdtutorials.com/esp32-servo-motor-web-server-arduino-ide/
  

  DroneBot Workshop 2020
  https://dronebotworkshop.com
*/

#include <ESP32Servo.h>

Servo indice;  // create servo object to control a servo
Servo corazon;
Servo anular;
Servo pulgar;

// Servo GPIO pin
static const int indicePin = 13;
static const int corazonPin = 12;
static const int anularPin = 14;
static const int pulgarPin = 27;

//GPIO pin for the muscular sensor
static const int EMGpin = 26;

int analogValue = 0; //Variable donde guardamos el valor que nos da el sensor
int minActiveValue = 0; //Variable donde guardaremos el valor minimo del sensor cuando activamos el musculo
int maxActiveValue = 0; //Variable donde guardaremos el valor maximo del sensor cuando activamos el musculo
int minInactiveValue = 0; //Variable donde guardaremos el valor minimo del sensor cuando no activamos el musculo
int maxInactiveValue = 0; //Variable donde guardaremos el valor maximo del sensor cuando no activamos el musculo

void abrirMano() {
  // Move servo into position
  indice.write(5);
  corazon.write(5);
  anular.write(5);
  pulgar.write(5);
}

void cerrarMano() {
  // Move servo into position
  indice.write(100);
  corazon.write(100);
  anular.write(100);
  pulgar.write(100);
}

void leerMaxMinValues(int* maxValue, int* minValue) {
  unsigned long tiempo1 = millis();
  unsigned long tiempo2 = millis();
  while (tiempo2 < (tiempo1+5000)) {
    tiempo2 = millis();
    //Serial.println(tiempo2/1000);
    analogValue = analogRead(EMGpin);
    if (*maxValue < analogValue){ //Si el valor leido es mayor que el valor maximo lo guardamos
      *maxValue = analogValue;
    }
    if (*minValue > analogValue){ //Si el valor leido es menor que el valor minimo lo guardamos
      *minValue = analogValue;
    }
  }
  return;
}

void setup() {
  
  // Allow allocation of all timers for servo library
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  
  // Set servo PWM frequency to 50Hz
  indice.setPeriodHertz(50);
  corazon.setPeriodHertz(50);
  anular.setPeriodHertz(50);
  pulgar.setPeriodHertz(50);
  
  // Attach to servo and define minimum and maximum positions
  // Modify as required
  indice.attach(indicePin,500, 2400);
  corazon.attach(corazonPin,500, 2400);
  anular.attach(anularPin,500, 2400);
  pulgar.attach(pulgarPin,500, 2400);

  // Start serial
  Serial.begin(115200);

  //set the resolution to 12 bits (0-4096)
  analogReadResolution(12);

  //Inicializamos el pin para el LED
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH); //Encendemos el LED para saber cuando estamos calibrando el sensor

  Serial.println("Asegurate de ponerte bien los electrodos.");
  Serial.println("Manten el musculo apretado durante 5 segundos.");
  delay(3000);
  minActiveValue = 8000; //Ponemos un valor muy alto en esta variable para guardar aquí el valor mas bajo
  leerMaxMinValues(&maxActiveValue, &minActiveValue);
  Serial.printf("Valor maximo: %d\nValor minimo: %d\n", maxActiveValue, minActiveValue);

  digitalWrite(2, LOW); //Apagamos el LED para saber cuando estamos calibrando el sensor

  Serial.println("Manten el musculo relajado durante 5 segundos.");
  delay(3000);
  minInactiveValue = 8000; //Ponemos un valor muy alto en esta variable para guardar aquí el valor mas bajo
  leerMaxMinValues(&maxInactiveValue, &minInactiveValue);
  Serial.printf("Valor maximo: %d\nValor minimo: %d\n", maxInactiveValue, minInactiveValue);

  digitalWrite(2, HIGH); //Encendemos el LED para saber cuando estamos calibrando el sensor
  delay(2000);
  digitalWrite(2, LOW); //Apagamos el LED para saber cuando estamos calibrando el sensor

}

void loop(){
  //Read the value from the sensor
  analogValue = analogRead(EMGpin);
  // print out the values you read:
  Serial.printf("%d\n", analogValue);
  
  if(analogValue == maxActiveValue) {
    cerrarMano();
  }

  if(analogValue <= minInactiveValue + 10) {
    abrirMano();
  }

  delay(100);

}