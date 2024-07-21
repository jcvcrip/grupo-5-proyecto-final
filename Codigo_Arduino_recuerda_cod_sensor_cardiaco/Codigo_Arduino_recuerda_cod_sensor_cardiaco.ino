#include <PulseSensorPlayground.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuración de la pantalla LCD I2C
LiquidCrystal_I2C lcd(0x27, 16, 2); // Dirección I2C 0x27, 16 columnas y 2 filas

// Configuración del sensor de pulso
const int PulseWire = A0;       // Cable PÚRPURA del PulseSensor conectado al PIN ANALÓGICO 0
const int ButtonPin = 2;        // Pin donde está conectado el botón
const int LED13 = 13;           // El LED incorporado en la placa Arduino, cerca del PIN 13
int Threshold = 550;            // Determinar qué señal se "contará como un latido" y cuál se ignorará.

PulseSensorPlayground pulseSensor;  // Crea una instancia del objeto PulseSensorPlayground llamado "pulseSensor"

void setup() {   
  pinMode(ButtonPin, INPUT_PULLUP);  // Configuración del botón como entrada con pull-up interno

  Serial.begin(9600);  // Para el Monitor Serial

  // Inicializa el generador de números aleatorios con un valor basado en el tiempo actual
  randomSeed(analogRead(0));

  // Configura el objeto PulseSensor asignando nuestras variables a él.
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED13);  // Parpadea automáticamente el LED de Arduino con el latido cardíaco.
  pulseSensor.setThreshold(Threshold);   

  // Verifica que el objeto "pulseSensor" se haya creado y haya comenzado a detectar una señal.
  if (pulseSensor.begin()) {
    Serial.println("SENSOR DE PULSO CARDIACO <3");  // Esto se imprime una vez al encender o reiniciar el Arduino.
  }

  // Inicializa la pantalla LCD I2C
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("En espera...");
}

void loop() {
  if (digitalRead(ButtonPin) == LOW) {  // Si se presiona el botón
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Midiendo... <3");

    long startTime = millis();
    int finalBPM = 0;
    String status = "";

    while (millis() - startTime < 7000) {  // 10 segundos
      pulseSensor.sawNewSample();
      int bpm = pulseSensor.getBeatsPerMinute();

      if (pulseSensor.sawStartOfBeat()) {  // Prueba constantemente si "ocurrió un latido".
        Serial.println("♥ Tu corazón esta latiendo");
        Serial.print("BPM: ");
        Serial.println(bpm);
        finalBPM = bpm;  // Actualizar el BPM final

        // Muestra los datos en la pantalla LCD I2C en tiempo real
        lcd.setCursor(0, 1);
        lcd.print("Pulso: ");
        lcd.print(bpm);
        lcd.print(" BPM  ");
      }

      delay(10);  // Pequeña pausa para estabilidad
    }

    // Generar un código aleatorio
    char randomCode[4];
    randomCode[0] = 'A' + random(26);  // Generar una letra aleatoria
    randomCode[1] = '0' + random(10);  // Generar un número aleatorio
    randomCode[2] = '0' + random(10);  // Generar un número aleatorio
    randomCode[3] = '\0';  // Terminar la cadena

    // Determinar el estado basado en el BPM final
    if (finalBPM >= 60 && finalBPM <= 100) {
      status = "normal";
    } else if (finalBPM > 100) {
      status = "taquicardia";
    } else if (finalBPM < 60) {
      status = "bradicardia";
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Cod:");
    lcd.print(randomCode);
    lcd.setCursor(0, 1);
    lcd.print("Pulso: ");
    lcd.print(finalBPM);
    lcd.print(" BPM ");
    lcd.setCursor(10, 0);
    lcd.print(status);

    // Enviar el código aleatorio, el BPM final y el estado por el puerto serial
    Serial.print("Code: ");
    Serial.println(randomCode);
    Serial.print("Final BPM: ");
    Serial.println(finalBPM);
    Serial.print("Status: ");
    Serial.println(status);

    delay(7000);  // Mantener el dato final en la pantalla por 7 segundos
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("En espera...");
  }
}
