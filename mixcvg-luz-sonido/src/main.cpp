#include <Arduino.h>
// Melodía intercalada entre pin 8 y pin 9
// Frecuencias aleatorias entre 5000 y 15000 Hz

// Melodía intercalada entre pin 8 y pin 9
// Frecuencias y duraciones aleatorias en cada ciclo

int pines[] = {8, 9};   // dos salidas
int indicePin = 0;      // para alternar
int totalNotas = 8;     // cantidad de notas por ciclo

void setup() {
  randomSeed(analogRead(A0));  // semilla aleatoria
}

void loop() {
  for (int i = 0; i < totalNotas; i++) {
    // frecuencia aleatoria entre 5000 y 15000 Hz
    int freq = random(5000, 15001);

    // duración aleatoria entre 100 ms y 600 ms
    int duracion = random(100, 601);

    int pin = pines[indicePin];    // alternar entre 8 y 9
    tone(pin, freq, duracion);     // nota en el parlante elegido

    delay(duracion * 1.3);         // nota + pequeña pausa

    indicePin = 1 - indicePin;     // alternar parlantes (0→1→0→1...)
  }

  delay(1000);  // pausa entre ciclos de melodía
}
