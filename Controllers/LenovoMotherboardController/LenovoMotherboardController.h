/*---------------------------------------------------------*\
| LenovoMotherboardController.h                             |
|                                                           |
|   Driver for Lenovo motherboard                           |
|                                                           |
|   Morgan Guimard (morg)                       26 Dec 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include <hidapi.h>
#include "RGBController.h"

#define LENOVO_MB_PACKET_LENGTH     64
#define LENOVO_MB_REPORT_ID         0xCC

/*---------------------------------------------------------*\
| Zone IDs                                                  |
|   Legion Tower 5 Gen 8 and Gen 10 layout.  Zone 0x12 is   |
|   the top lights on the Gen 10 and the CPU fan on the     |
|   older Legion T5 this driver was first written for.      |
\*---------------------------------------------------------*/
#define LENOVO_MB_ZONE_FRONT_FANS_1 0x10
#define LENOVO_MB_ZONE_REAR_FAN     0x11
#define LENOVO_MB_ZONE_TOP          0x12
#define LENOVO_MB_ZONE_CPU_FAN      0x13
#define LENOVO_MB_ZONE_FRONT_FANS_2 0x14
#define LENOVO_MB_ZONE_LOGO         0x17
#define LENOVO_MB_ZONE_AMBIENT      0x18

/*---------------------------------------------------------*\
| Commands                                                  |
|   Lenovo's software sends both apply commands after every |
|   change.  Without them, newer towers ignore the zone     |
|   packets.  Sync (used by Lenovo's "cycle" effects) stays |
|   on until it is cleared, and while it is on the zones    |
|   take turns lighting up instead of all at once.          |
\*---------------------------------------------------------*/
#define LENOVO_MB_CMD_SYNC          0x42
#define LENOVO_MB_CMD_APPLY_1       0x2D
#define LENOVO_MB_CMD_APPLY_2       0x5E

/*---------------------------------------------------------*\
| The ambient light has no color and only supports Off,     |
| Static, Sparkle and Breathing.  Lenovo's software always  |
| sends it with speed 2.                                    |
\*---------------------------------------------------------*/
#define LENOVO_MB_AMBIENT_SPEED     0x02

enum
{
    LENOVO_MB_OFF_MODE              = 0x00,
    LENOVO_MB_STATIC_MODE           = 0x01,
    LENOVO_MB_SPARKLE_MODE          = 0x02,
    LENOVO_MB_BREATHING_MODE        = 0x03,
    LENOVO_MB_WAVE_MODE             = 0x04,
    LENOVO_MB_SPECTER_MODE          = 0x06,
    LENOVO_MB_RAINBOW_WAVE_MODE     = 0x09,
    LENOVO_MB_RANDOM_MODE           = 0x0A,
    LENOVO_MB_METEOR_MODE           = 0x0B,
    LENOVO_MB_RAINBOW_METEOR_MODE   = 0x0C
};

enum
{
    LENOVO_MB_BRIGHTNESS_MIN    = 1,
    LENOVO_MB_BRIGHTNESS_MAX    = 4,
    LENOVO_MB_SPEED_MIN         = 1,
    LENOVO_MB_SPEED_MAX         = 4,
};

class LenovoMotherboardController
{
public:
    LenovoMotherboardController(hid_device* dev_handle, const hid_device_info& info, std::string dev_name);
    ~LenovoMotherboardController();

    std::string GetDeviceLocation();
    std::string GetNameString();
    std::string GetSerialString();

    void SetMode(uint8_t zone, uint8_t mode, uint8_t brightness, uint8_t speed, RGBColor color);
    void DisableSync();
    void Apply();

private:
    hid_device* dev;
    std::string location;
    std::string name;

    void SendCommand(uint8_t command, uint8_t arg_1, uint8_t arg_2);
};
