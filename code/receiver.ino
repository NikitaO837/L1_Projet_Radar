#include <SPI.h>
#include "LoRa.h"
#include <FastLED.h> // http://librarymanager/All#FASTLED

#define LED_PIN 4
#define DATA_PIN 4
#define NUM_LEDS 21
#define BRIGHTNESS 64
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending; //Inclusion des bibliothéques, définition des variables et constantes.
int counter = 0;

// Parameters you can play with :

int txPower = 14; // from 0 to 20, default is 14
int spreadingFactor = 12; // from 7 to 12, default is 12
long signalBandwidth = 125E3; // 7.8E3, 10.4E3, 15.6E3, 20.8E3, 31.25E3,41.7E3,62.5E3,125E3,250E3,500e3, default is 125E3
int codingRateDenominator=5; // Numerator is 4, and denominator from 5 to 8, default is 5
int preambleLength=8; // from 2 to 20, default is 8
String payload = "hello"; // you can change the payload

int distance = 0; // Valeur de la distance initiale

#define SS 10
#define RST 8
#define DI0 6
#define BAND 865E6 // Définition des constantes pour LoRa

void setup() {

  Serial.begin(115200);
  while (!Serial); // Activation du Serial Monitor

  Serial.println("LoRa Receiver");
  Serial.print("SetFrequency : ");
  Serial.print(BAND);
  Serial.println("Hz");
  Serial.print("SetSpreadingFactor : SF");
  Serial.println(spreadingFactor);

  SPI.begin();
  LoRa.setPins(SS,RST,DI0); // Activation de LoRa de la carte récéptrice

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1); // Setup de LoRa échoué
  }
 LoRa.setTxPower(txPower,1);
 LoRa.setSpreadingFactor(spreadingFactor);
 LoRa.setSignalBandwidth(signalBandwidth);
 LoRa.setCodingRate4(codingRateDenominator);
 LoRa.setPreambleLength(preambleLength); // Setup LoRa

 Serial.println("resetting");
 LEDS.addLeds<WS2812,DATA_PIN,RGB>(leds,NUM_LEDS);
 LEDS.setBrightness(84);  // Setup LED
 
 FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
 FastLED.setBrightness(  BRIGHTNESS );

}

void loop() {

if (distance>.0343) {
  fill_solid(leds, NUM_LEDS, CRGB::Red);
  FastLED.show();
} else {
  fill_solid(leds, NUM_LEDS, CRGB::Green);
  FastLED.show();
}

   // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {

    String received = "";
    // packet bien reçu
    Serial.print("Received distance is ");

    // lecture du packet reçu
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
      received += (char)LoRa.read();
    }

    distance = received.toFloat();
    Serial.print(" cm ");
    Serial.println(""); // String complémentaire pour affichage du signal reçu plus claire
  }

  delay(1000);

}
