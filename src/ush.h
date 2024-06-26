#ifndef USH_H
#define USU_H

#include <array>

#include "microshell.h"

// "protocol"
static void printProtocol(struct ush_object *self,
                          struct ush_file_descriptor const *file, int argc,
                          char *argv[]) {
    const auto alt = tud_alt_setting(0);
    if (alt == 0) {
        printf("Current Protocol is USB-MIDI1.0.\r\n");
    } else if (alt == 1) {
        printf("Current Protocol is USB-MIDI2.0.\r\n");
    } else {
        printf("There is a problem, alt = %d.\r\n", alt);
    }
}

// noteon1: send MT=2 noteOn
static void noteon1(struct ush_object *self,
                    struct ush_file_descriptor const *file, int argc,
                    char *argv[]) {
    if (argc != 5) {
        printf(
            "noteon1(send MT=2 noteOn): Wrong arguments.\r\n"
            "usage: noteon1 [group] [channel] [note] [vel]. (hex values)\r\n"
            "Example > noteon1 0 0 3C 64\r\n");
        return;
    }
    // TODO: range check. e.g. midi channel must be from 0 to 15.
    const uint32_t msgType = 0x20000000;
    const uint32_t status = 0x00900000;
    const uint32_t group =
        (static_cast<uint32_t>(strtoul(argv[1], NULL, 16)) << 24);
    const uint32_t channel =
        (static_cast<uint32_t>(strtoul(argv[2], NULL, 16)) << 16);
    const uint32_t note =
        (static_cast<uint32_t>(strtoul(argv[3], NULL, 16)) << 8);
    const uint32_t vel = (static_cast<uint32_t>(strtoul(argv[4], NULL, 16)));
    std::array<uint32_t, 1> msg = {msgType + status + group + channel + note +
                                   vel};
    sendUMP(msg);
}

// noteoff1: send MT=2 noteOff
static void noteoff1(struct ush_object *self,
                     struct ush_file_descriptor const *file, int argc,
                     char *argv[]) {
    if (argc != 5) {
        printf(
            "noteoff1(send MT=2 noteOff): Wrong arguments.\r\n"
            "usage: noteoff1 [group] [channel] [note] [vel]. (hex values)\r\n"
            "Example > noteoff1 0 0 3C 0\r\n");
        return;
    }
    // TODO: range check. e.g. midi channel must be from 0 to 15.
    const uint32_t msgType = 0x20000000;
    const uint32_t status = 0x00800000;
    const uint32_t group =
        (static_cast<uint32_t>(strtoul(argv[1], NULL, 16)) << 24);
    const uint32_t channel =
        (static_cast<uint32_t>(strtoul(argv[2], NULL, 16)) << 16);
    const uint32_t note =
        (static_cast<uint32_t>(strtoul(argv[3], NULL, 16)) << 8);
    const uint32_t vel = (static_cast<uint32_t>(strtoul(argv[4], NULL, 16)));
    std::array<uint32_t, 1> msg = {msgType + status + group + channel + note +
                                   vel};
    sendUMP(msg);
}

// noteon2: send MT=4 noteOn
static void noteon2(struct ush_object *self,
                    struct ush_file_descriptor const *file, int argc,
                    char *argv[]) {
    if (argc != 5) {
        printf(
            "noteon2(send MT=4 noteOn): Wrong arguments.\r\n"
            "usage: noteon2 [group] [channel] [note] [vel]. (hex values)\r\n"
            "Example > noteon2 0 0 3C 8000\r\n");
        return;
    }
    // TODO: range check. e.g. midi channel must be from 0 to 15.
    const uint32_t msgType = 0x40000000;
    const uint32_t status = 0x00900000;
    const uint32_t group =
        (static_cast<uint32_t>(strtoul(argv[1], NULL, 16)) << 24);
    const uint32_t channel =
        (static_cast<uint32_t>(strtoul(argv[2], NULL, 16)) << 16);
    const uint32_t note =
        (static_cast<uint32_t>(strtoul(argv[3], NULL, 16)) << 8);
    const uint32_t vel =
        (static_cast<uint32_t>(strtoul(argv[4], NULL, 16)) << 16);
    std::array<uint32_t, 2> msg = {msgType + status + group + channel + note,
                                   vel};
    sendUMP(msg);
}

// noteoff2: send MT=4 noteOn
static void noteoff2(struct ush_object *self,
                     struct ush_file_descriptor const *file, int argc,
                     char *argv[]) {
    if (argc != 5) {
        printf(
            "noteoff2(send MT=4 noteOff): Wrong arguments.\r\n"
            "usage: noteoff2 [group] [channel] [note] [vel]. (hex values)\r\n"
            "Example > noteoff2 0 0 3C 7FFF\r\n");
        return;
    }
    // TODO: range check. e.g. midi channel must be from 0 to 15.
    const uint32_t msgType = 0x40000000;
    const uint32_t status = 0x00800000;
    const uint32_t group =
        (static_cast<uint32_t>(strtoul(argv[1], NULL, 16)) << 24);
    const uint32_t channel =
        (static_cast<uint32_t>(strtoul(argv[2], NULL, 16)) << 16);
    const uint32_t note =
        (static_cast<uint32_t>(strtoul(argv[3], NULL, 16)) << 8);
    const uint32_t vel =
        (static_cast<uint32_t>(strtoul(argv[4], NULL, 16)) << 16);
    std::array<uint32_t, 2> msg = {msgType + status + group + channel + note,
                                   vel};
    sendUMP(msg);
}

// non-blocking read interface
static int ush_read(struct ush_object *self, char *ch) {
    // should be implemented as a FIFO
    while (tud_cdc_available() > 0) {
        tud_cdc_read(ch, 1);
        return 1;
    }
    return 0;
}

// non-blocking write interface
static int ush_write(struct ush_object *self, char ch) {
    int rtn = tud_cdc_write(&ch, 1);
    tud_cdc_write_flush();
    // should be implemented as a FIFO
    return (rtn == 1);
}

// I/O interface descriptor
static const struct ush_io_interface ush_iface = {
    .read = ush_read,
    .write = ush_write,
};

// working buffers allocations (size could be customized)
#define BUF_IN_SIZE 32
#define BUF_OUT_SIZE 32
#define PATH_MAX_SIZE 32

static char ush_in_buf[BUF_IN_SIZE];
static char ush_out_buf[BUF_OUT_SIZE];

// microshell instance handler
static struct ush_object ush;
// root directory handler
static struct ush_node_object root;
// cmd commands handler
static struct ush_node_object cmd;

// microshell descriptor
static const struct ush_descriptor ush_desc = {
    .io = &ush_iface,                           // I/O interface pointer
    .input_buffer = ush_in_buf,                 // working input buffer
    .input_buffer_size = sizeof(ush_in_buf),    // working input buffer size
    .output_buffer = ush_out_buf,               // working output buffer
    .output_buffer_size = sizeof(ush_out_buf),  // working output buffer size
    .path_max_length = PATH_MAX_SIZE,           // path maximum length (stack)
    .hostname = const_cast<char *>(""),         // hostname (in prompt)
    .exec = nullptr,
};

// cmd files descriptor
static const struct ush_file_descriptor cmd_files[] = {
    {
        .name = "protocol",
        .description = "print current protocol",
        .help = NULL,
        .exec = printProtocol,
        .get_data = nullptr,
        .set_data = nullptr,
        .process = nullptr,
    },
    {
        .name = "noteon1",
        .description = "send MT=2 NoteOn Message",
        .help = NULL,
        .exec = noteon1,
        .get_data = nullptr,
        .set_data = nullptr,
        .process = nullptr,
    },
    {
        .name = "noteoff1",
        .description = "send MT=2 NoteOff Message",
        .help = NULL,
        .exec = noteoff1,
        .get_data = nullptr,
        .set_data = nullptr,
        .process = nullptr,
    },
    {
        .name = "noteon2",
        .description = "send MT=4 NoteOn Message",
        .help = NULL,
        .exec = noteon2,
        .get_data = nullptr,
        .set_data = nullptr,
        .process = nullptr,
    },
    {
        .name = "noteoff2",
        .description = "send MT=4 NoteOff Message",
        .help = NULL,
        .exec = noteoff2,
        .get_data = nullptr,
        .set_data = nullptr,
        .process = nullptr,
    },
};

static void initializeMicroShell() {
    ush_init(&ush, &ush_desc);
    ush_node_mount(&ush, "/", &root, NULL, 0);
    ush_commands_add(&ush, &cmd, cmd_files,
                     sizeof(cmd_files) / sizeof(cmd_files[0]));
}

#endif  // USH_H