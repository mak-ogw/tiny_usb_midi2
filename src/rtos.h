#ifndef RTOS_H_
#define RTOS_H_

static void umpProcessing(void *p) {
    while (true) {
        taskYIELD();
        if (tud_ump_n_mounted(0)) {
            uint32_t ump_n_available = tud_ump_n_available(0);
            uint32_t UMPpacket[4];
            uint32_t umpCount;
            if (ump_n_available) {
                uint8_t mVersion = tud_alt_setting(0) + 1;
                if ((umpCount = tud_ump_read(0, UMPpacket, 4))) {
                    switch (umpCount) {
                        case 1:
                            printf("MIDI %d UMP 0x%08x \n", mVersion,
                                   UMPpacket[0]);
                            break;
                        case 2:
                            printf("MIDI %d UMP 0x%08x 0x%08x \n", mVersion,
                                   UMPpacket[0], UMPpacket[1]);
                            break;
                        case 3:
                            printf("MIDI %d UMP 0x%08x 0x%08x 0x%08x\n",
                                   mVersion, UMPpacket[0], UMPpacket[1],
                                   UMPpacket[2]);
                            break;
                        case 4:
                            printf("MIDI %d UMP 0x%08x 0x%08x 0x%08x 0x%08x\n",
                                   mVersion, UMPpacket[0], UMPpacket[1],
                                   UMPpacket[2], UMPpacket[3]);
                            break;
                    }

                    for (uint32_t i = 0; i < umpCount; i++) {
                        UMPHandler.processUMP(UMPpacket[i]);
                    }
                }
            }
        }
    }
}

// static void printMidiFlagTask(void *p) {
//     static uint8_t umpf = 0xFF;
//     while (true) {
//         const auto current = tud_alt_setting(0);
//         if (current != umpf) {
//             umpf = current;
//             if (umpf == 0) {
//                 printf("usb-midi1.0.\n");
//             }
//             if (umpf == 1) {
//                 printf("usb-midi2.0.\n");
//             }
//         }
//         vTaskDelay(pdMS_TO_TICKS(10));
//     }
// }

static void runTudTask(void *p) {
    while (true) {
        taskYIELD();
        // vTaskDelay(pdMS_TO_TICKS(10));
        tud_task();
    }
}

static void microShellTask(void *p) {
    while (true) {
        taskYIELD();
        // vTaskDelay(pdMS_TO_TICKS(10));
        ush_service(&ush);
    }
}

static void ledBlinkTask(void *p) {
    static bool led_state = false;
    while (true) {
        // taskYIELD();
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_put(PICO_DEFAULT_LED_PIN, led_state);
        led_state = 1 - led_state;  // toggle
    }
}

static void initializeRtos() {
    xTaskCreate(umpProcessing, /* Task function */
                "ump",         /* The text name assigned to task */
                1024,          /* The size of stack to allocate to the task */
                NULL,          /* The parameter passed to the task */
                1,             /* The priority assigned to the task */
                NULL           /* The task handle, NULL if not required */
    );

    // xTaskCreate(printMidiFlagTask, /* Task function */
    //             "tud",             /* The text name assigned to task */
    //             1024, /* The size of stack to allocate to the task */
    //             NULL, /* The parameter passed to the task */
    //             1,    /* The priority assigned to the task */
    //             NULL  /* The task handle, NULL if not required */
    // );

    xTaskCreate(runTudTask, /* Task function */
                "tud",      /* The text name assigned to task */
                1024,       /* The size of stack to allocate to the task */
                NULL,       /* The parameter passed to the task */
                1,          /* The priority assigned to the task */
                NULL        /* The task handle, NULL if not required */
    );

    xTaskCreate(microShellTask, /* Task function */
                "ush",          /* The text name assigned to task */
                1024,           /* The size of stack to allocate to the task */
                NULL,           /* The parameter passed to the task */
                1,              /* The priority assigned to the task */
                NULL            /* The task handle, NULL if not required */
    );

    xTaskCreate(ledBlinkTask, /* Task function */
                "blink",      /* The text name assigned to task */
                512,          /* The size of stack to allocate to the task */
                NULL,         /* The parameter passed to the task */
                1,            /* The priority assigned to the task */
                NULL          /* The task handle, NULL if not required */
    );

    vTaskStartScheduler();
}

#endif  // RTOS_H_