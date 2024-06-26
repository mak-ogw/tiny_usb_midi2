#ifndef MIDI2_H_
#define MIDI2_H_

#include "AM_MIDI2.0Lib/include/midiCIProcessor.h"
#include "AM_MIDI2.0Lib/include/midiCIMessageCreate.h"
#include "AM_MIDI2.0Lib/include/umpMessageCreate.h"
#include "AM_MIDI2.0Lib/include/umpProcessor.h"
#include "pico/unique_id.h"

static umpProcessor UMPHandler;
static midiCIProcessor MIDICIHandler;
static uint32_t myMuid;

static uint32_t random(uint32_t max) {
    return (rand() % static_cast<uint32_t>(max + 1));
}
template <std::size_t N>
static void sendUMP(std::array<uint32_t, N> UMP) {
    tud_ump_write(0, UMP.data(), N);
}

static uint8_t sxPos = 0;
static std::array<uint8_t, 6> sx;
static void sendOutSysex(uint8_t group, uint8_t *sysex, uint16_t length,
                         uint8_t state) {
    if (state < 2) {
        sxPos = 0;
    }
    for (int i = 0; i < length; i++) {
        sx[sxPos++] = sysex[i] & 0x7F;
        if (sxPos == 6) {
            sendUMP(UMPMessage::mt3Sysex7(group, state < 2 && i < 6 ? 1 : 2, 6,
                                          sx));
            sxPos = 0;
        }
    }
    if (state == 0 || state == 3) {
        sendUMP(UMPMessage::mt3Sysex7(group, length < 7 && state == 0 ? 0 : 3,
                                      sxPos, sx));
    }
}

// clang-format off
#include "midi2_stream.h"
#include "midi2_ci.h"
// clang-format on

static void initializeMidi2() {
    initializeStream();
    initializeMidiCi();
}

#endif  // MIDI2_H_