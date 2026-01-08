/*
 @file Name:		TEMP_GRN_ESP32_TTGO_V4.ino
 @date Created:	12/12/2024 6:06:56 PM
 @Author Author:	kmac3
 @Author Author:	grok4

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
lv_obj_t *status_label;     // For on-screen status
lv_obj_t *refresh_overlay;  // Refreshing spinner overlay
static lv_obj_t *spinner;
static lv_obj_t *refresh_msg;
bool simulating_refresh = false;

// Placeholder static data (will be replaced by real fetch later)
const char *current_temp = "72°";
const char *condition = "Sunny";
const char *high_low = "H:78° L:55°";
const char *humidity = "Hum: 45%";
const char *wind = "Wind: 8 mph";

static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[800 * 480 / 10];
//static lv_color_t disp_draw_buf;
static lv_disp_drv_t disp_drv;
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
  setup_ui();
  lv_timer_handler();

  Serial.println( "Setup done" );
}
/*******************************************************************
*  @brief  void setup_ui(void)
*******************************************************************/
void setup_ui(void)
{
  create_weather_tile();
  create_refresh_overlay();  // Create overlay once
}

void create_weather_tile() {
    lv_obj_clean(lv_scr_act());

    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

    // Huge temperature
    lv_obj_t *temp = lv_label_create(lv_scr_act());
    lv_label_set_text(temp, current_temp);
    lv_obj_set_style_text_font(temp, &lv_font_montserrat_48, 0);  // Much bigger!
    lv_obj_set_style_text_color(temp, lv_color_white(), 0);
    lv_obj_align(temp, LV_ALIGN_CENTER, 0, -60);

    // Condition
    lv_obj_t *cond = lv_label_create(lv_scr_act());
    lv_label_set_text(cond, condition);
    lv_obj_set_style_text_font(cond, &lv_font_montserrat_48, 0);
    lv_obj_align_to(cond, temp, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    // Details row
    lv_obj_t *details = lv_label_create(lv_scr_act());
    lv_label_set_text_fmt(details, "%s   |   %s   |   %s", high_low, humidity, wind);
    lv_obj_set_style_text_font(details, &lv_font_montserrat_32, 0);
    lv_obj_align_to(details, cond, LV_ALIGN_OUT_BOTTOM_MID, 0, 40);

    // Status bar (top-right)
    status_label = lv_label_create(lv_scr_act());
    lv_label_set_text(status_label, "Tap to refresh");
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_24, 0);
    lv_obj_align(status_label, LV_ALIGN_TOP_RIGHT, -20, 20);

    // Large icon placeholder (will be real image later)
    lv_obj_t *icon = lv_label_create(lv_scr_act());
    lv_label_set_text(icon, LV_SYMBOL_WARNING);  // temporary
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, 0);
    lv_obj_set_style_text_color(icon, lv_color_hex(0xFFD700), 0);
    lv_obj_align(icon, LV_ALIGN_CENTER, 0, -160);

    // Add tap handler to the whole screen
    lv_obj_add_event_cb(lv_scr_act(), trigger_refresh, LV_EVENT_PRESSED, NULL);
}
// Create the overlay and its children once (in setup)
void create_refresh_overlay() {
    refresh_overlay = lv_obj_create(lv_scr_act());
    lv_obj_set_size(refresh_overlay, 800, 480);
    lv_obj_set_style_bg_color(refresh_overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(refresh_overlay, LV_OPA_60, 0);  // semi-transparent
    lv_obj_add_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);     // hidden by default
    lv_obj_set_style_radius(refresh_overlay, 0, 0);

    spinner = lv_spinner_create(refresh_overlay, 1000, 90);   // v8 signature
    lv_obj_set_size(spinner, 120, 120);
    lv_obj_center(spinner);

    refresh_msg = lv_label_create(refresh_overlay);
    lv_label_set_text(refresh_msg, "Refreshing...");
    lv_obj_set_style_text_font(refresh_msg, &lv_font_montserrat_40, 0);
    lv_obj_set_style_text_color(refresh_msg, lv_color_white(), 0);
    lv_obj_align_to(refresh_msg, spinner, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
}
// Simulate refresh (tap handler)
void trigger_refresh(lv_event_t *e) {
    if (simulating_refresh) return;
    simulating_refresh = true;

    // Show overlay
    lv_obj_clear_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);

    // Backlight pulse
    tft.setBrightness(100);
    delay(200);
    tft.setBrightness(255);

    // Simulate network delay
    delay(1500);

    // Hide overlay
    lv_obj_add_flag(refresh_overlay, LV_OBJ_FLAG_HIDDEN);

    // Update status
    lv_label_set_text(status_label, "Updated just now");

    simulating_refresh = false;
}
/*******************************************************************
*  @brief  loop()
*******************************************************************/
void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay( 10 );
}

