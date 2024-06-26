/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Copyright (c) 2022 Michael Loh (AmeNote.com)
 * Copyright (c) 2022 Franz Detro (native-instruments.de)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

// clang-format off
#include "pico/unique_id.h"
#include "tusb.h"
#include "tusb_ump/ump_device.h"
#include "descriptor/usb_descriptors.h"
// clang-format on

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const* tud_descriptor_device_cb(void) {
    return (uint8_t const*)&desc_device;
}

//--------------------------------------------------------------------+
// Configuration Descriptor
//--------------------------------------------------------------------+

enum {
    ITF_NUM_CDC = 0,
    ITF_NUM_CDC_DATA,
    ITF_NUM_MIDI,
    ITF_NUM_MIDI_STREAMING,
    ITF_NUM_TOTAL
};

#define EPNUM_CDC_NOTIF 0x81
#define EPNUM_CDC_OUT 0x02
#define EPNUM_CDC_IN 0x82
#define EPNUM_MIDI 0x03

// The number of Terminal Group Blocks
#define NUM_TGBSS 3
#define CD_wTotalLeng 245 + (NUM_TGBSS * 2)
#define CD_wTotalLengthLSB (uint8_t)(CD_wTotalLeng & 0xFF)
#define CD_wTotalLengthMSB (uint8_t)((CD_wTotalLeng >> 8) & 0xFF)
#define MS_wTotalLeng 97  //+(NUM_TGBSS*2)
#define MS_wTotalLengthLSB (uint8_t)(MS_wTotalLeng & 0xFF)
#define MS_wTotalLengthMSB (uint8_t)((MS_wTotalLeng >> 8) & 0xFF)

constexpr unsigned cgfsize = sizeof(desc_fs_configuration);

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const* tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;  // for multiple configurations

    return desc_fs_configuration;
}

// Invoked when received GET DEVICE QUALIFIER DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete. device_qualifier descriptor describes
// information about a high-speed capable device that would change if the device
// were operating at the other speed. If not highspeed capable stall this
// request.
uint8_t const* tud_descriptor_device_qualifier_cb(void) {
    return (uint8_t const*)&desc_device_qualifier;
}

//--------------------------------------------------------------------+
// String Descriptors
//--------------------------------------------------------------------+

static uint16_t _desc_str[32];

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long
// enough for transfer to complete
uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    (void)langid;

    int len = 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1;
    char serialId[len];
    pico_get_unique_board_id_string(serialId, len);

    uint8_t chr_count;

    if (index == 0) {
        memcpy(&_desc_str[1], string_desc_arr[0], 2);
        chr_count = 1;
    } else {
        // Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
        // https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

        if (!(index < string_desc_arr_length)) return NULL;

        const char* str = string_desc_arr[index];

        // Cap at max char
        chr_count = strlen(str) > 255 ? 255 : static_cast<uint8_t>(strlen(str));
        if (chr_count > 31) chr_count = 31;

        // Convert ASCII string into UTF-16
        for (uint8_t i = 0; i < chr_count; i++) {
            _desc_str[1 + i] = str[i];
        }
    }

    // first byte is length (including header), second byte is string type
    _desc_str[0] =
        static_cast<uint16_t>((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

    return _desc_str;
}

//--------------------------------------------------------------------+
// Group Terminal Block Descriptor
//--------------------------------------------------------------------+

bool tud_ump_get_req_itf_cb(uint8_t rhport,
                            tusb_control_request_t const* request) {
    if (request->wValue ==
        0x2601)  // 0x26 - CS_GR_TRM_BLOCK 0x01 - alternate interface setting
    {
        const auto gtbNum = sizeof(gtbLengths) / sizeof(uint8_t);
        uint16_t totalLength = 0;
        for (size_t i = 0; i < gtbNum; ++i) {
            totalLength += gtbLengths[i];
        }

        uint16_t length = request->wLength;
        if (length > totalLength) {
            length = totalLength;
        }

        void* p =
            const_cast<void*>(reinterpret_cast<const void*>(group_descr[0]));

        tud_control_xfer(rhport, request, p, length);
        return true;
    } else
        return false;
}
