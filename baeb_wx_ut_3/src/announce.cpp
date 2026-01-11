
/*******************************************************************
*  include files
*******************************************************************/
#include <rom/rtc.h>    // Low-Level ESP32 resources

#include	"model2.h"

/*******************************************************************
*  forward references
*******************************************************************/
void report_flash(void);
void wakeup_reason(void);
void print_reset_reason(RESET_REASON reason);
/*******************************************************************
*   0.0.2 announce()
*******************************************************************/
void announce()
{
    // flash resources
    report_flash();
    // esp32 boot reason
    wakeup_reason();
    //
    extern String projectFile;
    extern String projectDate;

    PMODEL pm = &model;

    //String LVGL_Arduino = "LVGL = ";
    //LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
    //Serial.println(LVGL_Arduino);
    Serial.print(F("USER BOARD NAME     = ")); Serial.println(_HOST_BOARD);
    Serial.print(F("IDE BOARD NAME      = ")); Serial.println(ARDUINO_BOARD);
    Serial.print(F("USER BOARD FUNCTION = ")); Serial.println(_FUNCTION);
    Serial.print(F("USER BOARD VERSION  = ")); Serial.println(BRD_VERSION);
    Serial.print(F("USER FM VERSION     = ")); Serial.println(FM_VERSION);
    Serial.print(F("USER FILE           = ")); Serial.println(projectFile);
    Serial.print(F("USER FILE DATE      = ")); Serial.println(projectDate);
    Serial.print(F("CPU Frequency       = ")); Serial.print(F_CPU / MEG(1)); Serial.println(F(" MHz"));
    Serial.print(F("LED_BLU_PIN         = ")); Serial.println(LED_BLU);
     //Serial.print(F("TOUCH PIN           = ")); Serial.println(TOUCH_CS);
    Serial.print(F("OLED w   = "));            Serial.println(pm->tft->width());
    Serial.print(F("OLED h   = "));            Serial.println(pm->tft->height());
}

/*******************************************************************
   0.0.2.1 void report_flash(void)
*******************************************************************/
void report_flash(void)
{
    //uint32_t realSize = ESP.getFlashChipRealSize();
    uint32_t ideSize = ESP.getFlashChipSize();
    FlashMode_t ideMode = ESP.getFlashChipMode();

    //Serial.printf("Flash real id:   %08X\r\n", ESP.getFlashChipId());
    //Serial.printf("Flash real size: %u bytes\r\n", realSize);

    Serial.printf("Flash ide  size: %u bytes\r\n", ideSize);
    Serial.printf("Flash ide speed: %u Hz\r\n", ESP.getFlashChipSpeed());
    Serial.printf("Flash ide mode:  %s\r\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
}
/*******************************************************************
   0.0.2.2 void wakeup_reason(void)
*******************************************************************/
void wakeup_reason(void) 
{
    esp_sleep_wakeup_cause_t wakeup_reason;
    wakeup_reason = esp_sleep_get_wakeup_cause();

    switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
        Serial.println("\nWakeup by ext signal RTC_IO -> GPIO39");
        break;

    case ESP_SLEEP_WAKEUP_EXT1:
        Serial.println("Wakeup by ext signal RTC_CNTL -> GPIO34");
        break;

    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup by ULP program"); break;
    default: Serial.printf("Wakeup not caused by deep sleep: %d\n", wakeup_reason); break;
    }
    Serial.print("CPU0 reset reason: ");
    print_reset_reason(rtc_get_reset_reason(0));
    Serial.println();
    Serial.print("CPU1 reset reason: ");
    print_reset_reason(rtc_get_reset_reason(1));
    Serial.println();
}

void print_reset_reason(RESET_REASON reason)
{
    switch (reason) {
    case 1: Serial.print("POWERON_RESET"); break;
    case 3: Serial.print("SW_RESET"); break;
    case 4: Serial.print("OWDT_RESET"); break;
    case 5: Serial.print("DEEPSLEEP_RESET"); break;
    case 6: Serial.print("SDIO_RESET"); break;
    case 7: Serial.print("TG0WDT_SYS_RESET"); break;
    case 8: Serial.print("TG1WDT_SYS_RESET"); break;
    case 9: Serial.print("RTCWDT_SYS_RESET"); break;
    case 10: Serial.print("INTRUSION_RESET"); break;
    case 11: Serial.print("TGWDT_CPU_RESET"); break;
    case 12: Serial.print("SW_CPU_RESET"); break;
    case 13: Serial.print("RTCWDT_CPU_RESET"); break;
    case 14: Serial.print("EXT_CPU_RESET"); break;
    case 15: Serial.print("RTCWDT_BROWN_OUT_RESET"); break;
    case 16: Serial.print("RTCWDT_RTC_RESET"); break;
    default: Serial.print("UNKNOWN");
    }
}