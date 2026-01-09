/*
 @file Name:		TEMP_GRN_ESP32_TTGO_V4.ino
 @date Created:	12/12/2024 6:06:56 PM
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
*/
/*******************************************************************
*  include files
*******************************************************************/
#include <lvgl.h>
#include  "gfx_conf.h"

LV_FONT_DECLARE(lv_font_montserrat_120);
/*******************************************************************
*  imports
*******************************************************************/
#define TFT_BL 2
#define IO_TP_J2  38
// reserved for TP
// - lovyangfx is using i2c driver for i2c tp gt911
//#define IO19_I2C_SDA_J8 19
//#define IO20_I2C_SCL_J8 20
/*******************************************************************
*  forward references
*******************************************************************/
void setup_ui(void);
void create_weather_tile(void);
void trigger_refresh(lv_event_t *e) ;
void create_refresh_overlay();
/*******************************************************************
*  local data
*******************************************************************/
static lv_obj_t *status_label;
static lv_obj_t *refresh_overlay;
static lv_obj_t *spinner;
static lv_obj_t *refresh_msg;
static bool simulating_refresh = false;
static uint32_t last_event_time = 0;
const uint32_t DEBOUNCE_MS = 300;
// Placeholder static data (will be replaced by real fetch later)
const char *current_temp = "72°";
const char *condition = "Sunny";
const char *high_low = "H:78° L:55°";
const char *humidity = "Hum: 45%";
const char *wind = "Wind: 8 mph";

#if 0
static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[800 * 480 / 10];
//static lv_color_t disp_draw_buf;
static lv_disp_drv_t disp_drv;
#endif
/*******************************************************************
*  adjunct lgl display-centric stuff
*******************************************************************/
/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    #if (LV_COLOR_16_SWAP != 0)
    tft.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);
    #else
    tft.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);//
    #endif
    lv_disp_flush_ready(disp);
}
/*******************************************************************
*  adjunct lovyangfx touch pad-centric stuff
*******************************************************************/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    uint16_t touchX, touchY;
    bool touched = tft.getTouch( &touchX, &touchY);
    if( touched )
    {
      data->state = LV_INDEV_STATE_PR;
        /*Set the coordinates*/
      data->point.x = touchX;
      data->point.y = touchY;
      Serial.print( "Data x " );
      Serial.println( touchX );
      Serial.print( "Data y " );
      Serial.println( touchY );
    } else {
      data->state = LV_INDEV_STATE_REL;
    }
}
/***************************************************************//**
* @brief  0.0 setup()
*******************************************************************/
void setup()
{
    Serial.begin(115200);
    Serial.println("\n=== OSB1: WIzee-ESP32 Weather Tile ===\nLVGL v8.3.6 | LovyanGFX ^1.1.12");

    tft.begin();
    delay(100);
    tft.setRotation(0);          // Landscape – adjust 0-3 if needed
    tft.setBrightness(255);
    tft.fillScreen(TFT_BLACK);

    delay(200);
    lv_init();
    delay(100);
    //
    // boilerplate lvgl 
    //
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t disp_buf[800 * 480 / 10];
    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, 800 * 480 / 10);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 800;
    disp_drv.ver_res = 480;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);
  #if 0
    screenWidth = tft.width();
    screenHeight = tft.height();
    lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 10);
   
    /* Initialize the display */
    lv_disp_drv_init(&disp_drv);
    /* Change the following line to your display resolution */
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
  
    /* Initialize the (dummy) input device driver */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    #ifdef TFT_BL
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    #endif
  #endif
    setup_ui();
    //lv_timer_handler();

    Serial.println( "Setup done" );
}


/*******************************************************************
*  @brief  void setup_ui(void)
*******************************************************************/
void setup_ui(void)
{
    create_weather_tile();
    create_refresh_overlay();  // Create overlay once

        // Tap handler on screen and overlay
    lv_obj_add_event_cb(lv_scr_act(), trigger_refresh, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(refresh_overlay, trigger_refresh, LV_EVENT_CLICKED, NULL);
}

void create_weather_tile() {
    lv_obj_clean(lv_scr_act());
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

    // Huge temperature
    lv_obj_t *temp = lv_label_create(lv_scr_act());
    lv_label_set_text(temp, current_temp);
    lv_obj_set_style_text_font(temp, &lv_font_montserrat_120, 0);
    lv_obj_set_style_text_color(temp, lv_color_white(), 0);
    lv_obj_align(temp, LV_ALIGN_CENTER, 0, -60);

    // Condition
    lv_obj_t *cond = lv_label_create(lv_scr_act());
    lv_label_set_text(cond, condition);
    lv_obj_set_style_text_font(cond, &lv_font_montserrat_48, 0);
    lv_obj_align_to(cond, temp, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // Details row
    lv_obj_t *details = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(details, "%s | %s | %s", high_low, humidity, wind);
    lv_obj_set_style_text_font(details, &lv_font_montserrat_32, 0);
    lv_obj_align_to(details, cond, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);

    // Status (top-right)
    status_label = lv_label_create(lv_scr_act());
    lv_label_set_text(status_label, "Tap to refresh");
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_24, 0);
    lv_obj_align(status_label, LV_ALIGN_TOP_RIGHT, -20, 20);

    // Icon placeholder (sunny symbol for now)
    lv_obj_t *icon = lv_label_create(lv_scr_act());
    lv_label_set_text(icon, LV_SYMBOL_WARNING);  // Temporary - real icon next
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0xFFD700), 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -200);

}
// Create the overlay and its children once (in setup)
void create_refresh_overlay() {
    refresh_overlay = lv_obj_create(lv_scr_act());
    lv_obj_set_size(refresh_overlay, 800, 480);
    lv_obj_set_style_bg_color(refresh_overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(refresh_overlay, LV_OPA_70, 0);
    lv_obj_add_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);

    spinner = lv_spinner_create(refresh_overlay, 1000, 90);
    lv_obj_set_size(spinner, 200, 200);
    lv_obj_center(spinner);
    lv_obj_set_style_arc_color(spinner, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(spinner, 10, LV_PART_INDICATOR);

    refresh_msg = lv_label_create(refresh_overlay);
    lv_label_set_text(refresh_msg, "Refreshing...");
    lv_obj_set_style_text_font(refresh_msg, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(refresh_msg, lv_color_white(), 0);
    lv_obj_align_to(refresh_msg, spinner, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}

void trigger_refresh(lv_event_t *e) {

    static uint32_t refresh_start_time = 0;
    static const uint32_t SIMULATE_DELAY_MS = 1500;
    
    uint32_t now = millis();
    if (now - last_event_time < DEBOUNCE_MS) return;
    last_event_time = now;

    if (simulating_refresh) return;
    simulating_refresh = true;
    refresh_start_time = now;

    // Show overlay immediately
    lv_obj_clear_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);

    // TEMP TEST: Make overlay bright and fully opaque to prove it's drawing
    lv_obj_set_style_bg_color(refresh_overlay, lv_color_hex(0xFF0000), 0);  // Bright red background
    lv_obj_set_style_bg_opa(refresh_overlay, LV_OPA_COVER, 0);               // Fully opaque

    lv_obj_invalidate(refresh_overlay);
    lv_obj_invalidate(spinner);
    lv_obj_invalidate(refresh_msg);
    lv_obj_invalidate(lv_scr_act());

    // Backlight pulse
    tft.setBrightness(100);
    delay(200);  // Short delay OK — doesn't block animation
    tft.setBrightness(255);

    // No long delay here — animation runs in loop()
}
#if 0
void trigger_refresh(lv_event_t *e) {
    uint32_t now = millis();
    if (now - last_event_time < DEBOUNCE_MS) return;
    last_event_time = now;

    if (simulating_refresh) return;
    simulating_refresh = true;

    // Show and force full redraw of overlay + children
    lv_obj_clear_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_invalidate(refresh_overlay);        // Parent
    lv_obj_invalidate(spinner);                // Force spinner arc
    lv_obj_invalidate(refresh_msg);            // Force message
    lv_obj_invalidate(lv_scr_act());           // Safety

    // Optional: touch style to force refresh (harmless)
    lv_obj_set_style_bg_opa(refresh_overlay, LV_OPA_70, 0);

    // Backlight pulse
    tft.setBrightness(100);
    delay(200);
    tft.setBrightness(255);

    // Simulate
    delay(1500);

    // Hide
    lv_obj_add_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_invalidate(refresh_overlay);
    lv_obj_invalidate(lv_scr_act());

    lv_label_set_text(status_label, "Updated just now");

    simulating_refresh = false;
}

void trigger_refresh(lv_event_t *e) {
    uint32_t now = millis();
    if (now - last_event_time < DEBOUNCE_MS) return;
    last_event_time = now;

    if (simulating_refresh) return;
    simulating_refresh = true;

    // Show overlay
    lv_obj_clear_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);

    // Critical: Invalidate the overlay itself (forces children redraw)
    lv_obj_invalidate(refresh_overlay);
    lv_obj_invalidate(lv_scr_act());  // Also whole screen for safety

    // Backlight pulse
    tft.setBrightness(100);
    delay(200);
    tft.setBrightness(255);

    // Simulate fetch delay
    delay(1500);

    // Hide overlay
    lv_obj_add_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_invalidate(refresh_overlay);  // Invalidate again on hide
    lv_obj_invalidate(lv_scr_act());

    // Update status
    lv_label_set_text(status_label, "Updated just now");

    simulating_refresh = false;
}

void toggle_overlay(lv_event_t *e) {
    uint32_t now = millis();
    if (now - last_event_time < DEBOUNCE_MS) return;  // Debounce
    last_event_time = now;

    overlay_visible = !overlay_visible;
    if (overlay_visible) {
        lv_obj_clear_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(lv_obj_get_child(refresh_overlay, 0), "OVERLAY ACTIVE\n\nTap to hide");
    } else {
        lv_obj_add_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);
    }
    lv_obj_invalidate(lv_scr_act());
}
#endif
/*******************************************************************
*  @brief  loop()
*******************************************************************/
void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay( 10 );
}

