#include <Arduino.h>

// Pines de los Reles
int relePins[] = {2, 3, 4, 5};

// Pines de los LEDS
int ledPin1 = 9;
int ledPin2 = 10;

// inicializamos tiempos de subida y bajada leds
int tiempoSubida1 = 1;
int tiempoBajada1 = 10;
int tiempoSubida2 = 15;
int tiempoBajada2 = 5;

// para mapear comportamiento medido por sensor de temperatura
int comportamiento = 0;
int prevComportamiento = -1;
int temperatureSensorPin = A1; // Pin analógico para el sensor de temperatura
int Temperature = 0;
bool change = false; // para detectar si hubo un cambio para resetear reles y fades

// Variables para almacenar la temperatura
int rawTemperature = 0;

// variables ligadas al tiempo
unsigned long previousMillis = 0; // Store the last time the temperature was read
unsigned long currentMillis = 0;  // Store the current time
unsigned long interval = 1000;    // Interval at which to read the temperature

// Tiempos de encendido y apagado (ms) para cada RELE
int onTime1 = 510, offTime1 = 520;
int onTime2 = 530, offTime2 = 540;
int onTime3 = 550, offTime3 = 560;
int onTime4 = 570, offTime4 = 580;

// Estados y marcas de tiempo para cada LED
int releStates[4] = {LOW, LOW, LOW, LOW};
unsigned long prevMillis[4] = {0, 0, 0, 0};

void setup()
{
    Serial.begin(9600);
    // Declarar salidas
    for (int i = 0; i < 4; i++)
    {
        pinMode(relePins[i], OUTPUT);
    }

    pinMode(ledPin1, OUTPUT);
    pinMode(ledPin2, OUTPUT);
}

// Función genérica para controlar un RELE usando arrays
void controlarRele(int pin, unsigned long onT, unsigned long offT,
                   int releIndex)
{
    if (change == true)
    {
        releStates[releIndex] = LOW;
        prevMillis[releIndex] = currentMillis;
        digitalWrite(pin, releStates[releIndex]);
    }

    if (releStates[releIndex] == HIGH && (currentMillis - prevMillis[releIndex] >= onT))
    {
        releStates[releIndex] = LOW;
        prevMillis[releIndex] = currentMillis;
        digitalWrite(pin, releStates[releIndex]);
    }
    else if (releStates[releIndex] == LOW && (currentMillis - prevMillis[releIndex] >= offT))
    {
        releStates[releIndex] = HIGH;
        prevMillis[releIndex] = currentMillis;
        digitalWrite(pin, releStates[releIndex]);
    }
}

// ---------- FUNCIÓN DE FADE ----------
void fadeLed(int pin, int tiempoSubida, int tiempoBajada)
{
    static int brillo[20];             // soporta hasta 20 pines distintos
    static int paso[20];               // dirección del fade
    static unsigned long anterior[20]; // tiempo anterior

    // unsigned long ahora = millis();

    // Inicialización solo la primera vez
    if (paso[pin] == 0 || change == true)
    {
        brillo[pin] = 0;
        paso[pin] = 1;
        anterior[pin] = currentMillis;
    }

    // Escoger intervalo según dirección
    int intervalo = (paso[pin] > 0) ? tiempoSubida : tiempoBajada;

    if (currentMillis - anterior[pin] >= intervalo)
    {
        anterior[pin] = currentMillis;

        brillo[pin] += paso[pin];

        if (brillo[pin] <= 0)
        {
            brillo[pin] = 0;
            paso[pin] = 1; // subir
        }
        else if (brillo[pin] >= 255)
        {
            brillo[pin] = 255;
            paso[pin] = -1; // bajar
        }

        analogWrite(pin, brillo[pin]);
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

        change = false;
        previousMillis = currentMillis; // Update previousMillis here
    }
       
    if (comportamiento != prevComportamiento)
    {
        change = true;
        prevComportamiento = comportamiento;
    } else {
        change = false;
    }
}

void loop()
{
    currentMillis = millis();
    tReading();
    controlarComportamiento();

    // funcion para controlar encendidos y apagados de LEDS
    //  controlarRele(relePins[0], onTime1, offTime1, 0);
    //  controlarRele(relePins[1], onTime2, offTime2, 1);
    //  controlarRele(relePins[2], onTime3, offTime3, 2);
    //  controlarRele(relePins[3], onTime4, offTime4, 3);

    switch (comportamiento)
    {
    case 0:
        // Acción para comportamiento 0
        onTime1 = 510, offTime1 = 520;
        onTime2 = 530, offTime2 = 540;
        onTime3 = 550, offTime3 = 560;
        onTime4 = 570, offTime4 = 580;

        tiempoSubida1 = 0;
        tiempoBajada1 = 4;
        tiempoSubida2 = 4;
        tiempoBajada2 = 0;

        break;
    case 1:
        // Acción para comportamiento 1
        onTime1 = 200, offTime1 = 200;
        onTime2 = 200, offTime2 = 200;
        onTime3 = 200, offTime3 = 200;
        onTime4 = 200, offTime4 = 200;

        tiempoSubida1 = 10;
        tiempoBajada1 = 10;
        tiempoSubida2 = 11;
        tiempoBajada2 = 11;

        break;
    default:
        // Acción para otros valores (por si acaso)
        Serial.println("Comportamiento desconocido.");
        break;
    }

    // Actualizar RELES y LEDS
    controlarRele(relePins[0], onTime1, offTime1, 0);
    controlarRele(relePins[1], onTime2, offTime2, 1);
    controlarRele(relePins[2], onTime3, offTime3, 2);
    controlarRele(relePins[3], onTime4, offTime4, 3);

    fadeLed(ledPin1, tiempoSubida1, tiempoBajada1);
    fadeLed(ledPin2, tiempoSubida2, tiempoBajada2);

    // Serial.print("Comportamiento: ");
    // Serial.print(comportamiento);
    // Serial.print(" | sensorTemperatura: ");
    // Serial.print(rawTemperature);
    // Serial.println();
}
