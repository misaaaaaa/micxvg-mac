#include <Arduino.h>
#include <LEDFader.h>    // Include the LEDFader library for PWM control
#include <SimpleTimer.h> // Include the SimpleTimer library for timing events

LEDFader led1 = LEDFader(9);
LEDFader led2 = LEDFader(10);
LEDFader led1A = LEDFader(6);
LEDFader led2A = LEDFader(11);

SimpleTimer firstTimer;
SimpleTimer secondTimer;
SimpleTimer thirdTimer;
SimpleTimer fourthTimer;

bool flag = false; // Flag to control the first timer

int relePins[4] = {2, 3, 4, 5};
int estadoRele[4] = {LOW, LOW, LOW, LOW};
int pwmPins[2] = {9, 10};
int pwmPinsLeds[2] = {6, 11};

int temperatureSensorPin = A0;
int rawTemperature = 0;

int comportamiento = 0;

unsigned long previousMillis = 0; // Store the last time the temperature was read
unsigned long currentMillis = 0;  // Store the current time
unsigned long interval = 1000;    // Interval at which to read the temperature (1 second)

void tReading()
{

  if (currentMillis - previousMillis >= interval)
  {

    rawTemperature = analogRead(temperatureSensorPin);        // Read the temperature sensor value
    int Temperature = map(rawTemperature, 1023, 0, -40, 125); // Map the raw value to a temperature range

    Serial.print("Raw Temperature: ");
    Serial.println(rawTemperature);
    Serial.print("Mapped Temperature: ");
    Serial.println(Temperature);

    previousMillis = currentMillis;
  }
}

void setup()
{
  Serial.begin(9600); // Start serial communication at 9600 baud rate

  for (int i = 0; i < 4; i++)
  {
    pinMode(relePins[i], OUTPUT);   // Set rele pins as output
    digitalWrite(relePins[i], LOW); // Initialize rele pins to LOW
  }
  for (int i = 0; i < 2; i++)
  {
    pinMode(pwmPins[i], OUTPUT); // Set PWM pins as output
    analogWrite(pwmPins[i], 0);  // Initialize PWM pins to 0
  }
  for (int i = 0; i < 2; i++)
  {
    pinMode(pwmPinsLeds[i], OUTPUT); // Set PWM pins as output
    analogWrite(pwmPinsLeds[i], 0);  // Initialize PWM pins to 0
  }

  firstTimer.setInterval(5000);
  secondTimer.setInterval(3000);
  thirdTimer.setInterval(1000);
  fourthTimer.setInterval(4000);
}

// the loop function runs over and over again forever
void loop()
{

  currentMillis = millis(); // Get the current time

  led1.update();
  led2.update();
  led1A.update();
  led2A.update();

  if (!led1.is_fading())
  {
    if (led1.get_value() == 255)
    {
      led1.fade(0, 1000); // Fade to 0 over 1000 milliseconds
      led1A.fade(0, 1000); // Fade to 0 over 1000 milliseconds
    }
    else
    {
      led1.fade(254, 500); // Fade to 255 over 1000 milliseconds
      led1A.fade(254, 500); // Fade to 255 over 1000 milliseconds
    }
  }

  if (!led1.is_fading())
  {
    if (led1.get_value() == 254)
    {
      led1.fade(0, 10000); // Fade to 0 over 1000 milliseconds
      led1A.fade(0, 10000); // Fade to 0 over 1000 milliseconds
    }
    else
    {
      led1.fade(255, 100); // Fade to 255 over 1000 milliseconds
      led1A.fade(255, 100); // Fade to 255 over 1000 milliseconds
    }
  }

  // Cambia la dirección de led2 cuando termine su fade
  if (!led2.is_fading())
  {
    if (led2.get_value() == 255)
    {
      led2.fade(0, 750); // Fade to 0 over 1000 milliseconds
      led2A.fade(0, 750); // Fade to 0 over 1000 milliseconds
    }
    else
    {
      led2.fade(255, 750); // Fade to 255 over 1000 milliseconds
      led2A.fade(255, 750); // Fade to 255 over 1000 milliseconds
    }
  }

  // RELAY control
  //  if (firstTimer.isReady() && !flag) {            // Check is ready a first timer
  //        Serial.println("5 seconds have passed");
  //         estadoRele[0] = !estadoRele[0]; // Toggle the state of the first relay

  //       // Set the flag so as not to fall into this condition
  //       flag = true;
  //   }

  if (firstTimer.isReady())
  { // Check is ready a first timer
    Serial.println("Called every 5 sec");
    // Do something ...
    // For example, read a sensor or update a display
    estadoRele[0] = !estadoRele[0]; // Toggle the state of the first relay
    firstTimer.setInterval(5000);   // Reset the first timer interval to 5 seconds
    firstTimer.reset();             // Reset a first timer
  }

  if (secondTimer.isReady())
  { // Check is ready a second timer
    Serial.println("Called every 3 sec");
    // Do something ...
    estadoRele[1] = !estadoRele[1]; // Toggle the state of the first relay
    secondTimer.setInterval(3000);
    secondTimer.reset(); // Reset a second timer
  }

  if (thirdTimer.isReady())
  { // Check is ready a third timer
    Serial.println("Called every 1 sec");
    // Do something ...
    estadoRele[2] = !estadoRele[2]; // Toggle the state of the first relay
    thirdTimer.setInterval(1000);
    thirdTimer.reset(); // Reset a third timer
  }

  if (fourthTimer.isReady())
  { // Check is ready a fourth timer
    Serial.println("Called every 4 sec");
    // Do something ...
    estadoRele[3] = !estadoRele[3]; // Toggle the state of the first relay
    fourthTimer.setInterval(4000);
    fourthTimer.reset(); // Reset a fourth timer
  }

  digitalWrite(relePins[0], estadoRele[0]); // Write the new
  digitalWrite(relePins[1], estadoRele[1]); // Write the new state to the second relay
  digitalWrite(relePins[2], estadoRele[2]); // Write the new state to the third relay
  digitalWrite(relePins[3], estadoRele[3]); // Write the new state to the fourth relay

  tReading(); // Call the temperature reading function

  // delay(1000); // Wait for 1 second before the next reading
}
