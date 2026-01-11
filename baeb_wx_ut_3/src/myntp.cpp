/*******************************************************************
*  include files
*******************************************************************/
#include <ESP32Time.h>

#define _TASK_TIMECRITICAL
#include <TaskSchedulerDeclarations.h>

#include	"model2.h"
/*******************************************************************
*  imports
*******************************************************************/
extern Task NTPUpdateTask;
/*******************************************************************
*  local data
*******************************************************************/
ESP32Time rtc(0);
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -28800;            
const int   daylightOffset_sec = 3600;
/*******************************************************************
*  forward references
*******************************************************************/
void do_NTPUpdate(PMODEL pm);
void NTPUpdate_0(void);
void NTPUpdate_1(void);
void setTime(void);
/*******************************************************************
*  0.0 void do_NTPUpdate(PMODEL pm)
*******************************************************************/
void do_NTPUpdate(PMODEL pm)
{
	//Serial.printf("iNTP_0:%d\n", millis());
	if (pm->WIFIonline == true) 
	{
		// set rtc
		setTime();
		
		pm->hour   = rtc.getHour();
		pm->minute = rtc.getMinute();
		pm->second = rtc.getSecond();
		pm->day    = rtc.getDay();
		pm->month  = rtc.getMonth();
		pm->year   = rtc.getYear();
		Serial.printf("NTP: %d:%d:%d %d-%d-%d\n",
			pm->hour,
			pm->minute,
			pm->second,
			pm->month+1,
			pm->day,
			pm->year
		);
		NTPUpdateTask.setCallback(&NTPUpdate_0);
		pm->data_ready_time = true;
	}
	//Serial.printf("oNTP_0:%d\n", millis());
}
void NTPUpdate_0(void)
{
	PMODEL pm = &model;
#if 0
	NTPUpdateTask.setCallback(&NTPUpdate_1);
	NTPUpdateTask.enableDelayed(S(24 * 60 * 60));		// update again from ntp.org in 24 hrs
#endif

	//Serial.printf("iNTP_1:%d\n", millis());
	pm->hour	= rtc.getHour();
	pm->minute	= rtc.getMinute();
	pm->second	= rtc.getSecond();
	pm->day		= rtc.getDay();
	pm->month	= rtc.getMonth();
	pm->year	= rtc.getYear();
	pm->t		= rtc.getTimeDate();
	pm->data_ready_time = true;
	//
	// get clock again if we go offline
	if (pm->WIFIonline == false)
	{
		NTPUpdateTask.setCallback((TaskCallback)&do_NTPUpdate);
	}

	//Serial.printf("oNTP_1:%d\n", millis());
}

// might be good enough or now - no need for this state
void NTPUpdate_1(void)
{
	NTPUpdateTask.setCallback((TaskCallback) & do_NTPUpdate);
	NTPUpdateTask.enableDelayed(MS(2500));
}


void setTime(void)
{
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
	struct tm timeinfo;
	if (getLocalTime(&timeinfo)) {
		rtc.setTimeStruct(timeinfo);
	}
}

/*******************************************************************
*  0.1 void initTime(PMODEL pm)
*******************************************************************/
void initTime(PMODEL pm)
{
    unsigned long in;

    in = millis();
    Serial.printf("initTime in: %lu\n", in);
    //
    setenv("TZ", "PST8PDT,M3.2.0,M11.1.0", 1);
    tzset();  // Apply timezone
    time_t dummy = time(NULL);  // Force libc init
    localtime(&dummy);
    //
    #if 0
	pm->hour = rtc.getHour();
	pm->minute = rtc.getMinute();
	pm->second = rtc.getSecond();
	pm->day = rtc.getDay();
	pm->month = rtc.getMonth();
	pm->year = rtc.getYear();
	pm->t = rtc.getTimeDate();
	pm->data_ready_time = true;
    #endif
    Serial.printf("initTime out: %lu\n", millis() - in);
}