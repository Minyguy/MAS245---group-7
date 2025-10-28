// Written by K. M. Knausgård 2023-10-21

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <FlexCAN_T4.h>
#include <SPI.h>
#include <Wire.h>
#include <string.h>

#include "mas245_logo_bitmap.h"



namespace carrier
{
  namespace pin
  {
    constexpr uint8_t joyLeft{18};
    constexpr uint8_t joyRight{17};
    constexpr uint8_t joyClick{19};
    constexpr uint8_t joyUp{22};
    constexpr uint8_t joyDown{23};

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
CAN_message_t receiveCan();

namespace images
{
  namespace pumpkin 
  {
    constexpr uint8_t width{16};
    constexpr uint8_t height{18};

    constexpr static uint8_t PROGMEM bitmap[] =
    {
      0b00000000, 0b00100000,
      0b00000000, 0b11100000,
      0b00000001, 0b10000000,
      0b00000001, 0b10000000,
      0b00000001, 0b10000000,
      0b00000011, 0b11100000,
      0b00001111, 0b11111000,
      0b00111111, 0b11111000,
      0b01111111, 0b11111110,
      0b01111111, 0b11111110,
      0b11111111, 0b11111111,
      0b11111111, 0b11111111,
      0b11111111, 0b11111111,
      0b01111111, 0b11111110,
      0b01111111, 0b11111110,
      0b00011111, 0b11111000,
      0b00011111, 0b11110000,
      0b00000111, 0b11100000,
    };
  };
};


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


struct Message {
    uint8_t sequenceNumber;
    float temperature;
};


void drawSplash();
void demoMessage();
void sendCan();
void sendCan(const Message& message);
void draw_template();

void setup() {
  Serial.begin(9600);
  can0.begin();
  can0.setBaudRate(250000);
  //can1.begin();
  //can1.setBaudRate(250000);

  // Gen. display voltage from 3.3V (https://adafruit.github.io/Adafruit_SSD1306/html/_adafruit___s_s_d1306_8h.html#ad9d18b92ad68b542033c7e5ccbdcced0)
  if( !display.begin(SSD1306_SWITCHCAPVCC) )
  {
    Serial.println(F("ERROR: display.begin(SSD1306_SWITCHCAPVCC) failed."));

    for(;;)
    {
      // Blink angry LED pattern or something, initializing LED failed.
    }
  }

  display.clearDisplay();
  display.display();
  delay(2000);
  

  drawSplash();
  delay(2000);
  display.invertDisplay(true);
  delay(500);
  display.invertDisplay(false);
  delay(1000);
  display.invertDisplay(true);
  delay(100);
  display.invertDisplay(false);
  delay(1000);

  Serial.print(F("float size in bytes: "));
  Serial.println(sizeof(float));
}




void loop() {
  demoMessage();

  for (int16_t x = 0; x < 128; ++x)
  {
    display.drawPixel(x, 40 + (int16_t)(5.0 * std::sin(0.5 * x)), 1);
    if (x > 0)
    {
      display.drawLine(x-1, 50 + (int16_t)(5.0 * std::sin(0.5 * (x-1))), x, 50 + (int16_t)(5.0 * std::sin(0.5 * x)), 1);
    }
    display.drawPixel(x, 59 + (int16_t)(5.0 * std::sin(0.5 * x)), 1);

    display.display();
    delay(100);

    sendCan();
    receiveCan();
    Message message;
    message.sequenceNumber = static_cast<uint8_t>(x);
    message.temperature = 10 * std::sin(x);
    sendCan(message);
  }
}



void sendCan()
{
  msg.len = 3;
  msg.id = 0x007;
  msg.buf[0] = 0x26;
  msg.buf[1] = 0x42;
  msg.buf[2] = 0x00;
  can0.write(msg);

  //msg.buf[2] = 0x01;
  //Can1.write(msg);
}

void sendCan(CAN_message_t& msg)
{
  msg.len = 3;
  msg.id = 0x42;
  msg.buf[0] = 0x42;
  msg.buf[1] = 0x42;
  msg.buf[2] = 0x42;
  can0.write(msg);

  //msg.buf[2] = 0x01;
  //Can1.write(msg);
}

CAN_message_t receiveCan() {
  static CAN_message_t msg,rxmsg;
  uint8_t data[8];
  while(can0.read(rxmsg))
  { 
     
     String CANStr(""); 
     for (int i=0; i < 8; i++) {     

         CANStr += String(rxmsg.buf[i],HEX);
         CANStr += (" ");
         data[i] = rxmsg.buf[i];

     }
     Serial.print(rxmsg.id,HEX); 
     Serial.print(' '); 
     Serial.print(rxmsg.len,HEX); 
     Serial.print(' ');
     Serial.println(CANStr);  
     
     display.fillRect(0,15,128, 30,BLACK);
     display.setCursor(0,15);
     
     display.println(rxmsg.id,HEX); 
     display.println(rxmsg.len,HEX); 
     display.println(CANStr);
     display.display();
     
     
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


void sendCan(const Message& message) {
    // Create a CAN message object (assuming CanMsg is a type you have defined)
    CAN_message_t msg;

    // Set the CAN message ID and length
    msg.id = 0x245;
    msg.len = 1 + sizeof(float);  // Will not work with double, as double on this platform is size 8, and msg.buf maximum size is 8.

    // Serialize the uint8_t field directly into the first byte of the buffer
    msg.buf[0] = message.sequenceNumber;

    // Serialize the float field into the remaining bytes of the buffer. Asume same endianness for all platforms.
    memcpy(&msg.buf[1], &message.temperature, sizeof(float));

    // Send the CAN message
    if (can0.write(msg) < 0)
    {
      Serial.println("CAN send failed.");
    }
}



void demoMessage(void) {
  draw_template();
  
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Hello world!"));
  delay(500);
  display.println(F("MAS245 gruppe 45"));
  display.println("");

  const __FlashStringHelper* message = F("using platformio...");
  const char* mp = (const char*)message;
  int len = strlen(mp);
  for(int i = 0; i < len; i++) {
      display.print(mp[i]);
      display.display();
      delay(100);  // Adjust this delay to speed up or slow down the "typewriter" effect
  }
  display.println();  // Move to the next line after the message is printed
  display.display();
}

void draw_template(){
  display.clearDisplay();
  display.drawCircle(30, 30, 10, SSD1306_WHITE);
  display.drawRoundRect(5, 5, 70, 40, 5, SSD1306_WHITE);
}

void drawSplash(void) {
  namespace splash = images::mas245splash;
  display.clearDisplay();
  display.drawBitmap(0, 0, splash::bitmap, splash::width, splash::height, 1);
  display.display();
}