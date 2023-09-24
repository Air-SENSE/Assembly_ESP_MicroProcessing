/**
 * @file asm_main.c
 * @author SPARC Lab - Nguyen Nhu Hai Long (@long27032002)
 * @brief Main file to run assembly code in asm.S file
 * @version 0.1
 * @date 2023-09-24
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc_periph.h"

#if CONFIG_IDF_TARGET_ESP32
#include "esp32/ulp.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/ulp.h"
#include "esp32s2/ulp_riscv.h"
#elif CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/ulp.h"
#include "esp32s3/ulp_riscv.h"
#endif

#include "asm_main.h"


/**
 * @warning: PLEASE DO NOT MAKE ANY MODIFICATIONS TO THIS FILE UNLESS YOU UNDERSTAND IT CLEARLY!
 */


#if CONFIG_IDF_TARGET_ESP32
static const char *TAG = "esp32_assembly";
#elif CONFIG_IDF_TARGET_ESP32S2
static const char *TAG = "esp32s2_assembly";
#elif CONFIG_IDF_TARGET_ESP32S3
static const char *TAG = "esp32s3_assembly";
#endif

// Define the start and end addresses of the binary file
extern const uint8_t bin_start[] asm("_binary_asm_main_bin_start");
extern const uint8_t bin_end[] asm("_binary_asm_main_bin_end");

#define ASM_ENTRY ulp_entry
#define ULP_WAKEUP_MAX_PERIOD 3200000U

/**
 * @brief Entry point of the assembly code.
 *
 * This function is called to start the execution of the assembly code.
 * It does not return.
 */
__attribute__((noreturn)) extern void entry(void);


/**
 * @brief Initializes the assembly program.
 * 
 * This function loads the binary code into the ULP (Ultra Low Power) co-processor
 * and sets the wakeup period. Then it starts the program by running it.
 */
static void init_assembly_program(void)
{
    // Load the binary code into the ULP co-processor
    esp_err_t error_return;

#ifdef CONFIG_IDF_TARGET_ESP32
    error_return = ulp_load_binary(0, bin_start, (bin_end - bin_start) / sizeof(uint32_t));
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    error_return = ulp_riscv_load_binary(bin_start, (bin_end - bin_start));
#endif
    ESP_ERROR_CHECK_WITHOUT_ABORT(error_return);

    // Set the wakeup period
    ulp_set_wakeup_period(0, ULP_WAKEUP_MAX_PERIOD);

    // Start the program by running it
#ifdef CONFIG_IDF_TARGET_ESP32
    error_return = ulp_run(&ASM_ENTRY - RTC_SLOW_MEM);
#elif CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
    error_return = ulp_riscv_run();
#endif
    ESP_ERROR_CHECK_WITHOUT_ABORT(error_return);
}

void app_main(void)
{
    // Print chip information
    // Get information about the ESP chip
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    // Print chip information
    ESP_LOGI(TAG,  "This is %s chip with %d CPU core(s), WiFi%s%s, silicon revision %d, %dMB %s flash.\n",
                    CONFIG_IDF_TARGET,
                    chip_info.cores,
                    (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
                    (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
                    chip_info.revision,
                    spi_flash_get_chip_size() / (1024 * 1024),
                    (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    // Print minimum free heap size
    ESP_LOGI(TAG, "Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());

    // Initialize the assembly program
    init_assembly_program();

    ESP_LOGI(TAG, "Entering deep sleep\n\n");

    /**
     * Disable all wakeup sources for sleep.
     */
    ESP_ERROR_CHECK( esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL) );
    
    /**
     * Start deep sleep.
     */
    esp_deep_sleep_start();

}