/*******************************************************************
*  include files
*******************************************************************/
#define _TASK_TIMECRITICAL

#include    "FS.h"
#include    <LITTLEFS.h>
#include    "model2.h"
#include    "storage.h"

#include    <TaskSchedulerDeclarations.h>
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
    memset(&m, 0x00, sizeof(m));
    readFile2(LITTLEFS, name, (uint8_t*)&m, sizeof(model));
    Serial.printf("Reading MODEL file: %s\n", name);
    
        // RUN path
    if ((strcmp(m._xx_, "_xx_") != 0) || (sizeof(MODEL) != m.size))
    {
        internal_MkNull_MODEL(pm);
        writeFile2(LITTLEFS, name, (const uint8_t*)pm, sizeof(model));
        Serial.printf("Default file created\n");
    }
    else {
        Serial.printf("Valid\n");
        memcpy(pm, &m, sizeof(model));
    }
    pm->op_mode = RUN;
    pm->tft = &tft;
}
/*******************************************************************
*  void saveModel()
* - saves MODEL struct to FS
*******************************************************************/
void saveModel()
{
    PMODEL pm = &model;

    writeFile2(LITTLEFS, MODEL_FILENAME, (const uint8_t*)pm, sizeof(model));
    Serial.printf("Saved\n");
}
/*******************************************************************
*  void saveDefaultMode()
* - creates and saves a default model
*******************************************************************/
void saveDefaultMode()
{
    PMODEL pm = &model;
    int b;

    internal_MkNull_MODEL(pm);
    writeFile2(LITTLEFS, MODEL_FILENAME, (const uint8_t*)pm, sizeof(model));
    Serial.printf("Default file created\n");
    pm->reset_request = true;
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

/***************************************************************//**
*  @brief void internal_dump_model(void* m)
*  @verbatim - dumps memory as if it was a MODEL
*******************************************************************/
void internal_dump_model(void* m)
{
    PMODEL pm = (PMODEL)m;

    Serial.println("dm,---------------------");
    // time
    Serial.printf("time: %d.%d.%d %d-%d-%d\n",
        pm->hour,
        pm->minute,
        pm->second,
        pm->day,
        pm->month,
        pm->year
    );
    // model state
    Serial.print("tag:                 ");  Serial.println(pm->_xx_);
    Serial.print("model size:          ");  Serial.println(pm->size);
    // system
    Serial.print("clientId:            ");  Serial.println(pm->clientId);
    Serial.print("chipId:              ");  Serial.println(pm->chip_id);
    Serial.print("halt timeout:        ");  Serial.println(pm->halt_timeout);
    // print format
    Serial.print("format:              ");  Serial.println(pm->print_format);
#if 0
    // temperature sensor
    Serial.print("celsius 0:           ");  Serial.println(pm->devices[0].celsius);
    Serial.print("fahrenheit 0:        ");  Serial.println(pm->devices[0].fahrenheit);
    Serial.print("tempString1 :        ");  Serial.println(pm->devices[0].tempString);
    Serial.print("celsius 1:           ");  Serial.println(pm->devices[1].celsius);
    Serial.print("fahrenheit 1:        ");  Serial.println(pm->devices[1].fahrenheit);
    Serial.print("tempString 1:        ");  Serial.println(pm->devices[1].tempString);
    Serial.print("data_ready_temp:     ");  Serial.println(pm->data_ready_temp);
    Serial.print("swap:                ");  Serial.println(pm->temp_sw);
#endif
    // heartbeat
    Serial.print("bHeartBeat: ");             Serial.println(pm->bHeartBeat);
    Serial.print("data_ready_heart_beat: ");  Serial.println(pm->data_ready_heart_beat);
    // wifi
    Serial.print("node_ip_address:     ");  Serial.println(pm->node_ip_address.toString().c_str());
    Serial.print("hostname:            ");  Serial.println(pm->hostname);
    Serial.print("data_ready_wifi:     ");  Serial.println(pm->data_ready_wifi);
    Serial.print("wait_count:          ");  Serial.println(pm->wait_count);
    Serial.print("WIFIonline:          ");  Serial.println(pm->WIFIonline);
    Serial.print("diag:                ");  Serial.println(pm->doDiag);
    Serial.print("AP_data_ready_wifi:  ");  Serial.println(pm->AP_data_ready_wifi);
    Serial.print("APonline:            ");  Serial.println(pm->APonline);
    Serial.print("STAssid:             ");  Serial.println(pm->STAssid);
    Serial.print("STApassword:         ");  Serial.println(pm->STApassword);
    Serial.print("APssid:              ");  Serial.println(pm->APssid);
    Serial.print("APpassword:          ");  Serial.println(pm->APpassword);
    // mqtt
    Serial.print("mqtt_server_address   ");  Serial.println(pm->mqtt_server_address.toString().c_str());
    Serial.print("mqtt_server_hostname  ");  Serial.println(pm->mqtt_server_hostname);
    Serial.print("use ip address        ");  Serial.println(pm->use_ip_address);
    Serial.print("mqtt_server_connected ");  Serial.println(pm->mqtt_server_connected);
    Serial.print("publish interval      ");  Serial.println(pm->publish_interval);
    Serial.print("publish topic prefix A");  Serial.println(pm->publish_topic_prefix_A);
    Serial.print("publish topic  A      ");  Serial.println(pm->publish_topic_A);
    Serial.print("publish topic prefix B");  Serial.println(pm->publish_topic_prefix_B);
    Serial.print("publish topic  B      ");  Serial.println(pm->publish_topic_B);
#if 0
    int i;
    ip4_addr_t* ip;
    Serial.println("AP connections:      ");
    for (i = 0; i < ARRAY_COUNT(pm->ips); i++)
    {
        Serial.printf("%d: ", pm->ips[i].active);
        Serial.printf("%d ", pm->aids[i].aid);
        Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X ",
            pm->aids[i].mac[0],
            pm->aids[i].mac[1],
            pm->aids[i].mac[2],
            pm->aids[i].mac[3],
            pm->aids[i].mac[4],
            pm->aids[i].mac[5]
        );
        Serial.printf("%s", pm->ips[i].ip);
        Serial.printf("\n");
    }
#endif
    Serial.println("dm,---------------------");

}