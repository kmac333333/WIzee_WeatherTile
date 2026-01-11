/*
 @file Name:		main.cpp
 @date Created:	    1/10/26 12:00
 @Author Author:	kmac3
 @Author Author:	grok4
// ➕ https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File/tree/main
// ➕ https://www.elecrow.com/wiki/esp32-display-502727-intelligent-touch-screen-wi-fi26ble-800480-hmi-display.html
// ➕ https://www.elecrow.com/esp32-display-5-inch-hmi-display-rgb-tft-lcd-touch-screen-support-lvgl.html
// ➕ https://www.elecrow.com/wiki/CrowPanel_ESP32_HMI_Wiki_Content.html
// ➕ https://drive.google.com/drive/folders/1L4u6XIXdC1Blz7ftDLN4XtnQlR18M2co?referrer=grok.com
// ➕ https://drive.google.com/drive/folders/1npWkVzctd7e0Y6H5hahk8uxGhy9idxCz?referrer=grok.com
// ➕ https://github.com/Elecrow-RD/CrowPanel-5.0-HMI-ESP32-Display-800x480?referrer=grok.com
// ➕ https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File/tree/main/CrowPanel_ESP32_Tutorial
// ➕ https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File/tree/main/CrowPanel_ESP32_Tutorial/Code/V1.X

 v 3.00 ✔️ ❌ ✅ ➕
 ✔️ TaskScheduler 
 ✔️ lovyangfx
 ✔️ lvgl v8.3.5
 ✔️ wizee-esp32
*/
/*******************************************************************
*  include files
*******************************************************************/
#define _TASK_TIMECRITICAL
#include    "gfx_conf.h"
#include    "model2.h"
#include    "gui.h"
#include    "cli.h"
#include	"myWiFi2.h"
#include    <lvgl.h>
#include    <TaskScheduler.h>
#include    "WeatherData.h"
#include    <esp_task_wdt.h>

/*******************************************************************
*  imports
*******************************************************************/
#define TFT_BL 2
#define IO_TP_J2  38

extern void fetch_weather() ;
/*******************************************************************
*  forward references
*******************************************************************/
static void update_display(void);
static void cb_commandLine(void);
static void cb_viewUpdate(void);
static void cb_fetchWeather(void);
static void cb_wifiStatus(void);
/*******************************************************************
*  local data
*******************************************************************/
String projectFile = F(__FILE__);
String projectDate = F(__DATE__);
// Model
MODEL  model;
/* SCHEDULER */
Scheduler ts;
/* display */
LGFX tft = LGFX();
/* TASKS */
Task CommandLineCheck(TASK_SECOND / 1000, TASK_FOREVER, &cb_commandLine, &ts, false);
Task fetchTask(30000, TASK_FOREVER,                     &cb_fetchWeather, &ts, false);  // Check every 30s (adjust later)
Task displayUpdateTask(TASK_SECOND * 1,   TASK_FOREVER, &update_display, &ts, false);  
Task WiFiStatusCheck(MS(667), TASK_FOREVER,             &cb_wifiStatus, &ts, false);
// Dynamic text buffers (updated by weather fetch)


/***************************************************************//**
* @brief  0.0 setup()
*******************************************************************/
void setup()
{
    PMODEL pm = &model;
    /*************************************
    // 0.0.0  Basic init
    *************************************/
    Serial.begin(921600);
    Serial.flush();
    while (!Serial);
    delay(50);
    Serial.println("\n\r0) Setup - start");
    /*************************************
    // 0.1  init model
    *************************************/
    Serial.println("0.1) init model - start");
    initModel(&model);
    Serial.println("0.1) init model - done");
    /*************************************
    // 0.0.1  Display init
    *************************************/
    Serial.println("1) OLED Init - start");
    void* disp = ui_setup(&model);
    Serial.println("1) OLED Init - done");
    /*************************************
    // go
    *************************************/
    current_weather.valid = false;
    CommandLineCheck.enableDelayed(MS(997));
    fetchTask.enableDelayed(MS(229));
    displayUpdateTask.enableDelayed(MS(229));
    WiFiStatusCheck.enableDelayed(MS(97));
    //
    Serial.println( "Setup done" );
}
/*******************************************************************
*  @brief  loop()
*******************************************************************/
void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    ts.execute();
    yield();
    
    static uint32_t last_check = 0;
    if (millis() - last_check > 10000) {
      Serial.printf("Free heap: %d bytes | Display task active: %d\n",
                    ESP.getFreeHeap(), displayUpdateTask.isEnabled());
      last_check = millis();
    }
}
/*******************************************************************
*  C A L L B A C K S
*******************************************************************/
/*******************************************************************
* @brief 9.0 void cb_commandLine(void)
*******************************************************************/
void cb_commandLine(void)
{
    do_commandLine();
}
/*******************************************************************
*  @brief 7.0 void cb_wifiStatus(void)
*******************************************************************/
void cb_wifiStatus(void)
{
    setupWifi();
}
/*******************************************************************
* @brief 6.0  void cb_viewUpdate(void)
*******************************************************************/
void cb_viewUpdate(void)
{
    update_display();
}
// Update UI from weather data
static void update_display(void)
{
  //Serial.println( "display update" );
  if (current_weather.valid == true)
  {
    update_ui_from_weather();
    current_weather.valid =  false;
    Serial.println( "updated" );
  }
}
/*******************************************************************
* @brief 4.0 void cb_fetchWeather(void)
*******************************************************************/
static void cb_fetchWeather(void)
{
    PMODEL pm = &model;

    if (pm->WIFIonline == false) return;
    fetch_weather();
}
	