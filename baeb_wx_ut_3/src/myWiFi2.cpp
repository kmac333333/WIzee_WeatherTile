/*
 Name:		myWiFi2.cpp
 Created:	5/2/2025
 Author:	kmac3

 v 1.0 ✔️ ❌ ✅ ➕


 myWiFi2.cpp version handles STA mode and AP mode configured from
 MODEL file op_mode state.

 RUN mode supports wifi STA mode intended to connect to MQTT service
 CONFIG mode supports wifi AP mode that host a configuration web page
 */


/*******************************************************************
*  include files
*******************************************************************/
#define _TASK_MICRO_RES
#define _TASK_TIMECRITICAL
#if defined(ESP32)
#include <WiFi.h>
#include <ESPmDNS.h>
#else
#include <ESP8266WiFi.h>            // not tested on 8266
#endif
#include	"model2.h"


#include <TaskSchedulerDeclarations.h>
/*******************************************************************
*  imports
*******************************************************************/
extern Task  WiFiStatusCheck;
/*******************************************************************
*  forward references
*******************************************************************/
// wifi
void OnWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t type);
void addAID(PMODEL pm, uint8_t aid, uint8_t* mac);
void removeAID(PMODEL pm, uint8_t aid, uint8_t* mac);
void setupWifi(void);
//
static void setup_wifi_0(void);
static void setup_wifi_1(void);
static void setup_wifi_2(void);
static void setup_wifi_3(void);

static void setupWifiAccessPoint(void);
static void setupWifiStation(void);
/*******************************************************************
*  local data
*******************************************************************/

//static char* pwd;
//static char* ssid;

/*******************************************************************
* 7.1  void setup_wifi()
*******************************************************************/
void setupWifi(void)
{
    PMODEL pm = (PMODEL)&model;
    wl_status_t status;
     
    //Serial.printf("iWIFI_00:%lu\n", millis());
    // Set Hostname.
    snprintf(pm->chip_id, sizeof(pm->chip_id), "%012llX", ESP.getEfuseMac());
    strcpy(pm->hostname, HOSTNAME);
    strcat(pm->hostname, BRD_VERSION);
    strcat(pm->hostname, "-");
    strcat(pm->hostname, pm->chip_id);
    Serial.println(pm->hostname);
    WiFi.setHostname(HOSTNAME);

    if (pm->op_mode == RUN)
    {
        setupWifiStation();

        status = WiFi.begin(pm->STAssid, pm->STApassword);
        Serial.printf(" (%d)\n", status);
        WiFiStatusCheck.setCallback(&setup_wifi_0);
    }
#if 0
    else
    {
        WiFiStatusCheck.setCallback(&setup_wifi_0);
        setupWifiAccessPoint();
       // startServer();
    }
#endif
    WiFiStatusCheck.enableDelayed(MS(10));
    pm->data_ready_wifi = true;
    //Serial.printf("oWIFI_00:%lu\n", millis());
}
//
//
static void setup_wifi_0(void)
{
    PMODEL pm = (PMODEL)&model;
  
    //Serial.printf("iWIFI_0:%lu\n", millis());
    // radio is not connected at 0
    // start it up and wait for connect
    if (pm->op_mode == RUN)
        WiFiStatusCheck.setCallback(&setup_wifi_1);
    else
        WiFiStatusCheck.setCallback(&setup_wifi_2);
    // initiate a WiFi network for post-config operation
    // -or-
    // initiate AP mode for config operation

    pm->WIFIonline = false;
    pm->wait_count++;
    pm->data_ready_wifi = true;
    //Serial.printf("oWIFI_0:%lu\n", millis());
}
//
//
static void setup_wifi_1(void)
{
    PMODEL pm = (PMODEL)&model;

    //Serial.printf("iWIFI_1:%lu\n", millis());
    // if connected move on
    if (WiFi.status() == WL_CONNECTED) {
        WiFiStatusCheck.setCallback(&setup_wifi_2);
        //Serial.printf("ooWIFI_1:%lu\n", millis());
        return;
    }
    Serial.print(".");
  
    pm->wait_count++;
    pm->data_ready_wifi = true;
    //pm->doDiag = true;
    if (pm->doDiag == true) {
        if (pm->wait_count % 5 == 0) {
            WiFi.printDiag(Serial);
        }
    }
    //Serial.printf("oWIFI_1:%lu\n", millis());
}
//
//
static void setup_wifi_2(void)
{
    PMODEL pm = (PMODEL)&model;

    //Serial.printf("iWIFI_2:%lu\n", millis());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("Node IP address: ");
    if (pm->op_mode == RUN)
        pm->node_ip_address = WiFi.localIP();
    else
        pm->node_ip_address = WiFi.softAPIP();

    Serial.println(pm->node_ip_address.toString().c_str());
    pm->WIFIonline = true;
    pm->data_ready_wifi = true;
  
    WiFiStatusCheck.setCallback(&setup_wifi_3);
    //Serial.printf("oWIFI_2:%lu\n", millis());
}
//
//
static void setup_wifi_3(void)
{
    PMODEL pm = (PMODEL)&model;

    //Serial.printf("iWIFI_3:%lu\n", millis());
    if (pm->op_mode == RUN)
    {
        if (WiFi.status() == WL_CONNECTED) {
            WiFiStatusCheck.setInterval(S(1));
            //Serial.printf("ooWIFI_3:%lu\n", millis());
            return;
        }
        WiFiStatusCheck.setCallback(&setupWifi);
    }
#if 0
    if (pm->op_mode == CONFIG)
    {
        updateServer();
        delay(15);
    }
#endif
    //Serial.printf("oWIFI_3:%lu\n", millis());
}

/*************************************
// 0.0.5 station mode init
*************************************/
static void setupWifiStation(void)
{
    PMODEL pm = &model;

 
    WiFi.onEvent(OnWiFiEvent);
    WiFi.mode(WIFI_STA);
    Serial.println();
    Serial.print("WiFi connecting to ");
    Serial.print(pm->STAssid);
    Serial.print(" ");
    Serial.println(pm->STApassword);
}
/*************************************
// 0.0.6 access point init
*************************************/
static void setupWifiAccessPoint(void)
{
    PMODEL pm = (PMODEL)&model;
    
    //ssid = pm->APssid;
   // pwd  = pm->APpassword;
    WiFi.onEvent(OnWiFiEvent);
    WiFi.mode(WIFI_AP);
    WiFi.softAP(pm->APssid);
    pm->node_ip_address = WiFi.softAPIP();
    strcpy(pm->clientId, "CONFIG");
    pm->data_ready_wifi = true;
}

/*******************************************************************
*  7.0 void OnWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t type)
*******************************************************************/
void OnWiFiEvent(WiFiEvent_t event, WiFiEventInfo_t type)
{
    PMODEL pm = (PMODEL)&model;
    uint8_t* mac;           /**< MAC address of the station connected to ESP32 soft-AP */
    uint8_t aid;              /**< the aid that ESP32 soft-AP gives to the station connected to  */
    ip4_addr_t* ip;
    int i;

    pm->AP_data_ready_wifi = true;

    Serial.print("ASYNC*** ");
    switch (event) {

        case ARDUINO_EVENT_WIFI_STA_START:
            Serial.println("ESP32 STA started");
            break;
        case ARDUINO_EVENT_WIFI_READY:
            Serial.println("ESP32 Wifi Ready");
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            Serial.println("ESP32 Connected to WiFi Network");
            break;
        case ARDUINO_EVENT_WIFI_AP_START:
            Serial.println("ESP32 soft AP started");
            pm->APonline = true;
            break;
        case ARDUINO_EVENT_WIFI_AP_STOP:
            Serial.println("ESP32 soft AP stopped");
            pm->APonline = false;
            break;
        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            mac = type.wifi_ap_staconnected.mac;
            aid = type.wifi_ap_staconnected.aid;
            if (event == ARDUINO_EVENT_WIFI_AP_STACONNECTED) {
                Serial.println("Station connected to ESP32 soft AP");
                addAID(pm, aid, mac);
            }
            if (event == ARDUINO_EVENT_WIFI_AP_STADISCONNECTED) {
                Serial.println("Station disconnected from ESP32 soft AP");
                removeAID(pm, aid, mac);
            }
            Serial.printf("mac=%02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            Serial.printf("aid=%02X\n", aid);
            break;
        case ARDUINO_EVENT_WIFI_AP_STAIPASSIGNED:
            ip = (ip4_addr_t*)&type.wifi_ap_staipassigned.ip;
            Serial.print("\nAPIP: ");
            Serial.println(ip4addr_ntoa(ip));
            for (i = 0; i < ARRAY_COUNT(pm->ips); i++)
            {
                if (pm->ips[i].active &&
                    strlen(pm->ips[i].ip) == 0
                    ) {
                    strcpy(pm->ips[i].ip, ip4addr_ntoa(ip));
                }
            }
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("STA IP address: ");
            Serial.println(WiFi.localIP());
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("STA Disconnected event");
            break;
        default:
            Serial.printf("Un handled event=%d\n", event);
            break;
    }
}
void addAID(PMODEL pm, uint8_t aid, uint8_t* mac)
{
    if (aid < 0 || aid >  ARRAY_COUNT(pm->aids)) {
        Serial.printf("addAID:oops=%d\n", aid);
        return;
    }
    pm->ips[aid].active = true;
    pm->aids[aid].aid = aid;
    Serial.printf("addAID:aid=%d mac=%02X:%02X:%02X:%02X:%02X:%02X\n", aid, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    memcpy(pm->aids[aid].mac, mac, sizeof(pm->aids[aid].mac));
}
void removeAID(PMODEL pm, uint8_t aid, uint8_t* mac)
{
    if (aid < 0 || aid >  ARRAY_COUNT(pm->aids)) {
        Serial.printf("removeAID:oops=%d\n", aid);
        return;
    }
    pm->ips[aid].active = false;
    pm->aids[aid].aid = 0;
    memset(pm->aids[aid].mac, 0x00, sizeof(pm->aids));
    Serial.printf("removeAID:aid=%d mac=%02X:%02X:%02X:%02X:%02X:%02X\n", aid, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    mac = pm->aids[aid].mac;
    Serial.printf("removeAID:aid=%d mac=%02X:%02X:%02X:%02X:%02X:%02X\n", aid, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}