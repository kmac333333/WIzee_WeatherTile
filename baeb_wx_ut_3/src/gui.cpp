/*
 Name:		gui.cpp
 Created:	
 Author:	kmac3

 v 1.0 ✔️ ❌ ✅ ➕
 ✔️ lovyangfx 1.1.12
 ✔️ lvgl v8.3.6

 ➕ uses lvgl as display engine.  
*/
/*******************************************************************
*  include files
*******************************************************************/
#include  "gfx_conf.h"
#include	"gui.h"
#include  "WeatherData.h"
#include   <lvgl.h>
/*******************************************************************
*  imports
*******************************************************************/
#if 0 //rfu
typedef struct
{
	void *			_obj;
	int32_t			pos_x;
	int32_t			pos_y;
	int32_t			size_x;		//width
	int32_t			size_y;		//height
	void (* _cb)(void)		;		//cb for event
	const char* _text;		//text for object
} WIDGET, * PWIDGET;
#endif
LV_FONT_DECLARE(lv_font_montserrat_120);
/*******************************************************************
*  local data
*******************************************************************/
// Table-driven configuration - all main widgets
struct WidgetConfig {
     char *text_ptr;          // Pointer to dynamic text (nullptr for static/icon)
    const lv_font_t *font;
    lv_color_t color;
    lv_align_t align;
    int16_t x_ofs;
    int16_t y_ofs;
    lv_obj_t *obj_ptr;             // Where to store the created object (for chaining/icon)
};
static char temp_buf[16]    = "0°";
static char cond_buf[32]    = "not sure";
static char details_buf[64] = "H:78° L:55° | Hum: 45% | Wind: 8 mph";
static char status_buf[32]  = "Tap to refresh";

// gfx objects
static lv_obj_t *icon_obj = nullptr;  // Special case
static lv_obj_t *status_label;
static lv_obj_t *spinner;
static lv_obj_t *refresh_msg;
 
static lv_obj_t *refresh_overlay;
static bool simulating_refresh = false;
static uint32_t last_event_time = 0;
const uint32_t DEBOUNCE_MS = 300;

static  WidgetConfig tile_widgets[] = {
    { &temp_buf[0],    &lv_font_montserrat_120, lv_color_white(), LV_ALIGN_CENTER,     0, -100, nullptr },  // Temp - base
    { cond_buf,    &lv_font_montserrat_48,  lv_color_white(), LV_ALIGN_OUT_BOTTOM_MID, 0,  10, nullptr },  // Condition - under temp
    { details_buf, &lv_font_montserrat_32,  lv_color_white(), LV_ALIGN_OUT_BOTTOM_MID, 0,  20, nullptr },  // Details - under condition
    { status_buf,  &lv_font_montserrat_24,  lv_color_white(), LV_ALIGN_TOP_RIGHT,   -20, 20, nullptr },  // Status - top-right
    { nullptr,      &lv_font_montserrat_48,  lv_color_hex(0xFFD700), LV_ALIGN_CENTER, 0, -220, icon_obj },  // Icon - centered independently
};
/*******************************************************************
*  forward references
*******************************************************************/
static void initOLED(PMODEL pm);
static void my_disp_flush(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p);
static void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data);
static void ui_init() ;
static void setup_ui(void);
static void create_weather_tile(void);
static void create_refresh_overlay();
static void trigger_refresh(lv_event_t *e) ;
/*******************************************************************
*  0.0 setup()
*******************************************************************/
void* ui_setup(PMODEL pm)
{
	initOLED(pm);
	//createWidgets(&widgets);  //rfu
	return NULL;
}

/*******************************************************************
*   0.0.1 initOLED()
*******************************************************************/
static void initOLED(PMODEL pm)
{
    int32_t x;
    int32_t y;
    int32_t i, j;
    float r;
    int16_t oled_h, oled_w;

    lv_init();
    pm->tft->init();
    pm->tft->setRotation(0);
    pm->tft->fillScreen(TFT_NAVY);
    //
    // boilerplate lvgl 
    //
    // display buffer
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t disp_buf[800 * 480 / 10];
    lv_disp_draw_buf_init(&draw_buf, disp_buf, NULL, 800 * 480 / 10);
    // display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = 800;
    disp_drv.ver_res = 480;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);
    // touchpad driver
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = my_touchpad_read;
    lv_indev_drv_register(&indev_drv);

    /*************************************
    // 0.0.8 init ui
    *************************************/
    Serial.println("0.2) ui init - Start");
    ui_init();
    setup_ui();
    Serial.println("0.2) ui init - Done");
}
/*******************************************************************
*  @brief  void setup_ui(void)
*******************************************************************/
static void setup_ui(void)
{
    create_weather_tile();
    create_refresh_overlay();  // Create overlay once

        // Tap handler on screen and overlay
    lv_obj_add_event_cb(lv_scr_act(), trigger_refresh, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(refresh_overlay, trigger_refresh, LV_EVENT_CLICKED, NULL);
}
/*******************************************************************
*  
*******************************************************************/
static void create_weather_tile() 
{
    lv_obj_clean(lv_scr_act());  // Clear existing UI
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x003a57), LV_PART_MAIN);

    lv_obj_t *prev = nullptr;

    // Create and configure each widget
    for (size_t i = 0; i < sizeof(tile_widgets) / sizeof(tile_widgets[0]); i++) {
         auto &cfg = tile_widgets[i];

        lv_obj_t *obj = lv_label_create(lv_scr_act());

        if (cfg.text_ptr) {
            lv_label_set_text(obj, cfg.text_ptr);  // Dynamic text
        } else {
            lv_label_set_text(obj, LV_SYMBOL_WARNING);  // Placeholder for icon
        }

        lv_obj_set_style_text_font(obj, cfg.font, 0);
        lv_obj_set_style_text_color(obj, cfg.color, 0);

        // Alignment: Chain to previous if applicable
        if (prev && cfg.align != LV_ALIGN_CENTER) {
            lv_obj_align_to(obj, prev, cfg.align, cfg.x_ofs, cfg.y_ofs);
        } else {
            lv_obj_align(obj, cfg.align, cfg.x_ofs, cfg.y_ofs);
        }

        cfg.obj_ptr = obj;
        prev = obj;
    }

    // Optional: Future real icon handling (replace LV_SYMBOL_WARNING)
    // if (icon_obj && current_weather.icon_code) {
    //     // Map icon_code to LVGL symbol or image
    // }
}
// Create the overlay and its children once (in setup)
static void create_refresh_overlay() 
{
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

static void trigger_refresh(lv_event_t *e) 
{
    PMODEL pm = &model;

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
    pm->tft->setBrightness(100);
    delay(200);  // Short delay OK — doesn't block animation
    pm->tft->setBrightness(255);
}

void update_ui_from_weather(void) 
{
    if (current_weather.valid) {
        snprintf(temp_buf, sizeof(temp_buf), "%.0f°", current_weather.temp_f);
        snprintf(cond_buf, sizeof(cond_buf), "%s", current_weather.condition);
        snprintf(details_buf, sizeof(details_buf), "H:%.0f° L:%.0f° | Hum:%d%% | Wind:%.0f mph",
                 current_weather.temp_max, current_weather.temp_min,
                 current_weather.humidity, current_weather.wind_speed);
        snprintf(status_buf, sizeof(status_buf), "Updated just now");
    } else {
        snprintf(temp_buf, sizeof(temp_buf), "N/A");
        snprintf(cond_buf, sizeof(cond_buf), "Offline");
        snprintf(details_buf, sizeof(details_buf), "Last update failed");
        snprintf(status_buf, sizeof(status_buf), "Tap to retry");
    }

    //if (tile_widgets[0].obj_ptr!= nullptr) lv_label_set_text(tile_widgets[0].obj_ptr, temp_buf);
    //if (tile_widgets[1].obj_ptr!= nullptr) lv_label_set_text(tile_widgets[1].obj_ptr, cond_buf);
    // Force label updates (safe even if LVGL cached)
    if (tile_widgets[0].obj_ptr != nullptr) lv_label_set_text(tile_widgets[0].obj_ptr, temp_buf);
    if (tile_widgets[1].obj_ptr != nullptr) lv_label_set_text(tile_widgets[1].obj_ptr, cond_buf);
    if (tile_widgets[2].obj_ptr != nullptr) lv_label_set_text(tile_widgets[2].obj_ptr, details_buf);
    if (tile_widgets[3].obj_ptr != nullptr) lv_label_set_text(tile_widgets[3].obj_ptr, status_buf);
    lv_obj_invalidate(lv_scr_act());  // Redraw labels with new text
}
// rfu for squarline ui.c init
static void ui_init() 
{

}

/*******************************************************************
*  adjunct lgl display-centric stuff
*******************************************************************/
/* Display flushing */
static void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    PMODEL pm = &model;

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    #if (LV_COLOR_16_SWAP != 0)
    tft.pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);
    #else
    pm->tft->pushImageDMA(area->x1, area->y1, w, h,(lgfx::rgb565_t*)&color_p->full);//
    #endif
    lv_disp_flush_ready(disp);
}

/*******************************************************************
*  adjunct lovyangfx touch pad-centric stuff
*******************************************************************/
void my_touchpad_read(lv_indev_drv_t *indev_driver, lv_indev_data_t *data)
{
    PMODEL pm = &model;

    uint16_t touchX, touchY;
    bool touched = pm->tft->getTouch( &touchX, &touchY);
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