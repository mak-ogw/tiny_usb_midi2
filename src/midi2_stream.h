#ifndef MIDI2_STREAM_H_
#define MIDI2_STREAM_H_

#define C_STR_TO_UINT8(s) reinterpret_cast<uint8_t *>(const_cast<char *>(s))

#define DEVICE_MFRID 0x60, 0x7F, 0x7F  // 60-7F is reserved
#define DEVICE_FAMID 0x00, 0x00
#define DEVICE_MODELID 0x00, 0x00
#define DEVICE_VERSIONID 1, 2, 3, 4
#define MIDICI_MESSAGEFORMATVERSION 0x02
// refer to string_desc_arr in usb_descriptors.h
#define DEVICE_MIDIENPOINTNAME "EP_TinyUsbMidi2"
#define FB_NUM 1
#define FB_1_NAME "FB1"

static void midiendpoint(uint8_t majVer, uint8_t minVer, uint8_t filter) {
    if (filter & MIDIENDPOINT_INFO_NOTIFICATION) {
        sendUMP(UMPMessage::mtFMidiEndpointInfoNotify(
            FB_NUM,   // uint8_t numOfFuncBlock
            true,     // bool m2
            true,     // bool m1
            false,    // bool rxjr
            false));  // bool txjr
    }
    if (filter & MIDIENDPOINT_DEVICEINFO_NOTIFICATION) {
        sendUMP(UMPMessage::mtFMidiEndpointDeviceInfoNotify(
            {DEVICE_MFRID},        // manuId
            {DEVICE_FAMID},        // familyId
            {DEVICE_MODELID},      // modelId
            {DEVICE_VERSIONID}));  // version
    }
    if (filter & MIDIENDPOINT_NAME_NOTIFICATION) {
        const auto textLen = strlen(DEVICE_MIDIENPOINTNAME);
        for (uint8_t offset = 0; offset < textLen; offset += 14) {
            sendUMP(UMPMessage::mtFMidiEndpointTextNotify(
                MIDIENDPOINT_NAME_NOTIFICATION,          // uint16_t replyType
                offset,                                  // uint8_t offset
                C_STR_TO_UINT8(DEVICE_MIDIENPOINTNAME),  // uint8_t *text
                textLen));                               // uint8_t textLen
        }
    }
    if (filter & MIDIENDPOINT_PRODID_NOTIFICATION) {
        int textLen = 2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1;
        uint8_t buff[textLen];
        pico_get_unique_board_id_string((char *)buff, textLen);
        for (uint8_t offset = 0; offset < textLen; offset += 14) {
            sendUMP(UMPMessage::mtFMidiEndpointTextNotify(
                MIDIENDPOINT_PRODID_NOTIFICATION,  // uint16_t replyType
                offset,                            // uint8_t offset
                (uint8_t *)buff,                   // uint8_t *text
                textLen));                         // uint8_t textLen
        }
    }
}

static void handleJrProtocolRequest(uint8_t protocol, bool jrrx, bool jrtx) {
    // TEMP: follow requested protocol, jrrx, jrtx.
    sendUMP(UMPMessage::mtFNotifyProtocol(protocol, jrrx, jrtx));
}

static void functionblock(uint8_t fbIdx, uint8_t filter) {
    if (fbIdx >= FB_NUM && fbIdx != 0xFF) return;
    if (filter & 0x1) {
        if (fbIdx == 0 || fbIdx == 0xFF) {
            sendUMP(UMPMessage::mtFFunctionBlockInfoNotify(
                0,     // uint8_t fbIdx
                true,  // bool active
                3,     // uint8_t direction
                true,  // bool sender
                true,  // bool recv
                0,     // uint8_t firstGroup
                1,     // uint8_t groupLength
                0x01,  // uint8_t midiCISupport
                0,     // uint8_t isMIDI1
                0));   // uint8_t maxS8Streams
        }
    }
    if (filter & 0x2) {
        if (fbIdx == 0 || fbIdx == 0xFF) {
            sendUMP(UMPMessage::mtFFunctionBlockNameNotify(
                1,                          // uint8_t fbIdx
                0,                          // uint8_t offset
                C_STR_TO_UINT8(FB_1_NAME),  // uint8_t *text
                strlen(FB_1_NAME)));        // uint8_t textLen
        }
    }
}

static void initializeStream() {
    UMPHandler.setMidiEndpoint(midiendpoint);
    UMPHandler.setJRProtocolRequest(handleJrProtocolRequest);
    UMPHandler.setFunctionBlock(functionblock);
}

#endif  // MIDI2_STREAM_H_