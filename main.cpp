#include <FreeRTOS.h>
#include <task.h>

#include "bsp/board.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "tusb_ump/ump_device.h"

void ledBlinkTask(void *p) {
    static bool led_state = false;
    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, led_state);
        led_state = 1 - led_state;  // toggle
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void printMidiFlagTask(void *p) {
    static uint8_t umpf = 0xFF;
    while (true) {
        const auto current = tud_alt_setting(0);
        if (current != umpf) {
            umpf = current;
            if (umpf == 0) {
                printf("usb-midi1.0.\n");
            }
            if (umpf == 1) {
                printf("usb-midi2.0.\n");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void runTudTask(void *p) {
    while (true) {
        tud_task();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int main(void) {
    board_init();
    stdio_init_all();
    tusb_init();

    xTaskCreate(ledBlinkTask, /* Task function */
                "blink",      /* The text name assigned to task */
                512,          /* The size of stack to allocate to the task */
                NULL,         /* The parameter passed to the task */
                1,            /* The priority assigned to the task */
                NULL          /* The task handle, NULL if not required */
    );

    xTaskCreate(printMidiFlagTask, /* Task function */
                "tud",             /* The text name assigned to task */
                1024, /* The size of stack to allocate to the task */
                NULL, /* The parameter passed to the task */
                2,    /* The priority assigned to the task */
                NULL  /* The task handle, NULL if not required */
    );

    xTaskCreate(runTudTask, /* Task function */
                "tud",      /* The text name assigned to task */
                1024,       /* The size of stack to allocate to the task */
                NULL,       /* The parameter passed to the task */
                3,          /* The priority assigned to the task */
                NULL        /* The task handle, NULL if not required */
    );

    vTaskStartScheduler();

    while (true) {
        ;
    }
    return 0;
}
