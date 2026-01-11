
/*******************************************************************
*  // cli.h
*******************************************************************/
#define _TASK_TIMECRITICAL
#include    <TaskSchedulerDeclarations.h>
#ifndef _CLI_h
#define _CLI_h

#ifdef __cplusplus  //  +  c functions
extern "C" {        //  |  in cpp context
#endif              //  |
	// c interfaces //  |
#ifdef __cplusplus  //  |
}                   //  |
#endif              //  +
// c++ interfaces
extern void do_commandLine(void);
extern Task CommandLineCheck;

#endif

