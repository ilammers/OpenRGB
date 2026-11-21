/*---------------------------------------------------------*\
| LenovoMotherboardControllerDetect.cpp                     |
|                                                           |
|   Detector for Lenovo motherboard                         |
|                                                           |
|   Morgan Guimard (morg)                       26 Dec 2022 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-or-later               |
\*---------------------------------------------------------*/

#include "DetectionManager.h"
#include "dmiinfo.h"
#include "LenovoMotherboardController.h"
#include "RGBController_LenovoMotherboard.h"

/*---------------------------------------------------------*\
| vendor ID                                                 |
\*---------------------------------------------------------*/
#define LENOVO_MB_VID                                  0x17EF

/*---------------------------------------------------------*\
| Product ID                                                |
\*---------------------------------------------------------*/
#define LENOVO_MB_PID                                  0xC955

/*---------------------------------------------------------*\
| Boards tested to only write flash on the SAVE command, so |
| zone packets and APPLY can be sent on every update.  All  |
| boards share one USB ID, so the DMI board name is the     |
| only way to tell them apart.  Boards not listed here may  |
| save every change, so they get no Direct mode.            |
\*---------------------------------------------------------*/
static const char* lenovo_mb_manual_save_boards[] =
{
    "3787",                                     /* Legion T5 Gen 10 (30AGB10)   */
    "378B",                                     /* Legion T5 Gen 10 (30IAS10)   */
};

static bool LenovoMotherboardHasManualSave(const std::string& board)
{
    for(const char* manual_save_board : lenovo_mb_manual_save_boards)
    {
        if(board == manual_save_board)
        {
            return(true);
        }
    }

    return(false);
}

DetectedControllers DetectLenovoMotherboardControllers(hid_device_info* info, const std::string& name)
{
    DetectedControllers detected_controllers;
    hid_device*         dev;

    dev = hid_open_path(info->path);

    if(dev)
    {
        DMIInfo dmi;

        std::string                      board              = dmi.getMainboard();
        LenovoMotherboardController*     controller         = new LenovoMotherboardController(dev, *info, name + " " + board);
        RGBController_LenovoMotherboard* rgb_controller     = new RGBController_LenovoMotherboard(controller, LenovoMotherboardHasManualSave(board));

        detected_controllers.push_back(rgb_controller);
    }

    return(detected_controllers);
}

REGISTER_HID_DETECTOR_PU("Lenovo", DetectLenovoMotherboardControllers, LENOVO_MB_VID, LENOVO_MB_PID, 0xFF89, 0xCC);
