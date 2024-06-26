#ifndef MIDI_CAPABILITY_INQUIRY_H_
#define MIDI_CAPABILITY_INQUIRY_H_

// clang-format off
#include "midi2_pe.h"
#include "midi2_profile.h"
// clang-format on

#define CI_SUPPORT 0b01100  // PI, PE, Prof, PN, rsv

static void processUMPSysex(struct umpData mess) {
    static bool isProcMIDICI = false;
    // Example of Processing UMP into MIDI-CI processor
    if (mess.form == 1 && mess.data[0] == S7UNIVERSAL_NRT &&
        mess.data[2] == S7MIDICI) {
        if (mess.umpGroup == 0) {
            MIDICIHandler.startSysex7(mess.umpGroup, mess.data[1]);
            isProcMIDICI = true;
        }
    }
    for (int i = 0; i < mess.dataLength; i++) {
        if (mess.umpGroup == 0 && isProcMIDICI) {
            MIDICIHandler.processMIDICI(mess.data[i]);
        } else {
            // Process other SysEx
        }
    }
    if ((mess.form == 3 || mess.form == 0) && isProcMIDICI) {
        MIDICIHandler.endSysex7();
        isProcMIDICI = false;
    }
}

static bool checkMUIDCallback(uint8_t umpGroup, uint32_t muid) {
    return (myMuid == muid);
}

static void recvDiscovery(struct MIDICI ciDetails,
                          std::array<uint8_t, 3> manuId,
                          std::array<uint8_t, 2> familyId,
                          std::array<uint8_t, 2> modelId,
                          std::array<uint8_t, 4> version,
                          uint8_t remoteciSupport, uint16_t remotemaxSysex,
                          uint8_t outputPathId) {
    // All MIDI-CI Devices shall reply to a Discovery message
    printf("Received Discover on Group %d remote MUID: %d\n",
           ciDetails.umpGroup, ciDetails.remoteMUID);
    uint8_t sysexBuffer[64];
    const auto len = CIMessage::sendDiscoveryReply(
        sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid, ciDetails.remoteMUID,
        {DEVICE_MFRID}, {DEVICE_FAMID}, {DEVICE_MODELID}, {DEVICE_VERSIONID},
        CI_SUPPORT, 512, outputPathId,
        0  // fbIdx
    );
    sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
}

static void initializeMidiCi() {
    UMPHandler.setSysEx(processUMPSysex);
    myMuid = random(0xFFFFEFF);
    MIDICIHandler.setCheckMUID(checkMUIDCallback);
    MIDICIHandler.setRecvDiscovery(recvDiscovery);

    initializeProfile();
    initializePe();
}

#endif  // MIDI_CAPABILITY_INQUIRY_H_