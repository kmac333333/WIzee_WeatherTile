// util.h

#ifndef _UTIL_h
#define _UTIL_h

#ifdef __cplusplus
extern "C" {
#endif
	// c interfaces
#ifdef __cplusplus
}
#endif
// c++ interfaces
extern unsigned long deltastamp(void);
extern void DTi(int i);
extern unsigned long DTo(int i);
extern float minMax(float value, float min_value, float max_value);
extern int iminMax(int value, int min_value, int max_value);
extern void dump_hex(char* data, size_t size);
#endif

