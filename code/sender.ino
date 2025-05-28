#include <SPI.h>
#include "LoRa.h"
#include <FastLED.h> // http://librarymanager/All#FASTLED
#include <Servo.h>
#define LED_PIN     4
#define NUM_LEDS    21
#define BRIGHTNESS  64
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define SS 10
#define RST 8
#define DI0 3
#define BAND 865E6  // Here you define the frequency carrier
CRGB leds[NUM_LEDS]; // Setup LEDs, LoRa, capteur et inclusion des bibliothéques pour la carte émettrice
CRGBPalette16 currentPalette;
TBlendType    currentBlending; 
Servo myservo;  // Création de l'objet servo pour controler le servomoteur

int counter = 0;
const int trigPin = A2;  
const int echoPin = A3;
const int signAl = A0; // Attribution des pins: A2 pour Trig du capteur, A2 pour Echo du capteur, AO pour l'alarme

float duration, distance; 

// Parameters you can play with :

int txPower = 14; // from 0 to 20, default is 14
int spreadingFactor = 12; // from 7 to 12, default is 12
long signalBandwidth = 125E3; // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3,41.7E3,62.5E3,125E3,250E3,500e3, default is 125E3
int codingRateDenominator=5; // Numerator is 4, and denominator from 5 to 8, default is 5
int preambleLength=8; // from 2 to 20, default is 8
String payload = "hello"; // you can change the payload

void setup() {

 delay( 100 ); // power-up safety delay

  // Initialize the Serial port:
  
  Serial.begin(115200);
  Serial.println("RADAR VOITURE");

  // Initialize all pins
  
  pinMode(trigPin, OUTPUT);  
	pinMode(echoPin, INPUT); 

  pinMode(signAl, OUTPUT);

// Setup LED

 myservo.attach(A1);  // attaches the servo on pin A1 to the servo object

  while (!Serial); // Activation de LoRa

  Serial.println("LoRa Sender");
  Serial.print("SetFrequency : ");
  Serial.print(BAND);
  Serial.println("Hz");
  Serial.print("SetSpreadingFactor : SF");
  Serial.println(spreadingFactor);

  SPI.begin();
  LoRa.setPins(SS,RST,DI0);

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1); // Activation de LoRa échoué
  }
 LoRa.setTxPower(txPower,1);
 LoRa.setSpreadingFactor(spreadingFactor);
 LoRa.setSignalBandwidth(signalBandwidth);
 LoRa.setCodingRate4(codingRateDenominator);
 LoRa.setPreambleLength(preambleLength);
 
}

void loop() {

for (int i=0; i<=80; i=i+10){
  myservo.write(i);                  // sets the servo position according to the scaled value
  delay(300);}                           // waits for the servo to get there

  digitalWrite(trigPin, LOW);  
	delayMicroseconds(2);  
	digitalWrite(trigPin, HIGH);  
	delayMicroseconds(10);  
	digitalWrite(trigPin, LOW); 

  duration = pulseIn(echoPin, HIGH);
  distance = (duration*.0343)/2;   // The speed of sound is approximately 340 meters per second, but since the pulseIn() function returns the time in microseconds
  uint8_t dist_temp = map(distance,0,200,0,255); // Map value from distance sensor to LED

  // FastLED's built-in rainbow generator
  fill_solid( leds, NUM_LEDS, ColorFromPalette(RainbowColors_p,dist_temp,BRIGHTNESS, LINEARBLEND));
  
// send the 'leds' array out to the actual LED strip
  FastLED.show();
  delay(10); // power-up safety delay

  for (int i=80; i>=0; i=i-10){
  myservo.write(i);                  // sets the servo position according to the scaled value
  delay(300);}                       // waits for the servo to get there

if (distance <= 15){
    digitalWrite(signAl, 1000);
    Serial.println("OBSTACLE !!!");
    } else {
      digitalWrite(signAl, LOW);
      Serial.println("NO OBSTACLES :D");
      }
      delay( 10 );

 Serial.print("");
  
  // send packet
  
  LoRa.beginPacket();
  LoRa.print(distance); 
  
  LoRa.endPacket();
  counter++;

  Serial.print("Distance = {");
  Serial.print(distance);
  Serial.print("}cm check N°");
  Serial.println(counter); // String esthétique
 
  delay(100);
}
