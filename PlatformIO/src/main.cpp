#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>

CAN_message_t receiveCan();
void draw_template();

namespace carrier
{
  namespace pin
  {
    constexpr uint8_t oledDcPower{6};
    constexpr uint8_t oledCs{10};
    constexpr uint8_t oledReset{5};
  }

  namespace oled
  {
    constexpr uint8_t screenWidth{128}; // OLED display width in pixels
    constexpr uint8_t screenHeight{64}; // OLED display height in pixels
  }
}

namespace {
  CAN_message_t msg;

  FlexCAN_T4<CAN0, RX_SIZE_256, TX_SIZE_16> can0;
  //FlexCAN_T4<CAN1, RX_SIZE_256, TX_SIZE_16> can1;

  Adafruit_SSD1306 display( carrier::oled::screenWidth,
                            carrier::oled::screenHeight,
                            &SPI,
                            carrier::pin::oledDcPower,
                            carrier::pin::oledReset,
                            carrier::pin::oledCs);
}

void setup() {
  Serial.begin(9600);
  can0.begin();
  can0.setBaudRate(250000);

  display.clearDisplay();
  display.display();
  delay(2000);
}

void loop() {
  for (int16_t x = 0; x < 128; ++x)
  {
    receiveCan();
  }
}


CAN_message_t receiveCan() {
  static CAN_message_t msg,rxmsg;
  uint8_t data[8];
  while(can0.read(rxmsg))
  { 
     for (int i=0; i < 8; i++) {
         data[i] = rxmsg.buf[i];
     } 
  }

  CAN_message_t Ekko;
  Ekko.id = 0x13;
  Ekko.len = 8;
  
  for (int i=0;i<8;i++) {
    Ekko.buf[i] = data[i];
  }

  can0.write(Ekko);
  return rxmsg;
}

void draw_template(){
  display.clearDisplay();
  display.drawCircle(30, 30, 10, SSD1306_WHITE);
  display.drawRoundRect(5, 5, 70, 40, 5, SSD1306_WHITE);
}