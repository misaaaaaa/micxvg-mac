#include <Arduino.h>
#include <LEDFader.h> // Include the LEDFader library for PWM control

// Pines de los LEDs
int ledPins[] = {2, 3, 4, 5};

// para mapear comportamiento medido por sensor de temperatura
int comportamiento = 0;
int temperatureSensorPin = A1; // Pin analógico para el sensor de temperatura
int Temperature = 0;

// Variables para almacenar la temperatura
int rawTemperature = 0;
unsigned long previousMillis = 0; // Store the last time the temperature was read
unsigned long currentMillis = 0;  // Store the current time
unsigned long interval = 1000;    // Interval at which to read the temperature

// Tiempos de encendido y apagado (ms) para cada LED
int onTime1 = 510, offTime1 = 520;
int onTime2 = 530, offTime2 = 540;
int onTime3 = 550, offTime3 = 560;
int onTime4 = 570, offTime4 = 580;

// Estados y marcas de tiempo para cada LED
int ledStates[4] = {LOW, LOW, LOW, LOW};
unsigned long prevMillis[4] = {0, 0, 0, 0};

void setup()
{
    Serial.begin(9600);
    for (int i = 0; i < 4; i++)
    {
        pinMode(ledPins[i], OUTPUT);
    }
}
// Función genérica para controlar un LED usando arrays
void controlarLed(int pin, unsigned long onT, unsigned long offT,
                  int ledIndex)
{

    if (ledStates[ledIndex] == HIGH && (currentMillis - prevMillis[ledIndex] >= onT))
    {
        ledStates[ledIndex] = LOW;
        prevMillis[ledIndex] = currentMillis;
        digitalWrite(pin, ledStates[ledIndex]);
    }
    else if (ledStates[ledIndex] == LOW && (currentMillis - prevMillis[ledIndex] >= offT))
    {
        ledStates[ledIndex] = HIGH;
        prevMillis[ledIndex] = currentMillis;
        digitalWrite(pin, ledStates[ledIndex]);
    }
}

void tReading()
{

    rawTemperature = analogRead(temperatureSensorPin); // Read the temperature sensor value

    // int Temperature = map(rawTemperature, 1023, 0, -40, 125); // Map the raw value to a temperature range

}

// funcion para decir que hace que cambie el comportamiento
void controlarComportamiento()
{
    if (currentMillis - previousMillis >= interval)
    {
        // comportamiento = (rawTemperature / 4) % 2;
        if (rawTemperature < 512)
        {
            comportamiento = 0; // Comportamiento 0 para temperaturas bajas
        }
        else
        {
            comportamiento = 1; // Comportamiento 1 para temperaturas altas
        }
        Serial.print("Raw Temperature: ");
        Serial.print(rawTemperature);
        Serial.print(" | Comportamiento: ");
        Serial.print(comportamiento);
        Serial.println();
        previousMillis = currentMillis; // Update previousMillis here
    }
}

void loop()
{
    currentMillis = millis();
    tReading();
    controlarComportamiento();

    // funcion para controlar encendidos y apagados de LEDS
    //  controlarLed(ledPins[0], onTime1, offTime1, 0);
    //  controlarLed(ledPins[1], onTime2, offTime2, 1);
    //  controlarLed(ledPins[2], onTime3, offTime3, 2);
    //  controlarLed(ledPins[3], onTime4, offTime4, 3);

    switch (comportamiento)
    {
    case 0:
        // Acción para comportamiento 0
        controlarLed(ledPins[0], onTime1, offTime1, 0);
        controlarLed(ledPins[1], onTime2, offTime2, 1);
        controlarLed(ledPins[2], onTime3, offTime3, 2);
        controlarLed(ledPins[3], onTime4, offTime4, 3);
        break;
    case 1:
        // Acción para comportamiento 1
        controlarLed(ledPins[0], onTime1 * 2, offTime1 * 2, 0);
        controlarLed(ledPins[1], onTime2 * 2, offTime2 * 2, 1);
        controlarLed(ledPins[2], onTime3 * 2, offTime3 * 2, 2);
        controlarLed(ledPins[3], onTime4 * 2, offTime4 * 2, 3);
        break;
    default:
        // Acción para otros valores (por si acaso)
        Serial.println("Comportamiento desconocido.");
        break;
    }

    // Serial.print("Comportamiento: ");
    // Serial.print(comportamiento);
    // Serial.print(" | sensorTemperatura: ");
    // Serial.print(rawTemperature);
    // Serial.println();
}
