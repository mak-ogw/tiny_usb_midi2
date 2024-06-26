#ifndef MIDI2_PROFILE_H_
#define MIDI2_PROFILE_H_

using ProfileIds = std::array<uint8_t, 5>;
#define PROFILE_DRAWBAR 0x7E, 0x20, 0x01, 0x01, 0x01
#define PROFILE_GM2_SINGLECH 0x7E, 0x20, 0x02, 0x01, 0x01
#define PROFILE_DEFAULT_CC 0x7E, 0x21, 0x00, 0x01, 0x01
#define PROFILE_ORCH_ART 0x7E, 0x21, 0x01, 0x01, 0x01
#define PROFILE_ROTARY_SPEAKER 0x7E, 0x22, 0x00, 0x01, 0x01
#define PROFILE_MPE 0x7E, 0x31, 0x00, 0x01, 0x01
#define PROFILE_GM2_FB 0x7E, 0x00, 0x00, 0x01, 0x01
static ProfileIds profileDrawbar{PROFILE_DRAWBAR};
static ProfileIds profileGm2SingleCh{PROFILE_GM2_SINGLECH};
static ProfileIds profileDefaultCc{PROFILE_DEFAULT_CC};
static ProfileIds profileOrchArt{PROFILE_ORCH_ART};
static ProfileIds profileRotarySpeaker{PROFILE_ROTARY_SPEAKER};
static ProfileIds profileMpe{PROFILE_MPE};
static ProfileIds profileGm2Fb{PROFILE_GM2_FB};
using ProfileOnOff = std::map<ProfileIds, bool>;
static std::map<uint8_t, ProfileOnOff> profileState;

static void makeEnabledProfileArray(std::vector<uint8_t> &v, uint8_t dest) {
    if (!((dest <= 0x0F) || (dest == 0x7E) || (dest == 0x7F))) {
        return;
    }
    const ProfileOnOff &profileOnOff = profileState[dest];
    for (const auto &[ids, isOn] : profileOnOff) {
        if (isOn) {
            for (auto &id : ids) {
                v.push_back(id);
            }
        }
    }
}
static void makeDisabledProfileArray(std::vector<uint8_t> &v, uint8_t dest) {
    if (!((dest <= 0x0F) || (dest == 0x7E) || (dest == 0x7F))) {
        return;
    }
    const ProfileOnOff &profileOnOff = profileState[dest];
    for (const auto &[ids, isOn] : profileOnOff) {
        if (!isOn) {
            for (auto &id : ids) {
                v.push_back(id);
            }
        }
    }
}

static void handleProfileInquiry(MIDICI ciDetails) {
    // FB, group, and all channel
    if (ciDetails.deviceId == 0x7F) {
        // Function Block Profile
        {
            uint8_t dest = 0x7F;
            std::vector<uint8_t> profilesEnabled, profilesDisabled;
            makeEnabledProfileArray(profilesEnabled, dest);
            makeDisabledProfileArray(profilesDisabled, dest);
            uint8_t sysexBuffer[512];
            const auto len = CIMessage::sendProfileListResponse(
                sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid,
                ciDetails.remoteMUID, dest,
                static_cast<uint8_t>(profilesEnabled.size() / 5),
                profilesEnabled.data(),
                static_cast<uint8_t>(profilesDisabled.size() / 5),
                profilesDisabled.data());
            sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
        }
        // Group
        {
            uint8_t dest = 0x7E;
            std::vector<uint8_t> profilesEnabled, profilesDisabled;
            makeEnabledProfileArray(profilesEnabled, dest);
            makeDisabledProfileArray(profilesDisabled, dest);
            uint8_t sysexBuffer[512];
            const auto len = CIMessage::sendProfileListResponse(
                sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid,
                ciDetails.remoteMUID, dest,
                static_cast<uint8_t>(profilesEnabled.size() / 5),
                profilesEnabled.data(),
                static_cast<uint8_t>(profilesDisabled.size() / 5),
                profilesDisabled.data());
            sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
        }
        // all channel
        for (uint8_t ch = 0; ch < 16; ++ch) {
            std::vector<uint8_t> profilesEnabled, profilesDisabled;
            makeEnabledProfileArray(profilesEnabled, ch);
            makeDisabledProfileArray(profilesDisabled, ch);
            uint8_t sysexBuffer[512];
            const auto len = CIMessage::sendProfileListResponse(
                sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid,
                ciDetails.remoteMUID, ch,
                static_cast<uint8_t>(profilesEnabled.size() / 5),
                profilesEnabled.data(),
                static_cast<uint8_t>(profilesDisabled.size() / 5),
                profilesDisabled.data());
            sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
        }
    }
    // Group and all channel
    else if (ciDetails.deviceId == 0x7E) {
        // Group
        {
            uint8_t dest = 0x7E;
            std::vector<uint8_t> profilesEnabled, profilesDisabled;
            makeEnabledProfileArray(profilesEnabled, dest);
            makeDisabledProfileArray(profilesDisabled, dest);
            uint8_t sysexBuffer[512];
            const auto len = CIMessage::sendProfileListResponse(
                sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid,
                ciDetails.remoteMUID, dest,
                static_cast<uint8_t>(profilesEnabled.size() / 5),
                profilesEnabled.data(),
                static_cast<uint8_t>(profilesDisabled.size() / 5),
                profilesDisabled.data());
            sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
        }
        // all channel
        for (uint8_t ch = 0; ch < 16; ++ch) {
            std::vector<uint8_t> profilesEnabled, profilesDisabled;
            makeEnabledProfileArray(profilesEnabled, ch);
            makeDisabledProfileArray(profilesDisabled, ch);
            uint8_t sysexBuffer[512];
            const auto len = CIMessage::sendProfileListResponse(
                sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid,
                ciDetails.remoteMUID, ch,
                static_cast<uint8_t>(profilesEnabled.size() / 5),
                profilesEnabled.data(),
                static_cast<uint8_t>(profilesDisabled.size() / 5),
                profilesDisabled.data());
            sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
        }
    }
    // Single channel
    else if (ciDetails.deviceId <= 0x0F) {
        std::vector<uint8_t> profilesEnabled, profilesDisabled;
        makeEnabledProfileArray(profilesEnabled, ciDetails.deviceId);
        makeDisabledProfileArray(profilesDisabled, ciDetails.deviceId);
        uint8_t sysexBuffer[512];
        const auto len = CIMessage::sendProfileListResponse(
            sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid,
            ciDetails.remoteMUID, ciDetails.deviceId,
            static_cast<uint8_t>(profilesEnabled.size() / 5),
            profilesEnabled.data(),
            static_cast<uint8_t>(profilesDisabled.size() / 5),
            profilesDisabled.data());
        sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
    }
}

static void handleProfileDetailsInquiry(MIDICI ciDetails,
                                        std::array<uint8_t, 5> profile,
                                        uint8_t InquiryTarget) {
    if (ciDetails.deviceId > 0x0F) {
        return;
    }
    // Not supported so reply NAK
    if (profileState.at(ciDetails.deviceId).count(profile) == 0) {
        uint8_t sysexBuffer[512];
        uint8_t nakDetails[5] = {0};
        const auto len =
            CIMessage::sendNAK(sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid,
                               ciDetails.remoteMUID, ciDetails.deviceId,
                               MIDICI_PROFILE_DETAILS_INQUIRY,
                               0x04,  // Profile
                               0x00,  // not supported.
                               nakDetails, 0, nullptr);
        sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
        return;
    }
    // InquiryTarget =0x00 : Number of MIDI Channels
    if (InquiryTarget == 0) {
        const auto isOn = profileState.at(ciDetails.deviceId).at(profile);
        uint16_t data[2] = {
            static_cast<uint16_t>(isOn ? 0x01 : 0x00),  // in use
            0x01,                                       // maximum
        };
        uint8_t sysexBuffer[512];
        const auto len = CIMessage::sendProfileDetailsReply(
            sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid,
            ciDetails.remoteMUID, ciDetails.deviceId, profile, InquiryTarget,
            static_cast<uint16_t>(sizeof(data)),
            reinterpret_cast<uint8_t *>(data));
        sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
        return;
    }
    // Optional Features Supported
    if (InquiryTarget == 1) {
        // Drawbar Organ Profiles
        if (profile == profileDrawbar) {
            uint8_t data[1] = {0b11111};
            uint8_t sysexBuffer[512];
            const auto len = CIMessage::sendProfileDetailsReply(
                sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid,
                ciDetails.remoteMUID, ciDetails.deviceId, profile,
                InquiryTarget, static_cast<uint16_t>(sizeof(data)), data);
            sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
            return;
        }
    }
}

static void handleProfileOn(MIDICI ciDetails, std::array<uint8_t, 5> profile,
                            uint8_t numberOfChannels) {
    if (!((ciDetails.deviceId <= 0x0F) || (ciDetails.deviceId == 0x7E) ||
          (ciDetails.deviceId == 0x7F))) {
        return;
    }
    auto &profileOnOff = profileState.at(ciDetails.deviceId);
    // no capability then ignore
    if (profileOnOff.count(profile) == 0) {
        return;
    }
    // enabled already then ignore
    if (profileOnOff.at(profile)) {
        return;
    }
    // set and reply enabled
    profileOnOff.at(profile) = true;
    uint8_t enabledChannels = 1;  // on this destination so it is 1.
    uint8_t sysexBuffer[512];
    const auto len = CIMessage::sendProfileEnabled(
        sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid, ciDetails.remoteMUID,
        ciDetails.deviceId, profile, enabledChannels);
    sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
}

static void handleProfileOff(MIDICI ciDetails, std::array<uint8_t, 5> profile) {
    if (!((ciDetails.deviceId <= 0x0F) || (ciDetails.deviceId == 0x7E) ||
          (ciDetails.deviceId == 0x7F))) {
        return;
    }
    auto &profileOnOff = profileState.at(ciDetails.deviceId);
    // no capability then ignore
    if (profileOnOff.count(profile) == 0) {
        return;
    }
    // disabled already then ignore
    if (!profileOnOff.at(profile)) {
        return;
    }
    // set and reply disabled
    profileOnOff.at(profile) = false;
    uint8_t enabledChannels = 0;  // on this destination so it is 0.
    uint8_t sysexBuffer[512];
    const auto len = CIMessage::sendProfileDisabled(
        sysexBuffer, MIDICI_MESSAGEFORMATVERSION, myMuid, ciDetails.remoteMUID,
        ciDetails.deviceId, profile, enabledChannels);
    sendOutSysex(ciDetails.umpGroup, sysexBuffer, len, 0);
}

static void initializeProfile() {
    for (uint8_t ch = 0; ch < 16; ++ch) {
        profileState[ch][profileDrawbar] = false;
        profileState[ch][profileGm2SingleCh] = false;
        profileState[ch][profileDefaultCc] = false;
        profileState[ch][profileOrchArt] = false;
        profileState[ch][profileRotarySpeaker] = false;
        profileState[ch][profileMpe] = false;
    }
    profileState[0x7E] = ProfileOnOff();  // empty
    profileState[0x7F][profileGm2Fb] = false;

    MIDICIHandler.setRecvProfileInquiry(handleProfileInquiry);
    MIDICIHandler.setRecvProfileOn(handleProfileOn);
    MIDICIHandler.setRecvProfileOff(handleProfileOff);
    MIDICIHandler.setRecvProfileDetailsInquiry(handleProfileDetailsInquiry);
}

#endif  // MIDI2_PROFILE_H_