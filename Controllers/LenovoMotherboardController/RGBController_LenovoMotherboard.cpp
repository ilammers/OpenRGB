/*---------------------------------------------------------*\
| RGBController_LenovoMotherboard.cpp                       |
|                                                           |
|   RGBController for Lenovo motherboard                    |
|                                                           |
|   Morgan Guimard (morg)                       26 Dec 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include <chrono>
#include <thread>
#include "RGBController_LenovoMotherboard.h"

struct lenovo_mb_zone
{
    const char*     name;
    unsigned char   id;
};

#define LENOVO_MB_NUMBER_OF_ZONES   7

static const lenovo_mb_zone lenovo_mb_zones[LENOVO_MB_NUMBER_OF_ZONES] =
{
    { "CPU Fan",        LENOVO_MB_ZONE_CPU_FAN      },
    { "Rear Fan",       LENOVO_MB_ZONE_REAR_FAN     },
    { "Top Lights",     LENOVO_MB_ZONE_TOP          },
    { "Front Fans 1",   LENOVO_MB_ZONE_FRONT_FANS_1 },
    { "Front Fans 2",   LENOVO_MB_ZONE_FRONT_FANS_2 },
    { "Logo",           LENOVO_MB_ZONE_LOGO         },
    { "Ambient Light",  LENOVO_MB_ZONE_AMBIENT      },
};

/**------------------------------------------------------------------*\
    @name LenovoMotherboard mouse
    @category Motherboard
    @type USB
    @save :x:
    @direct :x:
    @effects :white_check_mark:
    @detectors DetectLenovoMotherboardControllers
    @comment
\*-------------------------------------------------------------------*/

RGBController_LenovoMotherboard::RGBController_LenovoMotherboard(LenovoMotherboardController* controller_ptr)
{
    controller                  = controller_ptr;

    name                        = controller->GetNameString();
    vendor                      = "Lenovo";
    type                        = DEVICE_TYPE_MOTHERBOARD;
    description                 = name;
    location                    = controller->GetDeviceLocation();
    serial                      = controller->GetSerialString();

    mode Static;
    Static.name                 = "Static";
    Static.value                = LENOVO_MB_STATIC_MODE;
    Static.flags                = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS;
    Static.color_mode           = MODE_COLORS_PER_LED;
    Static.brightness_min       = LENOVO_MB_BRIGHTNESS_MIN;
    Static.brightness_max       = LENOVO_MB_BRIGHTNESS_MAX;
    Static.brightness           = LENOVO_MB_BRIGHTNESS_MAX;
    Static.speed                = LENOVO_MB_SPEED_MIN;
    modes.push_back(Static);

    mode Sparkle;
    Sparkle.name                = "Sparkle";
    Sparkle.value               = LENOVO_MB_SPARKLE_MODE;
    Sparkle.flags               = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Sparkle.color_mode          = MODE_COLORS_PER_LED;
    Sparkle.brightness_min      = LENOVO_MB_BRIGHTNESS_MIN;
    Sparkle.brightness_max      = LENOVO_MB_BRIGHTNESS_MAX;
    Sparkle.brightness          = LENOVO_MB_BRIGHTNESS_MAX;
    Sparkle.speed               = LENOVO_MB_SPEED_MIN;
    Sparkle.speed_max           = LENOVO_MB_SPEED_MAX;
    Sparkle.speed_min           = LENOVO_MB_SPEED_MIN;
    modes.push_back(Sparkle);

    mode Breathing;
    Breathing.name              = "Breathing";
    Breathing.value             = LENOVO_MB_BREATHING_MODE;
    Breathing.flags             = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Breathing.color_mode        = MODE_COLORS_PER_LED;
    Breathing.brightness_min    = LENOVO_MB_BRIGHTNESS_MIN;
    Breathing.brightness_max    = LENOVO_MB_BRIGHTNESS_MAX;
    Breathing.brightness        = LENOVO_MB_BRIGHTNESS_MAX;
    Breathing.speed             = LENOVO_MB_SPEED_MIN;
    Breathing.speed_max         = LENOVO_MB_SPEED_MAX;
    Breathing.speed_min         = LENOVO_MB_SPEED_MIN;
    modes.push_back(Breathing);

    mode Wave;
    Wave.name                   = "Wave";
    Wave.value                  = LENOVO_MB_WAVE_MODE;
    Wave.flags                  = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Wave.color_mode             = MODE_COLORS_NONE;
    Wave.brightness_min         = LENOVO_MB_BRIGHTNESS_MIN;
    Wave.brightness_max         = LENOVO_MB_BRIGHTNESS_MAX;
    Wave.brightness             = LENOVO_MB_BRIGHTNESS_MAX;
    Wave.speed                  = LENOVO_MB_SPEED_MIN;
    Wave.speed_max              = LENOVO_MB_SPEED_MAX;
    Wave.speed_min              = LENOVO_MB_SPEED_MIN;
    modes.push_back(Wave);

    mode Specter;
    Specter.name                = "Specter";
    Specter.value               = LENOVO_MB_SPECTER_MODE;
    Specter.flags               = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Specter.color_mode          = MODE_COLORS_PER_LED;
    Specter.brightness_min      = LENOVO_MB_BRIGHTNESS_MIN;
    Specter.brightness_max      = LENOVO_MB_BRIGHTNESS_MAX;
    Specter.brightness          = LENOVO_MB_BRIGHTNESS_MAX;
    Specter.speed               = LENOVO_MB_SPEED_MIN;
    Specter.speed_max           = LENOVO_MB_SPEED_MAX;
    Specter.speed_min           = LENOVO_MB_SPEED_MIN;
    modes.push_back(Specter);

    mode Rainbow;
    Rainbow.name                = "Rainbow wave";
    Rainbow.value               = LENOVO_MB_RAINBOW_WAVE_MODE;
    Rainbow.flags               = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Rainbow.color_mode          = MODE_COLORS_NONE;
    Rainbow.brightness_min      = LENOVO_MB_BRIGHTNESS_MIN;
    Rainbow.brightness_max      = LENOVO_MB_BRIGHTNESS_MAX;
    Rainbow.brightness          = LENOVO_MB_BRIGHTNESS_MAX;
    Rainbow.speed               = LENOVO_MB_SPEED_MIN;
    Rainbow.speed_max           = LENOVO_MB_SPEED_MAX;
    Rainbow.speed_min           = LENOVO_MB_SPEED_MIN;
    modes.push_back(Rainbow);

    mode Random;
    Random.name                 = "Random";
    Random.value                = LENOVO_MB_RANDOM_MODE;
    Random.flags                = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Random.color_mode           = MODE_COLORS_NONE;
    Random.brightness_min       = LENOVO_MB_BRIGHTNESS_MIN;
    Random.brightness_max       = LENOVO_MB_BRIGHTNESS_MAX;
    Random.brightness           = LENOVO_MB_BRIGHTNESS_MAX;
    Random.speed                = LENOVO_MB_SPEED_MIN;
    Random.speed_max            = LENOVO_MB_SPEED_MAX;
    Random.speed_min            = LENOVO_MB_SPEED_MIN;
    modes.push_back(Random);

    mode Meteor;
    Meteor.name                 = "Meteor";
    Meteor.value                = LENOVO_MB_METEOR_MODE;
    Meteor.flags                = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    Meteor.color_mode           = MODE_COLORS_PER_LED;
    Meteor.brightness_min       = LENOVO_MB_BRIGHTNESS_MIN;
    Meteor.brightness_max       = LENOVO_MB_BRIGHTNESS_MAX;
    Meteor.brightness           = LENOVO_MB_BRIGHTNESS_MAX;
    Meteor.speed                = LENOVO_MB_SPEED_MIN;
    Meteor.speed_max            = LENOVO_MB_SPEED_MAX;
    Meteor.speed_min            = LENOVO_MB_SPEED_MIN;
    modes.push_back(Meteor);

    mode RainbowMeteor;
    RainbowMeteor.name          = "Rainbow meteor";
    RainbowMeteor.value         = LENOVO_MB_RAINBOW_METEOR_MODE;
    RainbowMeteor.flags         = MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED;
    RainbowMeteor.color_mode    = MODE_COLORS_NONE;
    RainbowMeteor.brightness_min = LENOVO_MB_BRIGHTNESS_MIN;
    RainbowMeteor.brightness_max = LENOVO_MB_BRIGHTNESS_MAX;
    RainbowMeteor.brightness    = LENOVO_MB_BRIGHTNESS_MAX;
    RainbowMeteor.speed         = LENOVO_MB_SPEED_MIN;
    RainbowMeteor.speed_max     = LENOVO_MB_SPEED_MAX;
    RainbowMeteor.speed_min     = LENOVO_MB_SPEED_MIN;
    modes.push_back(RainbowMeteor);

    mode Off;
    Off.name                    = "Off";
    Off.value                   = LENOVO_MB_OFF_MODE;
    Off.flags                   = 0;
    Off.color_mode              = MODE_COLORS_NONE;
    Off.brightness              = LENOVO_MB_BRIGHTNESS_MIN;
    Off.speed                   = LENOVO_MB_SPEED_MIN;
    modes.push_back(Off);

    SetupZones();
}

RGBController_LenovoMotherboard::~RGBController_LenovoMotherboard()
{
    Shutdown();

    delete controller;
}

void RGBController_LenovoMotherboard::SetupZones()
{
    /*-----------------------------------------------------*\
    | Each zone is a single LED whose value is the zone ID  |
    \*-----------------------------------------------------*/
    for(unsigned int zone_idx = 0; zone_idx < LENOVO_MB_NUMBER_OF_ZONES; zone_idx++)
    {
        zone new_zone;

        new_zone.name           = lenovo_mb_zones[zone_idx].name;
        new_zone.type           = ZONE_TYPE_SINGLE;
        new_zone.leds_min       = 1;
        new_zone.leds_max       = 1;
        new_zone.leds_count     = 1;

        zones.push_back(new_zone);

        led new_led;

        new_led.name            = lenovo_mb_zones[zone_idx].name;
        new_led.value           = lenovo_mb_zones[zone_idx].id;

        leds.push_back(new_led);
    }

    SetupColors();
}

void RGBController_LenovoMotherboard::DeviceUpdateLEDs()
{
    for(unsigned int led_idx = 0; led_idx < leds.size(); led_idx++)
    {
        unsigned char zone_mode     = modes[active_mode].value;
        unsigned char speed         = modes[active_mode].speed;
        RGBColor      color         = colors[led_idx];

        /*-------------------------------------------------*\
        | The ambient light has no color.  Turn it off when |
        | its color is black and fall back to Static for    |
        | effects it can't show.                            |
        \*-------------------------------------------------*/
        if(leds[led_idx].value == LENOVO_MB_ZONE_AMBIENT)
        {
            if(color == 0)
            {
                zone_mode           = LENOVO_MB_OFF_MODE;
            }
            else if(zone_mode != LENOVO_MB_OFF_MODE
                 && zone_mode != LENOVO_MB_SPARKLE_MODE
                 && zone_mode != LENOVO_MB_BREATHING_MODE)
            {
                zone_mode           = LENOVO_MB_STATIC_MODE;
            }

            speed                   = LENOVO_MB_AMBIENT_SPEED;
            color                   = 0;
        }

        controller->SetMode(
                    leds[led_idx].value,
                    zone_mode,
                    modes[active_mode].brightness,
                    speed,
                    color
                    );
    }

    controller->DisableSync();
    controller->Apply();
}

void RGBController_LenovoMotherboard::DeviceUpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_LenovoMotherboard::DeviceUpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_LenovoMotherboard::DeviceUpdateMode()
{
    DeviceUpdateLEDs();
}
