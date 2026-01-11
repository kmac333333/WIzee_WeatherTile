/*******************************************************************
*  include files
*******************************************************************/
#include    <Arduino.h>
/*******************************************************************
*  local data
*******************************************************************/
/* PERF */
unsigned long perf[10];


unsigned long deltastamp(void)
{
    static unsigned long last_stamp = micros();
    unsigned long now, delta;

    now = micros();
    delta = now - last_stamp;
    last_stamp = now;
    return delta;
}
void DTi(int i)
{
    perf[i] = micros();
}
unsigned long DTo(int i)
{
    perf[i] = micros() - perf[i];
    return perf[i];
}
/**
 * Make sure that given value is not over min_value/max_value range.
 *
 * @param float value     : The value to convert
 * @param float min_value : The min value
 * @param float max_value : The max value
 *
 * @return float
 */
float minMax(float value, float min_value, float max_value) 
{
    if (value > max_value) {
        value = max_value;
    }
    else if (value < min_value) {
        value = min_value;
    }

    return value;
}
int iminMax(int value, int min_value, int max_value) 
{
    if (value > max_value) {
        value = max_value;
    }
    else if (value < min_value) {
        value = min_value;
    }

    return value;
}
/*******************************************************************
* 1.9.9 static void dump_hex(const unsigned char* data, size_t size)
* dump hex from grok
*******************************************************************/
void dump_hex(char* data, size_t size) 
{
    size_t i, j;

    if (data == NULL) return;

    for (i = 0; i < size; i += 16) {
        // Print offset
        printf("%08zx: ", i);

        // Print hex values
        for (j = 0; j < 16; j++) {
            if (i + j < size) {
                printf("%02x ", data[i + j]);
            }
            else {
                printf("   "); // Padding for incomplete lines
            }

            // Add extra space after 8 bytes for readability
            if (j == 7) printf(" ");
        }

        // Print ASCII representation
        printf(" |");
        for (j = 0; j < 16 && (i + j) < size; j++) {
            unsigned char c = data[i + j];
            // Print printable characters, use '.' for non-printable
            printf("%c", (c >= 32 && c <= 126) ? c : '.');
        }
        printf("|\n");
    }
}