// https://www.elecrow.com/wiki/esp32-display-502727-intelligent-touch-screen-wi-fi26ble-800480-hmi-display.html
// https://www.elecrow.com/esp32-display-5-inch-hmi-display-rgb-tft-lcd-touch-screen-support-lvgl.html
// https://www.elecrow.com/wiki/CrowPanel_ESP32_HMI_Wiki_Content.html
// https://drive.google.com/drive/folders/1L4u6XIXdC1Blz7ftDLN4XtnQlR18M2co?referrer=grok.com
// https://drive.google.com/drive/folders/1npWkVzctd7e0Y6H5hahk8uxGhy9idxCz?referrer=grok.com
// https://github.com/Elecrow-RD/CrowPanel-5.0-HMI-ESP32-Display-800x480?referrer=grok.com
// https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File/tree/main/CrowPanel_ESP32_Tutorial
// https://github.com/Elecrow-RD/CrowPanel-ESP32-Display-Course-File/tree/main/CrowPanel_ESP32_Tutorial/Code/V1.X
#include <lvgl.h>
#include  "gfx_conf.h"
//#include "ui.h"

#define TFT_BL 2
#define IO_TP_J2  38
// reserved for TP
// - lovyangfx is using i2c driver for i2c tp gt911
#define IO19_I2C_SDA_J8 19
#define IO20_I2C_SCL_J8 20

//extern LGFX tft;
int led;

static uint32_t screenWidth;
static uint32_t screenHeight;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t disp_draw_buf[800 * 480 / 10];
//static lv_color_t disp_draw_buf;
static lv_disp_drv_t disp_drv;

/* Display flushing */
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{

  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  

  //lcd.fillScreen(TFT_WHITE);
#if (LV_COLOR_16_SWAP != 0)
  tft.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);
#else
  tft.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);//
#endif

  lv_disp_flush_ready(disp);

}
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

void setup()
{
  //Wire.begin(IO19_I2C_SDA_J8, IO20_I2C_SCL_J8);
  //dht20.begin();
  //IO口引脚
  pinMode(IO_TP_J2, OUTPUT);
  digitalWrite(IO_TP_J2, LOW);

  Serial.begin(115200);
  Serial.println("\n=== OSB1: WIzee-ESP32 Weather Tile ===\nLVGL v8.3.6 | LovyanGFX ^1.1.12");

  tft.begin();
  tft.setRotation(0);          // Landscape – adjust 0-3 if needed
  tft.setBrightness(255);
  tft.fillScreen(TFT_BLACK);
  
  delay(200);
  lv_init();
  delay(100);

  screenWidth = tft.width();
  screenHeight = tft.height();

  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, screenWidth * screenHeight / 10);
  //  lv_disp_draw_buf_init(&draw_buf, disp_draw_buf, NULL, 480 * 272 / 10);
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
  
// Basic UI – dark theme like our dashboard
  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_COVER, LV_PART_MAIN);

  lv_obj_t *label = lv_label_create(lv_scr_act());
  lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
  lv_label_set_text(label,
    "Weather Tile\n"
    "v0.1.0\n\n"
    "WIzee-ESP32 5\"\n"
    "LVGL 8.3.6\n"
    "Touch screen\n"
    "to test");
  lv_obj_set_style_text_font(label, &lv_font_montserrat_48, 0);
  lv_obj_set_style_text_color(label, lv_color_white(), 0);
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  lv_timer_handler();

  Serial.println( "Setup done" );

}

void loop()
{
  if(led == 1)
  digitalWrite(IO_TP_J2, HIGH);
  if(led == 0)
  digitalWrite(IO_TP_J2, LOW);
  lv_timer_handler(); /* let the GUI do its work */
  delay( 10 );
}