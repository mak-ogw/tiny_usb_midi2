#ifndef MIDI2_PROPERTY_EXCHANGE_H_
#define MIDI2_PROPERTY_EXCHANGE_H_

static const uint8_t myNumSimul = 1;
static const uint8_t myMajVer = 0x00;
static const uint8_t myMinVer = 0x00;

static void handlePeCapabilities(MIDICI ciDetails, uint8_t numSimulRequests,
                                 uint8_t majVer, uint8_t minVer) {
    uint8_t sysexBuffer[512];
    const auto len = CIMessage::sendPECapabilityReply(
        sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid, ciDetails.remoteMUID,
        myNumSimul, myMajVer, myMinVer);
    sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
}

static void handlePeGetInquiry(MIDICI ciDetails, std::string requestDetails) {
    printf("%s\n", requestDetails.c_str());
}

static void handlePeSetInquiry(MIDICI ciDetails, std::string requestDetails,
                               uint16_t bodyLen, uint8_t* body,
                               bool lastByteOfChunk, bool lastByteOfSet) {}

static void handlePeSubInquiry(MIDICI ciDetails, std::string requestDetails,
                               uint16_t bodyLen, uint8_t* body,
                               bool lastByteOfChunk, bool lastByteOfSet) {}

static void initializePe() {
    MIDICIHandler.setPECapabilities(handlePeCapabilities);
    MIDICIHandler.setRecvPEGetInquiry(handlePeGetInquiry);
    MIDICIHandler.setRecvPESetInquiry(handlePeSetInquiry);
    MIDICIHandler.setRecvPESubInquiry(handlePeSubInquiry);
}

#endif  // MIDI2_PROPERTY_EXCHANGE_H_