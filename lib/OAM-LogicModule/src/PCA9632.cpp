/**
 * Marlin 3D Printer Firmware
 * Copyright (C) 2016 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/*
 * Driver for the Philips PCA9632 LED driver.
 * Written by Robert Mendon Feb 2017.
 */


#include "pca9632.h"

#define PCA9632_MODE1_VALUE   0b00000001 //(ALLCALL)
#define PCA9632_MODE2_VALUE   0b00000001 //(DIMMING, NONINVERT, CHANGE ON STOP,OpenDrain)
#define PCA9632_LEDOUT_VALUE  0b00101010


/* Register addresses */
#define PCA9632_MODE1       0x00
#define PCA9632_MODE2       0x01
#define PCA9632_PWM0        0x02
#define PCA9632_PWM1        0x03
#define PCA9632_PWM2        0x04
#define PCA9632_PWM3        0x05
#define PCA9632_GRPPWM      0x06
#define PCA9632_GRPFREQ     0x07
#define PCA9632_LEDOUT      0X08
#define PCA9632_SUBADR1     0x09
#define PCA9632_SUBADR2     0x0A
#define PCA9632_SUBADR3     0x0B
#define PCA9632_ALLCALLADDR 0x0C

#define PCA9632_NO_AUTOINC  0x00
#define PCA9632_AUTO_ALL    0x80
#define PCA9632_AUTO_IND    0xA0
#define PCA9632_AUTOGLO     0xC0
#define PCA9632_AUTOGI      0xE0

// Red   LED0
// Green LED1
// Blue  LED2
#define PCA9632_RED     0x00
#define PCA9632_GRN     0x02
#define PCA9632_BLU     0x04

#define LED_OFF   0x00
#define LED_ON    0x01
#define LED_PWM   0x02

#define PCA9632_ADDRESS 0b01100000

byte PCA_init = 0;

static void PCA9632_WriteRegister(const byte addr, const byte regadd, const byte value) {
  Wire.beginTransmission(addr);
  Wire.write(regadd);
  Wire.write(value);
  Wire.endTransmission();
}

static void PCA9632_WriteAllRegisters(const byte addr, const byte regadd, const byte value1, const byte value2, const byte value3) {
  Wire.beginTransmission(addr);
  Wire.write(PCA9632_AUTO_IND | regadd);
  Wire.write(value1);
  Wire.write(value2);
  Wire.write(value3);
  Wire.endTransmission();
}

// static byte PCA9632_ReadRegister(const byte addr, const byte regadd) {
//   Wire.beginTransmission(addr);
//   Wire.write(regadd);
//   const byte value = Wire.read();
//   Wire.endTransmission();
//   return value;
// }

const uint16_t pwmtable[32] PROGMEM =
    {
        0, 1, 2, 2, 2, 3, 3, 4, 5, 6, 7, 8, 10, 11, 13, 16, 19, 23,
        27, 32, 38, 45, 54, 64, 76, 91, 108, 128, 152, 181, 215, 255};

void PCA9632_SetColor(const byte iB, const byte iG, const byte iR) {
  if (!PCA_init) {
    PCA_init = 1;
    // Wire.begin();
    PCA9632_WriteRegister(PCA9632_ADDRESS,PCA9632_MODE1, PCA9632_MODE1_VALUE);
    PCA9632_WriteRegister(PCA9632_ADDRESS,PCA9632_MODE2, PCA9632_MODE2_VALUE);
  }

  // we do logarithmic dimming here
  uint8_t lR = pwmtable[iR >> 3];
  uint8_t lG = pwmtable[iG >> 3];
  uint8_t lB = pwmtable[iB >> 3];

  const byte LEDOUT = (lR ? LED_PWM << PCA9632_RED : LED_OFF)
                    | (lG ? LED_PWM << PCA9632_GRN : LED_OFF)
                    | (lB ? LED_PWM << PCA9632_BLU : LED_OFF);

  PCA9632_WriteAllRegisters(PCA9632_ADDRESS,PCA9632_PWM0, lR, lG, lB);
  PCA9632_WriteRegister(PCA9632_ADDRESS,PCA9632_LEDOUT, LEDOUT);
}
