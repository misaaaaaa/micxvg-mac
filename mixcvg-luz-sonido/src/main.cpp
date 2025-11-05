#include <Arduino.h>

// --- CONFIGURACIÓN DE PINES ---
const int NUM_PARLANTES = 10;
int parlante[NUM_PARLANTES] = {2, 3, 4, 5, 6, 7, 8, 10, 11, 12};

unsigned long tiempoActual = 0;
unsigned long tiempoAnterior = 0;

int ldrPin = A0; // Pin analógico para el LDR
int intensidadLuz = 0;

unsigned long intervaloComportamiento = 30000; // Intervalo para cambiar comportamiento (1 segundo)
int comportamiento = 0;
int cantidadComportamientos = 10; // Número de comportamientos diferentes

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
    // comportamiento = 0;
    if (tiempoActual - tiempoAnterior >= intervaloComportamiento)
    {
        comportamiento = intensidadLuz % cantidadComportamientos; // Cambia el comportamiento basado en la intensidad de luz
        // Serial.println("Cambiando comportamiento a: " + String(comportamiento));
        tiempoAnterior = tiempoActual;
    }
}

void loop()
{
    intensidadLuz = analogRead(ldrPin); // + random(10); // Lee el valor del LDR (0-1023)

    seleccionarComportamiento();

    // sonido(0, 200, 30, 10);              // Reproduce tono de 200 Hz en el pin 2
    // pausa(100);                          // Pausa de 100 ms
    // sonido(1, random(100, 500), 20, 10); // Reproduce tono aleatorio en el pin 3
    // pausa(200);

    switch (comportamiento)
    {

        // case 0:
        //     // Comportamiento 0: Tonos bajos y largos
        //     sonido(0, random(8000, 15000), random(30, 400), 50); // Pin 2
        //     pausa(random(500, 1000));
        //     sonido(1, 3000, 100, 50); // Pin 3
        //     pausa(random(500, 1000));
        //     sonido(7, random (32, 100), 100, 50); // Pin 10
        //     pausa(random(500, 1000));
        //     break;
        // case 1:
        //     // Comportamiento 1: Tonos medios y rápidos
        //     sonido(0, 400, 50, 20); // Pin 6
        //     pausa(350);
        //     sonido(1, 450, 50, 20); // Pin 7
        //     pausa(150);
        //     sonido(7, 2000, 100, 50); // Pin 10
        //     pausa(400);
        //     break;
        // case 2:
        //     // Comportamiento 2: Tonos altos y cortos
        //     sonido(0, 100, 500, 10); // Pin 8
        //     pausa(300);
        //     sonido(1, 200, 500, 10); // Pin 10
        //     pausa(300);
        //     break;

    case 0:
        // Comportamiento 0: Altos claros con cierre medio
        sonido(0, random(4000, 8000), 250, 80);
        pausa(400);
        sonido(2, random(2000, 6000), 200, 60);
        pausa(250);
        sonido(5, random(1500, 5000), 180, 70);
        pausa(220);
        sonido(7, random(300, 900), 80, 40);
        pausa(250);
        sonido(1, random(5000, 8000), 200, 60);
        pausa(400);
        sonido(8, random(300, 800), 70, 30);
        pausa(300);
        pausa(1600);
        break;

    case 1:
        // Comportamiento 1: Pulsos rápidos, rango alto
        for (int i = 0; i < 7; i++)
        {
            sonido(random(0, 6), random(1000, 8000), random(80, 180), random(40, 70));
            pausa(random(120, 200));
        }
        sonido(8, random(300, 1000), 60, 30);
        pausa(250);
        pausa(900);
        break;

    case 2:
        // Comportamiento 2: Escalera descendente, cierre grave suave
        sonido(0, random(6000, 8000), 200, 70);
        pausa(180);
        sonido(1, random(4000, 6000), 180, 60);
        pausa(160);
        sonido(3, random(2000, 5000), 160, 50);
        pausa(150);
        sonido(4, random(1500, 3000), 150, 50);
        pausa(200);
        sonido(8, random(300, 900), 70, 40);
        pausa(400);
        sonido(9, random(130, 240), 260, 120);
        pausa(1200);
        break;

    case 3:
        // Comportamiento 3: Serie resonante media-alta
        sonido(4, random(2000, 7000), 140, 50);
        pausa(120);
        sonido(6, random(1500, 4000), 160, 60);
        pausa(150);
        sonido(2, random(3000, 7000), 180, 60);
        pausa(160);
        sonido(7, random(400, 800), 70, 30);
        pausa(200);
        sonido(5, random(2000, 5000), 180, 60);
        pausa(400);
        pausa(900);
        break;

    case 4:
        // Comportamiento 4: Agudos sostenidos con quiebres graves
        sonido(0, random(5000, 8000), 300, 120);
        pausa(300);
        sonido(7, random(300, 700), 80, 40);
        pausa(150);
        sonido(1, random(2000, 7000), 200, 80);
        pausa(250);
        sonido(8, random(400, 900), 90, 40);
        pausa(200);
        sonido(9, random(130, 230), 230, 110);
        pausa(1200);
        break;

    case 5:
        // Comportamiento 5: Pulsos breves y dispersos
        for (int i = 0; i < 8; i++)
        {
            sonido(random(0, 6), random(1000, 8000), random(70, 160), random(40, 70));
            pausa(random(100, 220));
        }
        sonido(8, random(300, 900), 80, 30);
        pausa(250);
        pausa(1000);
        break;

    case 6:
        // Comportamiento 6: Alternancia entre medios y agudos
        sonido(2, random(3000, 7000), 150, 50);
        pausa(150);
        sonido(8, random(300, 900), 80, 40);
        pausa(150);
        sonido(5, random(3000, 8000), 200, 70);
        pausa(200);
        sonido(7, random(200, 800), 70, 30);
        pausa(250);
        sonido(1, random(5000, 8000), 160, 60);
        pausa(250);
        pausa(1000);
        break;

    case 7:
        // Comportamiento 7: Graves rítmicos con contrastes agudos
        sonido(9, random(120, 250), 240, 100);
        pausa(200);
        sonido(3, random(2000, 7000), 180, 60);
        pausa(250);
        sonido(7, random(300, 900), 80, 40);
        pausa(200);
        sonido(0, random(4000, 8000), 220, 70);
        pausa(250);
        pausa(1200);
        break;

    case 8:
        // Comportamiento 8: Cascada irregular de tonos altos
        sonido(0, random(1000, 4000), 200, 60);
        pausa(100);
        sonido(1, random(2000, 5000), 180, 50);
        pausa(120);
        sonido(2, random(3000, 6000), 160, 50);
        pausa(150);
        sonido(3, random(4000, 8000), 140, 40);
        pausa(180);
        sonido(4, random(3000, 7000), 120, 40);
        pausa(200);
        sonido(5, random(1000, 4000), 100, 40);
        pausa(220);
        sonido(7, random(400, 800), 70, 30);
        pausa(400);
        pausa(900);
        break;

    case 9:
        // Comportamiento 9: Grave profundo con ecos medios
        sonido(9, random(120, 220), 300, 120);
        pausa(250);
        sonido(8, random(300, 900), 80, 40);
        pausa(200);
        sonido(6, random(2000, 5000), 180, 60);
        pausa(180);
        sonido(7, random(200, 600), 70, 30);
        pausa(250);
        sonido(4, random(3000, 8000), 200, 70);
        pausa(1000);
        break;

    default:
        break;
    }

    Serial.println("Tiempo actual: " + String(tiempoActual) + " ms" +
                   " | Intensidad luz: " + String(intensidadLuz) +
                   " | Comportamiento: " + String(comportamiento));
}
