#include <Arduino.h>

// --- CONFIGURACIÓN DE PINES ---
const int NUM_PARLANTES = 10;
int parlante[NUM_PARLANTES] = {2, 3, 4, 5, 6, 7, 8, 10, 11, 12};

unsigned long tiempoActual = 0;
unsigned long tiempoAnterior = 0;

int ldrPin = A0; // Pin analógico para el LDR
int intensidadLuz = 0;

int intervaloComportamiento = 1000; // Intervalo para cambiar comportamiento (1 segundo)
int comportamiento = 0;
int cantidadComportamientos = 3; // Número de comportamientos diferentes

void sonido(int index, int frecuencia, int duracion, int silencio)
{
    tiempoActual = millis();
    tone(parlante[index], frecuencia, duracion);
    delay(duracion + 30); // Espera 30 ms
    tiempoActual = millis();
    noTone(parlante[index]); // Detiene el tono
    delay(silencio);         // Pausa el silencio
    tiempoActual = millis();
}

void pausa(int duracion)
{
    for (int i = 0; i < NUM_PARLANTES; i++)
    {
        noTone(parlante[i]); // Apaga el tono si está sonando
    }
    delay(duracion); // Pausa el silencio
    tiempoActual = millis();
}

void playIntro()
{
    // Introducción: secuencia de tonos al iniciar
    sonido(0, 500, 500, 50);  // Pin 2
    sonido(1, 600, 500, 50);  // Pin 3
    sonido(2, 700, 500, 50);  // Pin 4
    sonido(3, 800, 500, 50);  // Pin 5
    sonido(4, 900, 500, 50);  // Pin 6
    sonido(5, 1000, 500, 50); // Pin 7
    sonido(6, 1100, 500, 50); // Pin 8
    sonido(7, 1200, 500, 50); // Pin 10
    sonido(8, 1300, 500, 50); // Pin 11
    sonido(9, 1400, 500, 50); // Pin 12

    sonido(0, 500, 500, 50);  // Pin 2
    sonido(1, 600, 500, 50);  // Pin 3
    sonido(2, 700, 500, 50);  // Pin 4
    sonido(3, 800, 500, 50);  // Pin 5
    sonido(4, 900, 500, 50);  // Pin 6
    sonido(5, 1000, 500, 50); // Pin 7
    sonido(6, 1100, 500, 50); // Pin 8
    sonido(7, 1200, 500, 50); // Pin 10
    sonido(8, 1300, 500, 50); // Pin 11
    sonido(9, 1400, 500, 50); // Pin 12

    pausa(500); // Pausa de medio segundo
}

void setup()
{
    Serial.begin(9600);
    for (int i = 0; i < NUM_PARLANTES; i++)
    {
        pinMode(parlante[i], OUTPUT);
    }

    playIntro();
}

void seleccionarComportamiento()
{

    if (tiempoActual - tiempoAnterior >= intervaloComportamiento)
    {
        comportamiento = intensidadLuz % cantidadComportamientos; // Cambia el comportamiento basado en la intensidad de luz
        tiempoAnterior = tiempoActual;
    }
}

void loop()
{
    intensidadLuz = analogRead(ldrPin); // Lee el valor del LDR (0-1023)
    seleccionarComportamiento();

    sonido(0, 200, 30, 10);              // Reproduce tono de 200 Hz en el pin 2
    pausa(100);                          // Pausa de 100 ms
    sonido(1, random(100, 500), 20, 10); // Reproduce tono aleatorio en el pin 3
    pausa(200);

    switch (comportamiento)
    {
    case 0:
        // Comportamiento 0: Tonos bajos y largos
        sonido(0, 1000, 100, 50); // Pin 2
        pausa(100);
        sonido(1, 3000, 100, 50); // Pin 3
        pausa(100);
        sonido(7, 2000, 100, 50); // Pin 10
        pausa(100);
        break;
    case 1:
        // Comportamiento 1: Tonos medios y rápidos
        sonido(0, 400, 50, 20); // Pin 6
        pausa(150);
        sonido(1, 450, 50, 20); // Pin 7
        pausa(150);
        break;
    case 2:
        // Comportamiento 2: Tonos altos y cortos
        sonido(0, 100, 500, 10); // Pin 8
        pausa(300);
        sonido(1, 200, 500, 10); // Pin 10
        pausa(300);
        break;
    default:
        break;
    }

    Serial.println("Tiempo actual: " + String(tiempoActual) + " ms" +
                   " | Intensidad luz: " + String(intensidadLuz) +
                   " | Comportamiento: " + String(comportamiento));
}
