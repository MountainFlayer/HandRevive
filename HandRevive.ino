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

#include <WiFi.h>
#include <ESP32Servo.h>

Servo indice;  // create servo object to control a servo
Servo corazon;
Servo anular;
Servo pulgar;
Servo menique;

// Servo GPIO pin
static const int indicePin = 13;
static const int corazonPin = 12;
static const int anularPin = 14;
static const int pulgarPin = 27;
static const int meniquePin = 26;

//GPIO pin for the muscular sensor
static const int EMGpin = 34;

// Value to save the values from the miostatine sensor
int analogValue = 0; //Variable donde guardamos el valor que nos da el sensor
int minActiveValue = 0; //Variable donde guardaremos el valor minimo del sensor cuando activamos el musculo
int maxActiveValue = 0; //Variable donde guardaremos el valor maximo del sensor cuando activamos el musculo
int minInactiveValue = 0; //Variable donde guardaremos el valor minimo del sensor cuando no activamos el musculo
int maxInactiveValue = 0; //Variable donde guardaremos el valor maximo del sensor cuando no activamos el musculo

// Network credentials
const char* ssid     = "XXXXXXX";
const char* password = "XXXXXXX";

// Web server on port 80 (http)
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Decode HTTP GET value
String valueString = String(5);
int pos1 = 0;
int pos2 = 0;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

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

void abrirMano() {
  // Move servo into position
  indice.write(0);
  corazon.write(0);
  anular.write(0);
  pulgar.write(0);
  menique.write(0);
}

void cerrarMano() {
  // Move servo into position
  indice.write(180);
  corazon.write(180);
  anular.write(180);
  pulgar.write(180);
  menique.write(180);
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
  menique.setPeriodHertz(50);
  
  // Attach to servo and define minimum and maximum positions
  // Modify as required
  indice.attach(indicePin,500, 2400);
  corazon.attach(corazonPin,500, 2400);
  anular.attach(anularPin,500, 2400);
  pulgar.attach(pulgarPin,500, 2400);
  menique.attach(meniquePin,500, 2400);

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

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  //Read the value from the sensor
  int analogValue = analogRead(EMGpin);
  // print out the values you read:
  Serial.printf("%d\n", analogValue);

  delay(100);

  // Listen for incoming clients
  WiFiClient client = server.available();   

  // Client Connected
  if (client) {                             
    // Set timer references
    currentTime = millis();
    previousTime = currentTime;
    
    // Print to serial port
    Serial.println("New Client."); 
    
    // String to hold data from client
    String currentLine = ""; 
    
   // Do while client is connected
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c); // print it out to the serial monitor
        header += c;
        if (c == '\n') { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK) and a content-type
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html; charset=utf-8");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page

            // HTML Header
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<meta charset=\"UTF-8\">"); // Establecer la codificación UTF-8
            client.println("<link rel=\"icon\" href=\"data:,\">");

            // CSS - Modify as desired
            client.println("<style>body { text-align: center; font-family: \"Helvetica Neue\", Helvetica, Arial, sans-serif; background-color: #f2f2f2; margin: 0; padding: 20px; }");
            client.println(".switch-container { display: flex; flex-direction: column; align-items: center; margin-top: 20px; }");
            client.println(".switch { display: inline-block; vertical-align: top; width: 80px; height: 34px; position: relative; border-radius: 34px; background-color: #e6e6e6; margin-bottom: 10px; }");
            client.println(".switch input { display: none; }");
            client.println(".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #fff; transition: .4s; border-radius: 34px; }");
            client.println(".slider:before { position: absolute; content: ''; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: #fff; transition: .4s; border-radius: 50%; }");
            client.println(".switch-label { position: absolute; top: -25px; width: 100%; text-align: center; font-size: 12px; color: #333; }");
            client.println("input:checked + .slider { background-color: #70cfff; }");
            client.println("input:checked + .slider:before { transform: translateX(46px); }");
            client.println("</style>");

            // Get JQuery
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script>");

            // Page title
            client.println("</head><body><h1 style=\"color:#ff3410;\">Servo Control</h1>");

            // Switch controls
            client.println("<h2 style=\"color:#333;\">Control de dedos:</h2>");
            client.println("<div class=\"switch-container\">");
            client.println("<label class=\"switch\" style=\"text-align: left; margin-bottom: 40px;\"><input type=\"checkbox\" onchange=\"toggleMotor(this, 'pulgar')\"><span class=\"slider\"></span><span class=\"switch-label\">Pulgar</span></label>");
            client.println("<label class=\"switch\" style=\"text-align: left; margin-bottom: 40px;\"><input type=\"checkbox\" onchange=\"toggleMotor(this, 'indice')\"><span class=\"slider\"></span><span class=\"switch-label\">Indice</span></label>");
            client.println("<label class=\"switch\" style=\"text-align: left; margin-bottom: 40px;\"><input type=\"checkbox\" onchange=\"toggleMotor(this, 'corazon')\"><span class=\"slider\"></span><span class=\"switch-label\">Corazon</span></label>");
            client.println("<label class=\"switch\" style=\"text-align: left; margin-bottom: 40px;\"><input type=\"checkbox\" onchange=\"toggleMotor(this, 'anular')\"><span class=\"slider\"></span><span class=\"switch-label\">Anular</span></label>");
            client.println("<label class=\"switch\" style=\"text-align: left; margin-bottom: 40px;\"><input type=\"checkbox\" onchange=\"toggleMotor(this, 'menique')\"><span class=\"slider\"></span><span class=\"switch-label\">Meñique</span></label>");
            client.println("<label class=\"switch\" style=\"text-align: left; margin-bottom: 40px;\"><input type=\"checkbox\" onchange=\"toggleMotor(this, 'all')\"><span class=\"slider\"></span><span class=\"switch-label\">Abrir/Cerrar mano</span></label>");

            client.println("</div>");

            // JavaScript
            client.println("<script>");
            client.println("function toggleMotor(checkbox, motor) {");
            client.println("var pos = checkbox.checked ? 180 : 0;");
            client.println("$.get(\"/?motor=\" + motor + \"&pos=\" + pos + \"&\");");
            client.println("}");
            client.println("$.ajaxSetup({timeout:1000});");
            client.println("</script>");

            // End page
            client.println("</body></html>");

            // GET data
            if (header.indexOf("GET /?motor=") >= 0) {
              int motorPosIndex = header.indexOf("pos=");
              if (motorPosIndex >= 0) {
                String motor = header.substring(header.indexOf('=') + 1, motorPosIndex - 1);
                //String pos = header.substring(motorPosIndex + 4, header.indexOf('&'));
                String pos = header.substring(motorPosIndex + 4);
                pos = pos.substring(0, pos.indexOf('&'));
                // Perform the necessary actions based on motor and position
                if (motor == "pulgar") {
                  // Code to control pulgar with pos value
                  pulgar.write(pos.toInt());
                }
                if (motor == "indice") {
                  // Code to control indice with pos value
                  indice.write(pos.toInt());
                }
                if (motor == "corazon") {
                  // Code to control corazon with pos value (corazon)
                  corazon.write(pos.toInt());
                }
                if (motor == "anular") {
                  // Code to control anular with pos value (anular)
                  anular.write(pos.toInt());
                }
                if (motor == "menique") {
                  // Code to control menique with pos value (meñique)
                  menique.write(pos.toInt());
                }
                if (motor == "all") {
                  // Code to open/close the hand
                  if (pos.toInt() == 180){
                    cerrarMano();
                  } else {
                    abrirMano();
                  }
                }
              }
            }

            // The HTTP response ends with another blank line
            client.println();

            // Break out of the while loop
            break;

          } else {
            // New line is received, clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");

  }

  //EMG control code
  if(maxActiveValue > 500){ //EMG connected and properly calibrated
    if(analogValue == maxActiveValue) {
      abrirMano();
    }

    if(analogValue <= (((minInactiveValue+maxInactiveValue)/2) + 10) && (analogValue != 0)) {
      cerrarMano();
    }

    delay(100);
  }

}