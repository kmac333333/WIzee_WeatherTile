/*
 Name:		cli.cpp
 Created:	4/7/2024 10:36:43 AM
 Author:	kmac3

 v 1.0 ✔️ ❌ ✅ ➕

*/


/***************************************************************//**
*  include files
*******************************************************************/
#define _TASK_TIMECRITICAL
#include "FS.h"
#include <LITTLEFS.h>
#include    "model2.h"

#if defined(ESP32)
#include    <rom/rtc.h>    // Low-Level ESP32 resources
#endif
//

#include <TaskSchedulerDeclarations.h>
/***************************************************************//**
*  imports
*******************************************************************/
extern Task CommandLineCheck;
extern Task ViewUpdateTask;
extern Task WiFiStatusCheck;
extern Task TouchSampleTask;
extern Task MQTTStatusCheck;
extern Task NTPUpdateTask;
extern Task TempUpdateTask;
extern Task HeartBeat;
extern Task PublishUpdateTask;
extern Scheduler ts;
extern unsigned long perf[];

#define     ARG                     // rfu
#define		DCL_HANDLER(name)       static void name(ARG)
#define     DEF_HANDLER(name,arg)   static void do_##name##_command(arg)

typedef   void (*PFUN_PM)(ARG);
typedef struct {
    char*       command;
    char*       help_text;
    PFUN_PM     foo;
}COMMAND_TABLE_ENTRY, * PCOMMAND_TABLE_ENTRY;
/***************************************************************//**
*  forward references
*******************************************************************/
static void parseCommandBuffer(void);
static bool accumulateLineBuffer(void);
static void reportData(void);
static void do_Report(int print_format);
static void reportData_format_1(void);
static COMMAND_TABLE_ENTRY* searchCommandTable(COMMAND_TABLE_ENTRY* table, char* command, int n);
//
DCL_HANDLER(do_dump_model_command);
DCL_HANDLER(do_o_command);
DCL_HANDLER(do_version_command);
DCL_HANDLER(do_echo_command);
DCL_HANDLER(do_perf_command);
DCL_HANDLER(do_format_command);
DCL_HANDLER(do_dump_report_command);
DCL_HANDLER(do_help_command);
DCL_HANDLER(do_list_dir_command);
DCL_HANDLER(do_save_model_command);
DCL_HANDLER(do_delete_file_command);
DCL_HANDLER(do_load_model_command);
DCL_HANDLER(do_switch_command);
DCL_HANDLER(do_set_client_command);
DCL_HANDLER(do_reset_command);
DCL_HANDLER(do_make_default_command);
DCL_HANDLER(do_set_mqtt_ip_command);
DCL_HANDLER(do_set_ssid_command);
DCL_HANDLER(do_set_pass_command);
DCL_HANDLER(do_set_hostname_command);
DCL_HANDLER(do_stop_command);
DCL_HANDLER(do_temp_command);
DCL_HANDLER(do_tempn_command);
//
// DCL_HANDLER(do_save_command);
/***************************************************************//**
*  local data
*******************************************************************/
/* CLI */
#define COMMAND_BUFFER_SIZE 48
static char commandBuffer[COMMAND_BUFFER_SIZE];

/* COMMAND TABLE */
static COMMAND_TABLE_ENTRY CommandTable[] = {
    {"dm",      "dm - dump model",      do_dump_model_command},
    {"o",       "task stats",           do_o_command},
    {"e",       "echo",                 do_echo_command},
    {"p",       "perf metrics dump",    do_perf_command},
    {"f",       "format 0,1,2,3,4,5",   do_format_command},
    {"dr",      " [n] - do report [n]", do_dump_report_command},
    {"def",     "make default model",   do_make_default_command},
    {"im",       "re-init model",       do_make_default_command},
    {"h",       "help",                 do_help_command},
    {"ls",      "list files",           do_list_dir_command},
    {"r",       "reset",                do_reset_command},
    {"sm",       "save model",          do_save_model_command},
    {"rm",       "delete file",         do_delete_file_command},
    {"lm",       "load model",          do_load_model_command},
    {"sw",      "switch sensor",        do_switch_command}, 
    {"sc",      "set client",           do_set_client_command},

    {"smq",      "set mqtt broker ip",  do_set_mqtt_ip_command},
    {"ss",       "set ssid",            do_set_ssid_command},
    {"sp",       "set pass",            do_set_pass_command},

    {"sh",       "set hostname",        do_set_hostname_command}, 
    {"stop",     "stop timeout",        do_stop_command},
    {"t",        "temp [n]",               do_temp_command},
    {"tn",       "number of temp devices", do_tempn_command},
    {"v",       "version",              do_version_command},
  //  { NULL,NULL,NULL }
};
/* args */
char* args[3] = { "","","" };

/***************************************************************//**
* @brief 9.02    void submit(char* s, int n)
* @brief submit a string for cli processing
* @param char *s - mqtt payload
* @param int n - length of payload
*******************************************************************/
void submit(char* s, int n)
{
    static bool ledState = false;

    for (int i = 0; i < n; i++) {
       
        commandBuffer[i] = *s++;
        digitalWrite(LED_GRN, ledState);
        ledState = !ledState;
    }
    //Serial.print(commandBuffer);
    parseCommandBuffer();
    memset(commandBuffer, 0x00, sizeof(commandBuffer));
}

/***************************************************************//**
* @brief 9.01    void do_commandLine()
*******************************************************************/
void do_commandLine(void)
{
    static bool b = 0;
    b = accumulateLineBuffer();
    if (b == true)
    {
        parseCommandBuffer();
        memset(commandBuffer, 0x00, sizeof(commandBuffer));
    }
}
/***************************************************************//**
* @brief 9.1 void parseCommandBuffer()
* -un-glamorous cli
*******************************************************************/
static void parseCommandBuffer(void)
{
    COMMAND_TABLE_ENTRY* p;

    p = searchCommandTable(CommandTable, commandBuffer, ARRAY_COUNT(CommandTable));
    if (p != NULL)
    {
        (*p->foo)();          // note: watch this when ARG is changed
        return;
    }
    Serial.printf("?\r\n");
}

static COMMAND_TABLE_ENTRY* searchCommandTable(COMMAND_TABLE_ENTRY* table, char* command, int n)
{
    int i = 0;

    //dump_hex(command, 8);
    args[0] = strtok(command, " ");
    args[1] = strtok(NULL, " ");
    args[2] = strtok(NULL, " ");

    //dump_hex(args[0], 8);
    //dump_hex(args[1], 8);
    //dump_hex(args[2], 8);

    for (i = 0; i < ARRAY_COUNT(CommandTable); i++)
    {
        if (strcmp(table[i].command, command) == 0) 
            return &table[i];
    }
    return NULL;
}

// accumulate cli input
static bool accumulateLineBuffer(void)
{
    static int i = 0;
    static char* s = commandBuffer;
    int n;
    uint8_t c;
    static bool ledState = false;

    n = Serial.available();
    if (n == 0) return false;
    //
    c = Serial.read();
    if (c == 0xff || c == 0x00)
    {
        //i = 0;
        return false;
    }
    //
    digitalWrite(LED_GRN, ledState);
    ledState = !ledState;
    if (c == '\n' || c == '\r')
    {
        if (i == 0) return false;        // catch single \n or \r
        //dump_hex(commandBuffer, sizeof(commandBuffer));
        *s = 0x00;
        s = commandBuffer;
        i = 0;
        digitalWrite(LED_GRN, OFF);
        //dump_hex(commandBuffer, sizeof(commandBuffer));
        return true;
    }

    if (i < sizeof(commandBuffer))
    {
        *s++ = c;
        i++;
        //Serial.printf("%c",c);
    }
    return false;
}
/***************************************************************//**
* @brief 8.0.1 static void do_help_command(PMODEL pm)
*******************************************************************/
//static void do_help_command(PMODEL pm)
DEF_HANDLER(help, ARG)
{
    int i;
    PCOMMAND_TABLE_ENTRY p;

    p = &CommandTable[0];
    for (i = 0; i < ARRAY_COUNT(CommandTable); i++)
    {
        Serial.printf("h,%s-%s\r\n", p->command, p->help_text);
        p++;
    }
    Serial.println();
}
/***************************************************************//**
* @brief 8.0.5 static void do_dump_report_command(PMODEL pm)
*******************************************************************/
//static void do_dump_report_command(PMODEL pm)
DEF_HANDLER(dump_report, ARG)
{
    int a;
    int b;

#if 0
    a = atoi(&commandBuffer[2]);
    if (commandBuffer[2] != 0x00) {
        b = atol(&commandBuffer[2]);
        do_Report(b);
    }
#endif
    if (args[1] != NULL) {
        a = atoi(args[1]);
        if (a != 0x00) {
            do_Report(a);
        }
    }
}
/***************************************************************//**
* @brief 8.0.6 static void do_format_command(PMODEL pm)
*******************************************************************/
//static void do_format_command(PMODEL pm)
DEF_HANDLER(format, ARG)
{
}
/***************************************************************//**
* @brief 8.0.7 static void do_perf_command(PMODEL pm)
*******************************************************************/
//static void do_perf_command(PMODEL pm)
DEF_HANDLER(perf, ARG)
{

}
/***************************************************************//**
* @brief  8.0.8 static void do_echo_command(PMODEL pm)
*******************************************************************/
//static void do_echo_command(PMODEL pm)
DEF_HANDLER(echo, ARG)
{
    Serial.printf("e, %s\r\n", args[1]);
}
/***************************************************************//**
* @brief 8.0.9 static void do_version_command(PMODEL pm)
*******************************************************************/
//static void do_version_command(PMODEL pm)
DEF_HANDLER(version, ARG)
{
    Serial.printf("v, %s,  %s, %s\r\n", _FUNCTION, BRD_VERSION, FM_VERSION);
}
/***************************************************************//**
* @brief 8.1.0 static void do_o_command(PMODEL pm)
*******************************************************************/
//static void do_o_command(PMODEL pm)
DEF_HANDLER(o, ARG)
{
    const char* format_o = "o, % 18s:% 8d:% 8d:% 8d\r\n";
    
    Serial.println("=========== Task     Start Delay  Overrun    Run Count");
    //Serial.printf(format_o, "LedBlink", LedBlink.getStartDelay(), LedBlink.getOverrun(), LedBlink.getRunCounter());
    Serial.printf(format_o, "CommandLineCheck", CommandLineCheck.getStartDelay(), CommandLineCheck.getOverrun(), CommandLineCheck.getRunCounter());
    //Serial.printf(format_o, "ViewUpdateTask", ViewUpdateTask.getStartDelay(), ViewUpdateTask.getOverrun(), ViewUpdateTask.getRunCounter());
    //Serial.printf(format_o, "WiFiStatusCheck", WiFiStatusCheck.getStartDelay(), WiFiStatusCheck.getOverrun(), WiFiStatusCheck.getRunCounter());
    //Serial.printf(format_o, "TouchSampleTask", TouchSampleTask.getStartDelay(), TouchSampleTask.getOverrun(), TouchSampleTask.getRunCounter());
    //Serial.printf(format_o, "MQTTStatusCheck", MQTTStatusCheck.getStartDelay(), MQTTStatusCheck.getOverrun(), MQTTStatusCheck.getRunCounter());
    //Serial.printf(format_o, "NTPUpdateTask", NTPUpdateTask.getStartDelay(), NTPUpdateTask.getOverrun(), NTPUpdateTask.getRunCounter());
    //Serial.printf(format_o, "PublishUpdateTask", PublishUpdateTask.getStartDelay(), PublishUpdateTask.getOverrun(), PublishUpdateTask.getRunCounter());
    //Serial.printf(format_o, "TempUpdateTask", TempUpdateTask.getStartDelay(), TempUpdateTask.getOverrun(), TempUpdateTask.getRunCounter());
    //Serial.printf(format_o, "HeartBeat", HeartBeat.getStartDelay(), HeartBeat.getOverrun(), HeartBeat.getRunCounter());
    //
    // ts
    unsigned long cpuTot = ts.getCpuLoadTotal();
    unsigned long cpuCyc = ts.getCpuLoadCycle();
    unsigned long cpuIdl = ts.getCpuLoadIdle();
    float idle = (float)cpuIdl / (float)cpuTot * 100;
    float prod = (float)(cpuIdl + cpuCyc) / (float)cpuTot * 100;
    float ovhd = (float)cpuCyc / (float)cpuTot * 100;
    Serial.println("=========== scheduler ===================");
    Serial.print("Scheduling Overhead CPU time="); Serial.print(cpuCyc); Serial.println(" micros");
    Serial.print("Idle Sleep CPU time="); Serial.print(cpuIdl); Serial.println(" micros");
    Serial.print("Productive work CPU time="); Serial.print(cpuTot - cpuIdl - cpuCyc); Serial.println(" micros");
    Serial.println();
    Serial.print("CPU Idle Sleep "); Serial.print(idle); Serial.println(" % of time.");
    Serial.print("Productive work (not idle, not scheduling)  "); Serial.print(100.00 - prod); Serial.println(" % of time.");
    Serial.print("Overhead  "); Serial.print(ovhd); Serial.println(" % of time.");

    ts.cpuLoadReset();
    
}
/***************************************************************//**
* @brief 8.1.3 void do_switch_command(PMODEL pm)
*******************************************************************/
DEF_HANDLER(switch, ARG)
{
}
/***************************************************************//**
* @brief 8.1.2 void do_dump_model(PMODEL pm)
*******************************************************************/
//static void do_dump_model(PMODEL pm)
DEF_HANDLER(dump_model, ARG)
{
    PMODEL pm = &model;
   // internal_dump_model(pm);
}


/***************************************************************//**
*  @brief 8.1.1   void reportData(void)
*******************************************************************/
static void do_Report(int print_format)
{
}
/***************************************************************//**
*  Format 1
*******************************************************************/
static void reportData_format_1(void)
{
}
/***************************************************************//**
* @brief 8.1.2 static void do_list_dir_command(PMODEL pm)
*******************************************************************/
//static void do_help_command(PMODEL pm)
DEF_HANDLER(list_dir, ARG)
{
 //   listDir(LITTLEFS, "/", 3);
}

/***************************************************************//**
* @brief  8.1.3 static void do_save_model_command(PMODEL pm)
*******************************************************************/
DEF_HANDLER(save_model, ARG)
{
    PMODEL pm = &model;

    char name[32];

    if (args[1] == NULL) return;
    Serial.printf("sm, sm %s\n", args[1]);
    strcpy(name, "/");
    strcat(name, args[1]);

 //   writeFile2(LITTLEFS, name, (const uint8_t *) & model, sizeof(model));
}
/***************************************************************//**
* @brief 8.1.4 static void do_delete_file_command(PMODEL pm)
*  rm "fname"
*******************************************************************/
DEF_HANDLER(delete_file, ARG)
{
    char buf[32];

    if (args[1] == NULL) return;
    Serial.printf("rm, rm %s\n", args[1]);
    strcpy(buf, "/");
    strcat(buf, args[1]);
 //   deleteFile(LITTLEFS, buf);
}
/***************************************************************//**
* @brief 8.1.5 static void do_load_model_command(PMODEL pm)
* lm "fname"
*******************************************************************/
DEF_HANDLER(load_model, ARG)
{
    
    char name[32];
    MODEL m;

    if (args[1] == NULL) return;
    Serial.printf("lm, lm %s\n", args[1]);
    strcpy(name, "/");
    strcat(name, args[1]);
 //   readFile2(LITTLEFS, name, ( uint8_t*)&m, sizeof(model));

 //   internal_dump_model( &m);
    memcpy(&model, &m, sizeof(model));
 //   internal_dump_model(&model);
}
/***************************************************************//**
* @brief 8.1.6 static void do_set_client_command(PMODEL pm)
*******************************************************************/
DEF_HANDLER(set_client, ARG)
{
    PMODEL pm = &model;

    if (args[1] != NULL) {
        strcpy(pm->clientId, args[1]);
        Serial.printf("sc, sc %s\n", pm->clientId);
    }
}
/***************************************************************//**
* @brief 8.1.7 static void do_reset_command(PMODEL pm)
*******************************************************************/
DEF_HANDLER(reset, ARG)
{
    PMODEL pm = &model;

    pm->reset_request = true;
}
/***************************************************************//**
* @brief 8.1.8 static void do_make_default_command((PMODEL pm))
*******************************************************************/
DEF_HANDLER(make_default, ARG)
{
    extern void saveDefaultMode();

 //   saveDefaultMode();
}
/***************************************************************//**
* @brief 8.2.0 static do_set_mqtt_ip_command()
* @param (smq) - set mqtt ip address
*******************************************************************/
DEF_HANDLER(set_mqtt_ip, ARG)
{
    PMODEL pm = &model;

    if (args[1] != NULL) {
        pm->mqtt_server_address.fromString(args[1]);
    }
    Serial.printf("smq, smq %s\n", pm->mqtt_server_address.toString().c_str());
}
/***************************************************************//**
* @brief 8.2.1 static do_set_ssid_command()
* @param (ss) - set ssid
*******************************************************************/
DEF_HANDLER(set_ssid, ARG)
{
    PMODEL pm = &model;
    
    if (args[1] != NULL) {
        strncpy(pm->STAssid, args[1], sizeof( pm->STAssid));
    }
    Serial.printf("ss, ss %s\n", pm->STAssid);
}

/***************************************************************//**
* @brief 8.2.2 static do_set_pass_command()
* @param (sp) - set password
*******************************************************************/
DEF_HANDLER(set_pass, ARG)
{
    PMODEL pm = &model;

    if (args[1] != NULL) {
        strncpy(pm->STApassword, args[1], sizeof(pm->STApassword));
    }
    Serial.printf("sp, sp %s\n", pm->STApassword);
}
/***************************************************************//**
* @brief 8.2.3 static do_set_hostname_command()
* @param (sh) - set hostname
*******************************************************************/
DEF_HANDLER(set_hostname, ARG)
{
    PMODEL pm = &model;

    if (args[1] != NULL) {
        strncpy(pm->hostname, args[1], sizeof(pm->hostname));
    }
    Serial.printf("sh, sh %s\n", pm->hostname);
}
/***************************************************************//**
* @brief 8.2.4 static do_stop_command()
* @param (stop) - stop timeout
*******************************************************************/
DEF_HANDLER(stop, ARG)
{
    PMODEL pm = &model;

    if (pm->halt_timeout == false)
    {
        pm->halt_timeout = true;
    }
    else
    {
        pm->halt_timeout = false;
    }
    Serial.printf("stop, stop %d\n", pm->halt_timeout);
}
/***************************************************************//**
* @brief 8.2.5 static do_temp_command()
* @param temp read - temp n read
* todo: abstract to n devices
*******************************************************************/
DEF_HANDLER(temp, ARG)
{
 
}
/***************************************************************//**
* @brief 8.2.6 static do_temp_command()
* @param temp n - number of temp devices
*******************************************************************/
DEF_HANDLER(tempn, ARG)
{
    PMODEL pm = &model;

    Serial.printf("tn, %d\n", 0);
}