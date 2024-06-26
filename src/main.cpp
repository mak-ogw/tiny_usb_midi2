// clang-format off
#include <FreeRTOS.h>
#include <task.h>

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "tusb_ump/ump_device.h"

#include "midi2.h"
#include "ush.h"
#include "rtos.h"
// clang-format on

int main(void) {
    board_init();
    stdio_init_all();
    tusb_init();

    initializeMicroShell();
    initializeMidi2();
    initializeRtos();

    while (true) {
        ;
    }
    return 0;
}
