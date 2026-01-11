/*******************************************************************
*  include files
*******************************************************************/
#include    "model2.h"
//#include    "weather_credentials.h"
//#include    "gfx_conf.h"
/*******************************************************************
*  imports
*******************************************************************/
extern LGFX tft;
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
#define MODEL_FILENAME  "/MODEL"
/*******************************************************************
*  local data
*******************************************************************/
const char* default_STAssid = WIFI_SSID ;
const char* default_STApassword = WIFI_PASSWORD ;
static const char* default_APssid = "config";
static const char* default_APpassword = "";
/*******************************************************************
*  forward references
*******************************************************************/
static void internal_MkNull_MODEL(PMODEL pm);
static void mergeTopicParts(PMODEL pm, int device, char * partA, char * partB, char * partC, char * partD);



void initModel(PMODEL pm)
{
    char name[32];
    static MODEL m;
    int b;

    strcpy(name, MODEL_FILENAME);
    pm->tft = &tft;
        // RUN path
    if ((strcmp(m._xx_, "_xx_") != 0) || (sizeof(MODEL) != m.size))
    {
        internal_MkNull_MODEL(pm);
        //writeFile2(LITTLEFS, name, (const uint8_t*)pm, sizeof(model));
        Serial.printf("Default file created\n");
    }
    else {
        Serial.printf("Valid\n");
    }
    pm->op_mode = RUN;
}
/***************************************************************//**
*  @brief void internal_MkNull_MODEL(PMODEL pm)
*  @verbatim - fills out a MODEL struct with default values
*******************************************************************/
static void internal_MkNull_MODEL(PMODEL pm)
{
    // model state
    memset(&model, 0x0, sizeof(model));
// todo: fixme  find unitialized data being used somewhere
//
    pm->now = micros();
    pm->last_now = pm->now;
    strcpy(pm->version, FM_VERSION);
    pm->size = sizeof(MODEL);
    pm->print_format = 0;
    pm->reset_request = false;
    pm->op_mode = RUN;
    // file tag
    strcpy(pm->_xx_, "_xx_");
    pm->size = sizeof(MODEL);
    // storage
    pm->dirty = true;
    pm->bInitialized = true;
    // display
    pm->tft = &tft;
    pm->width = pm->tft->width();
    pm->height = pm->tft->height();
    // - topics per device
    // A
    // - four configurable parts for prefix
    mergeTopicParts(pm, 0, "home", "temp", "unit", "A");
    // topic published gets node IP appended
    memset(pm->publish_topic_A, '\0', sizeof(pm->publish_topic_A));
    // B
    // - four configurable parts for prefix
    mergeTopicParts(pm, 1, "home", "temp", "unit", "B");
    // topic published gets node IP appended
    memset(pm->publish_topic_B, '\0', sizeof(pm->publish_topic_B));
    //
    pm->mqtt_server_connected = false;
    pm->data_ready_mqtt = true;
    // system
    memset(pm->chip_id, '0', sizeof(pm->chip_id));
    snprintf(pm->chip_id, sizeof(pm->chip_id), "%012llX", ESP.getEfuseMac());
    pm->freeRam = 0;
    pm->data_ready_system = true;
    pm->halt_timeout = true;
    // WiFi
    memset(pm->clientId, '\0', sizeof(pm->clientId));
    strcpy(pm->clientId, "temp unit ");
    strcat(pm->clientId, pm->chip_id);
    pm->doDiag = true;
    //
    memset(pm->hostname, '\0', sizeof(pm->hostname));
    pm->node_ip_address = IPAddress(127, 0, 0, 1);
    strcpy(pm->STAssid, default_STAssid);
    strcpy(pm->STApassword, default_STApassword);
    strcpy(pm->APssid, default_APssid);
    strcpy(pm->APpassword, default_APpassword);
    pm->data_ready_heart_beat = true;
    pm->data_ready_wifi = true;
    pm->AP_data_ready_wifi = true;
    pm->data_ready_mqtt = true;
    pm->data_ready_system = true;
}
/***************************************************************//**
*  @brief vvoid mergeTopicParts()
*  @verbatim - allow for 4 configurable parts to merge into a topic
*******************************************************************/
static void mergeTopicParts(PMODEL pm, int device, char * partA, char * partB, char * partC, char * partD)
{
    int n;
    char* d;

    // select device
    switch (device)
    {
    case 0:  { n = 0; d = pm->publish_topic_prefix_A; break; }
    case 1:  { n = 1; d = pm->publish_topic_prefix_B; break; }
    default: { n = 0; d = pm->publish_topic_prefix_A; break; }
    }
    // - four configurable parts for prefix
    strcpy(pm->devices[n].topicPrefixPart[0], partA);
    strcpy(pm->devices[n].topicPrefixPart[1], partB);
    strcpy(pm->devices[n].topicPrefixPart[2], partC);
    strcpy(pm->devices[n].topicPrefixPart[3], partD);
    memset(d, '\0', sizeof(pm->publish_topic_prefix_A));
    strcat(d, "/"); strcat(d, pm->devices[n].topicPrefixPart[0]);
    strcat(d, "/"); strcat(d, pm->devices[n].topicPrefixPart[1]);
    strcat(d, "/"); strcat(d, pm->devices[n].topicPrefixPart[2]);
    strcat(d, "/"); strcat(d, pm->devices[n].topicPrefixPart[3]);
    strcat(d, "/");
}