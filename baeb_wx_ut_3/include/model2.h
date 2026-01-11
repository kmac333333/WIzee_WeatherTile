/*******************************************************************
*  // model.h
*******************************************************************/


#ifndef _MODEL_h
#define _MODEL_h


#if defined(ESP32)
#include    <WiFi.h>
#endif
#if defined(ESP8266)
#include    <ESP8266WiFi.h>
#endif
#include    "gfx_conf.h"
//
// application data model
// data model

#if defined(ESP32)
#ifndef LED_BUILTIN
#define LED_BUILTIN 2
#endif
#define LED_BLU LED_BUILTIN
#define LED_GRN 27
#define LED_RED 26     
#define BUTTON1PIN 17
#endif

#if defined(ESP8266)
#define LED_BLU D0          // nodemcu wroom esp32s
#endif
#define OFF HIGH
#define ON  LOW

//
#define FM_VERSION "v3.00"
#define BRD_VERSION "v1.1"
#define _FUNCTION "ESP32 WX, Elecrow Wizee "
#define _HOST_BOARD "Wizee-ESP32 "
#define HOSTNAME ("esp32-wizee-cyd-wx ")

//
#define ARRAY_COUNT(array_) (sizeof(array_)/sizeof((array_)[0]))
#define MEG(n)  (n*1000000UL)
#ifdef _TASK_MICRO_RES
#define MS(n) (n * 1000UL)
#define S(n)  (n * MS(1000UL))
#else
#define MS(n) (n * 1UL)
#define S(n)  (n * MS(1000UL))
#define M(n)  (n * S(60))
#endif

typedef enum {
    CONFIG,
    RUN
}OP_MODE;
typedef struct {
    uint8_t mac[6];    /**< MAC address of the station connected to ESP32 soft-AP */
    uint8_t aid;
}AID, * PAID;
//
typedef struct {
    char ip[32];    
    bool active;
}IPELEMENT, * PIPELEMENT;
typedef struct _TS {
    char    topicPrefixPart[4][8];  // 4 parts each 8 bytes for mqtt topic header
    bool    valid;
}DEVICE, * PDEVICE;

// MODEL
typedef struct __attribute__((packed, aligned(4)))  {
    // model state
    char        version[6];
    int         size;
    char        _xx_[5];        // file tag
    uint32_t    now;
    uint32_t    last_now;
    bool        reset_request;
    int        op_mode;
    /* PRINT FORMAT */
    int         print_format;
    // storage
    bool        dirty;
    bool        bInitialized;
    // heartbeat
    bool        bHeartBeat;
    bool        data_ready_heart_beat;
    // display
    uint32_t    width;
    uint32_t    height;
    LGFX        *tft;
    // touch
    uint16_t    t_x;
    uint16_t    t_y;
    uint8_t     pressed;
    // WiFi
    IPAddress   node_ip_address;
    char        clientId[64];
    char        hostname[128];
    bool        data_ready_wifi;
    bool        AP_data_ready_wifi;
    int         wait_count;
    bool        APonline;
    bool        WIFIonline;
    AID         aids[8];
    IPELEMENT   ips[8];
    bool        doDiag;
    char        STAssid[32];            // STA ssid
    char        STApassword[32];        // STA password
    char        APssid[32];             // AP ssid
    char        APpassword[32];         // STA password
    /* MQTT */
    IPAddress   mqtt_server_address;
    char*       mqtt_server_hostname;
    bool        use_ip_address;
    long        lastMsg = 0;
    unsigned long publish_interval;
    char        publish_topic_prefix_A[50];
    char        publish_topic_A[80];
    char        publish_topic_prefix_B[50];
    char        publish_topic_B[80];
    bool        mqtt_server_connected;
    bool        data_ready_mqtt;
    DEVICE      devices[2];
    // system
    char            chip_id[32];
    unsigned long   freeRam;
    bool            data_ready_system;
    bool            halt_timeout;
    // time
    int     hour;
    int     minute;
    int     second;
    int     day;
    int     month;
    int     year;
    String  t;
    bool    data_ready_time;
}MODEL, * PMODEL;



extern MODEL model;

extern void initModel(PMODEL pm);
extern void internal_dump_model(void* m);
#endif

