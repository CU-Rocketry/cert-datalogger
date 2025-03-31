#include <Arduino.h>
#include <SPI.h>

#define VSPI_MISO MISO
#define VSPI_MOSI MOSI
#define VSPI_SCLK SCK
#define VSPI_SS   SS

#define HSPI_MISO 12
#define HSPI_MOSI 13
#define HSPI_SCLK 14
#define HSPI_SS   15

#ifndef CONFIG_IDF_TARGET_ESP32
  #define VSPI FSPI
#endif

static const int spiClk = 1000000;  // 1 MHz

//uninitialized pointers to SPI objects
SPIClass *vspi = NULL;
SPIClass *hspi = NULL;

void spiCommand(SPIClass *spi, byte data) {
  //use it as you would the regular arduino SPI API
  spi->beginTransaction(SPISettings(spiClk, MSBFIRST, SPI_MODE0));
  digitalWrite(spi->pinSS(), LOW);  //pull SS slow to prep other end for transfer
  spi->transfer(data);
  digitalWrite(spi->pinSS(), HIGH);  //pull ss high to signify end of data transfer
  spi->endTransaction();
}

void setup() {
  // initialize SPI instances
  vspi = new SPIClass(VSPI);
  hspi = new SPIClass(HSPI);

  vspi->begin(VSPI_SCLK, VSPI_MISO, VSPI_MOSI, VSPI_SS);
  hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS);

  // setup slave select manually
  pinMode(VSPI_SS, OUTPUT);
  pinMode(HSPI_SS, OUTPUT);

  // init barometer
  LPS22HBSensor lp22hb(&vspi, VSPI_SS);
  lp22hb.begin();
  lp22hb.Enable();
}

// the loop function runs over and over again until power down or reset
void loop() {
  //use the SPI buses
  spiCommand(vspi, 0b01010101);  // junk data to illustrate usage
  spiCommand(hspi, 0b11001100);
  delay(100);
}