#include <Arduino.h>
#include <Servo.h>

// Pin del Servo
int servoPin = 9;
Servo myServo;

// ángulos máximo y mínimo del Servo
int maxAngle = 180;
int minAngle = 0;

// tiempo de pausa en ms entre movimientos
int pausa = 1000;

// angulo máximo de cada giro
int anguloMaximo = 180;
int anguloMinimo = 0;

// inicializamos tiempos de subida y bajada Servo
int tiempoSubida = 1;
int tiempoBajada = 10;

// para mapear comportamiento medido por sensor de temperatura
int comportamiento = 0;
int cantidadComportamientos = 10;
int prevComportamiento = -1;
//int temperatureSensorPin = A1; // Pin analógico para el sensor de temperatura
//int Temperature = 0;
bool change = false; // para detectar si hubo un cambio para resetear reles y fades

// Variables para almacenar la temperatura
int rawTemperature = 0;

// variables ligadas al tiempo
unsigned long previousMillis = 0; // Store the last time the temperature was read
unsigned long currentMillis = 0;  // Store the current time
unsigned long interval = 30000;    // Interval at which to read the temperature

unsigned long prevMillis[4] = {0, 0, 0, 0};

int repeticionesAntesLectura = 0;

// ---------- FUNCIÓN DE FADE SIMPLE ----------
void fadeServoSimple(int tiempoSubida, int tiempoBajada, int tiempoPausa, int anguloMin, int anguloMax)
{
    static int posicion;                   // posición actual del servo
    static int direccion;                  // dirección del fade
    static unsigned long posicionAnterior; // tiempo anterior
    static unsigned long pausaInicio = 0;
    static bool enPausa = false;

    // Inicialización solo la primera vez
    if (direccion == 0 || change == true)
    {
        posicion = anguloMin;
        direccion = 1;
        posicionAnterior = currentMillis;
        pausaInicio = 0;
        enPausa = false;
    }

    // Si está en pausa, esperar el tiempo de pausa
    if (enPausa)
    {
        if (currentMillis - pausaInicio >= (unsigned long)tiempoPausa)
        {
            enPausa = false;
            // Cambiar dirección después de la pausa
            direccion = (posicion == anguloMin) ? 1 : -1;
            posicionAnterior = currentMillis;
        }
        return;
    }

    // Escoger intervalo según dirección
    int intervalo = (direccion > 0) ? tiempoSubida : tiempoBajada;

    if (currentMillis - posicionAnterior >= (unsigned long)intervalo)
    {
        posicionAnterior = currentMillis;
        posicion += direccion;

        if (posicion <= anguloMin)
        {
            posicion = anguloMin;
            enPausa = true;
            pausaInicio = currentMillis;
        }
        else if (posicion >= anguloMax)
        {
            posicion = anguloMax;
            direccion = -1; // Cambia dirección sin pausa
        }

        myServo.write(posicion);
    }
}

// Pines para el sensor HC-SR04
const int trigPin = 2;
const int echoPin = 3;
unsigned long tiempoLectura = 500; // Intervalo de lectura en ms
unsigned long ultimoTiempoLectura = 0;

volatile bool triggerSent = false;
volatile unsigned long echoStart = 0;
volatile unsigned long echoEnd = 0;
volatile bool echoReceived = false;

void triggerUltrasonic()
{
    // Enviar pulso de disparo: 2us LOW, 10us HIGH, 2us LOW
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    triggerSent = true;
}
// Nueva versión de fadeServo con argumento de repeticiones que gatilla
// una nueva lectura de distancia al completar X idas y vueltas.
void fadeServo(int tiempoSubida, int tiempoBajada, int tiempoPausa, int anguloMin, int anguloMax, int repeticionesAntesLectura)
{
    static int posicion = 0;                   // posición actual del servo
    static int direccion = 0;                  // dirección del fade (+1 subida, -1 bajada)
    static unsigned long posicionAnterior = 0; // tiempo anterior para pasos
    static unsigned long pausaInicio = 0;
    static bool enPausa = false;

    // Contador de idas y vueltas completas (min -> max -> min)
    static int vueltasCompletas = 0;
    // Indicador temporal para evitar contar una vuelta al inicializar en el mínimo
    static bool inicioContado = false;

    // Reinicio cuando cambia el modo externo
    if (direccion == 0 || change == true)
    {
        posicion = anguloMin;
        direccion = 1;
        posicionAnterior = currentMillis;
        pausaInicio = 0;
        enPausa = false;
        vueltasCompletas = 0;
        inicioContado = true; // evitamos contar la vuelta inicial incompleta
    }

    // Si está en pausa, esperar el tiempo de pausa
    if (enPausa)
    {
        if (currentMillis - pausaInicio >= (unsigned long)tiempoPausa)
        {
            enPausa = false;
            // Cambiar dirección después de la pausa
            direccion = (posicion == anguloMin) ? 1 : -1;
            posicionAnterior = currentMillis;
        }
        return;
    }

    // Escoger intervalo según dirección
    int intervalo = (direccion > 0) ? tiempoSubida : tiempoBajada;

    if (currentMillis - posicionAnterior >= (unsigned long)intervalo)
    {
        posicionAnterior = currentMillis;
        // Guardamos la dirección previa para detectar llegada al mínimo desde la bajada
        int direccionPrev = direccion;
        posicion += direccion;

        if (posicion <= anguloMin)
        {
            // Llegó al mínimo
            posicion = anguloMin;
            enPausa = true;
            pausaInicio = currentMillis;

            // Si venía bajando y no es la inicialización, contamos una vuelta completa
            if (direccionPrev < 0 && inicioContado)
            {
                vueltasCompletas++;
            }
            // Una vez pasada la fase de arranque, permitimos conteos posteriores
            inicioContado = true;
        }
        else if (posicion >= anguloMax)
        {
            // Llegó al máximo: cambia dirección (sin pausa en este diseño)
            posicion = anguloMax;
            direccion = -1;
        }

        myServo.write(posicion);

        // Si se alcanzó el número de repeticiones solicitadas, enviar trigger de ultrasonido
        if (repeticionesAntesLectura > 0 && vueltasCompletas >= repeticionesAntesLectura)
        {
            // Llamada no bloqueante para solicitar la lectura
            triggerUltrasonic();
            // Reiniciar contador para futuras solicitudes
            vueltasCompletas = 0;
        }
    }
}
void echoISR()
{
    if (digitalRead(echoPin) == HIGH)
    {
        echoStart = micros();
    }
    else
    {
        echoEnd = micros();
        echoReceived = true;
    }
}

void setupUltrasonic()
{
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(echoPin), echoISR, CHANGE);
}

void leerSensorUltrasonico()
{
    // Solo ejecutar si ha pasado el intervalo requerido
    if (currentMillis - previousMillis >= interval)
    {
        if (!triggerSent)
        {
            triggerUltrasonic();
        }
        if (echoReceived)
        {
            // El cálculo con unsigned long maneja correctamente el rollover de micros()
            unsigned long duracion = echoEnd - echoStart;
            // Convertir tiempo de ida y vuelta (µs) a distancia en cm: distancia_cm = duracion / 58
            unsigned long distancia_cm = duracion / 58UL;

            comportamiento = duracion % cantidadComportamientos; // Mapear duración a comportamiento (0, 1, ... cantidadComportamientos-1)

            Serial.print("Tiempo de pulso (us): ");
            Serial.print(duracion);
            Serial.print(" | Comportamiento (cm): ");
            Serial.println(comportamiento);

            // Actualizar el tiempo de la última lectura
            previousMillis = currentMillis;
        }
    }

        // Reset flags para permitir nueva medición
        triggerSent = false;
        echoReceived = false;
    }


void setup()
{
    myServo.attach(servoPin);

    Serial.begin(9600);
    // Inicializar pines y la interrupción del sensor ultrasónico
    setupUltrasonic();
    // Inicializar el temporizador de control para evitar lecturas inmediatas repetidas
    previousMillis = millis();
}

void loop()
{
    currentMillis = millis();
    //controlarComportamiento();

    switch (comportamiento)
    {
    case 0:
        // Acción para comportamiento 0
        tiempoSubida = 40;
        tiempoBajada = 20;
        pausa = 500;
        anguloMinimo = 10;
        anguloMaximo = 80;
        repeticionesAntesLectura = 4;
        break;

    case 1:
        // Acción para comportamiento 1
        tiempoSubida = 30;
        tiempoBajada = 60;
        pausa = 500;
        anguloMinimo = 10;
        anguloMaximo = 80;
        repeticionesAntesLectura = 4;
        break;

    case 2:
        // Acción para comportamiento 2
        tiempoSubida = 40;
        tiempoBajada = 60;
        pausa = 1000;
        anguloMinimo = 10;
        anguloMaximo = 170;
        repeticionesAntesLectura = 2;
        break;

    case 3:
        // Movimiento más lento y amplio
        tiempoSubida = 80;
        tiempoBajada = 80;
        pausa = 800;
        anguloMinimo = 20;
        anguloMaximo = 160;
        repeticionesAntesLectura = 3;
        break;

    case 4:
        // Subida lenta, bajada rápida
        tiempoSubida = 90;
        tiempoBajada = 40;
        pausa = 600;
        anguloMinimo = 30;
        anguloMaximo = 120;
        repeticionesAntesLectura = 5;
        break;

    case 5:
        // Oscilación pequeña pero rítmica
        tiempoSubida = 50;
        tiempoBajada = 50;
        pausa = 300;
        anguloMinimo = 40;
        anguloMaximo = 100;
        repeticionesAntesLectura = 6;
        break;

    case 6:
        // Movimiento amplio y pausado
        tiempoSubida = 100;
        tiempoBajada = 100;
        pausa = 1200;
        anguloMinimo = 15;
        anguloMaximo = 180;
        repeticionesAntesLectura = 1;
        break;

    case 7:
        // Subida más rápida, bajada más lenta
        tiempoSubida = 40;
        tiempoBajada = 90;
        pausa = 700;
        anguloMinimo = 20;
        anguloMaximo = 150;
        repeticionesAntesLectura = 3;
        break;

    case 8:
        // Movimiento irregular con pausa larga
        tiempoSubida = 60;
        tiempoBajada = 30;
        pausa = 1500;
        anguloMinimo = 50;
        anguloMaximo = 130;
        repeticionesAntesLectura = 2;
        break;

    case 9:
        // Movimiento corto y rítmico
        tiempoSubida = 30;
        tiempoBajada = 30;
        pausa = 200;
        anguloMinimo = 70;
        anguloMaximo = 110;
        repeticionesAntesLectura = 8;
        break;
    default:
        // Acción para otros valores (por si acaso)
        Serial.println("Comportamiento desconocido.");
        break;
    }

    // Procesar lecturas del ultrasonido de forma no bloqueante
    leerSensorUltrasonico();

           fadeServo(tiempoSubida, tiempoBajada, pausa, anguloMinimo, anguloMaximo, repeticionesAntesLectura);


    // Serial.print("Comportamiento: ");
    // Serial.print(comportamiento);
    // Serial.print(" | sensorTemperatura: ");
    // Serial.print(rawTemperature);
    // Serial.println();
}
