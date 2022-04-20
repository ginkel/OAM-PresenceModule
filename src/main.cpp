#include <OpenKNX.h>
#include "PMmodul.h"
#include "HardwareDevices.h"

uint8_t mSerial2Active = false;

void appSetup(bool iSaveSupported);
void appLoop();

void setup()
{
#ifdef ARDUINO_ARCH_RP2040
    Serial1.setRX(KNX_UART_RX_PIN);
    Serial1.setTX(KNX_UART_TX_PIN);
    Serial2.setRX(HF_UART_RX_PIN);
    Serial2.setTX(HF_UART_TX_PIN);
#endif
    SERIAL_DEBUG.begin(115200);
    pinMode(PROG_LED_PIN, OUTPUT);
    digitalWrite(PROG_LED_PIN, HIGH);
    delay(DEBUG_DELAY);
    digitalWrite(PROG_LED_PIN, LOW);
#ifdef HF_POWER_PIN
    pinMode(PRESENCE_LED_PIN, OUTPUT);
    pinMode(MOVE_LED_PIN, OUTPUT);
    pinMode(HF_S1_PIN, INPUT);
    pinMode(HF_S2_PIN, INPUT);
    pinMode(HF_POWER_PIN, OUTPUT);
#endif
    SERIAL_DEBUG.println("Startup called...");
    ArduinoPlatform::SerialDebug = &SERIAL_DEBUG;

#ifdef INFO_LED_PIN
  pinMode(INFO_LED_PIN, OUTPUT);
  ledInfo(true);
#endif

  // moved to checkBoard!!!
  // Wire.begin();
  // initialize knx-stack and check versions
  uint8_t firmwareResivion = 1;
  OpenKNX::knxRead(MAIN_OpenKnxId, MAIN_ApplicationNumber, MAIN_ApplicationVersion, firmwareResivion);

  // pin or GPIO the programming led is connected to. Default is LED_BUILDIN
  knx.ledPin(PROG_LED_PIN);
  // is the led active on HIGH or low? Default is LOW
  knx.ledPinActiveOn(PROG_LED_PIN_ACTIVE_ON);
  // pin or GPIO programming button is connected to. Default is 0
  knx.buttonPin(PROG_BUTTON_PIN);
  // Is the interrup created in RISING or FALLING signal? Default is RISING
  knx.buttonPinInterruptOn(PROG_BUTTON_PIN_INTERRUPT_ON);

  appSetup(SAVE_INTERRUPT_PIN);

  // start the framework.
  knx.start();
  ledInfo(false);
}

void loop()
{
  // don't delay here to much. Otherwise you might lose packages or mess up the timing with ETS
  knx.loop();

#ifdef HF_POWER_PIN
  // only run the application code if the device was configured with ETS
  if (knx.configured()) 
  {
    if (!mSerial2Active)
    {
      // we start HF communication as late as possible
      mSerial2Active = true;
      Serial2.begin(9600);
    }
    appLoop();
  }
  else
  {
    if (mSerial2Active) 
    {
        // during ETS programming, we stop HF communication
        mSerial2Active = false;
        Serial2.end();
    }
  }
#else
  appLoop();
#endif
}