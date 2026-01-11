/*******************************************************************
*  include files
*******************************************************************/
#include    "FS.h"
#include    <LittleFS.h>
#include    "storage.h"
#include    "model2.h"
/*******************************************************************
*  imports
*******************************************************************/

#define FORMAT_LITTLEFS_IF_FAILED true
/*******************************************************************
*  forward references
*******************************************************************/

/*******************************************************************
*  local data
*******************************************************************/

/*******************************************************************
*  support functions
*******************************************************************/
void initFS(void)
{
    if (!LITTLEFS.begin(FORMAT_LITTLEFS_IF_FAILED)) {
        Serial.println("LITTLEFS Mount Failed");
        return;
    }
}
/*******************************************************************
*  void deleteFile(fs::FS& fs, const char* path) 
*******************************************************************/
void deleteFile(fs::FS& fs, const char* path) 
{
    Serial.printf("Deleting file: %s\r\n", path);
    if (fs.remove(path)) {
        Serial.println("- file deleted");
    }
    else {
        Serial.println("- delete failed");
    }
}
/*******************************************************************
*  void writeFile2(fs::FS& fs, const char* path)
*******************************************************************/
void writeFile2(fs::FS& fs, const char* path, const uint8_t* buf, size_t size)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;  
    }
    //const uint8_t *buf, size_t size
    if (file.write(buf, size)) {
        Serial.println("- file written");
    }
    else {
        Serial.println("- write failed");
    }
    file.close();
}
/*******************************************************************
*  void readFile2(fs::FS& fs, const char* path)
*******************************************************************/
void readFile2(fs::FS& fs, const char* path, uint8_t* buf, size_t size)
{
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path, FILE_READ);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;
    }
    //const uint8_t *buf, size_t size
    if (file.read(buf, size)) {
        Serial.println("- file read");
    }
    else {
        Serial.println("- write failed");
    }
    file.close();
}
/*******************************************************************
*  void writeFile(fs::FS& fs, const char* path, const char* message)
*******************************************************************/
void writeFile(fs::FS& fs, const char* path, const char* message)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message)) {
        Serial.println("- file written");
    }
    else {
        Serial.println("- write failed");
    }
    file.close();
}
/*******************************************************************
*  void listDir(fs::FS& fs, const char* dirname, uint8_t levels) 
*******************************************************************/
void listDir(fs::FS& fs, const char* dirname, uint8_t levels) 
{
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs.open(dirname);
    if (!root) {
        Serial.println("- failed to open directory");
        return;
    }
    if (!root.isDirectory()) {
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while (file) {
        if (file.isDirectory()) {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if (levels) {
                listDir(fs, file.name(), levels - 1);
            }
        }
        else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}