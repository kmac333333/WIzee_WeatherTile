#pragma once
#include    "FS.h"

#define LITTLEFS LittleFS

extern void initFS(void);

extern void listDir(fs::FS& fs, const char* dirname, uint8_t levels);
extern void writeFile(fs::FS& fs, const char* path, const char* message);
extern void deleteFile(fs::FS& fs, const char* path);
extern void writeFile2(fs::FS& fs, const char* path, const uint8_t* buf, size_t size);
extern void readFile2(fs::FS& fs, const char* path, uint8_t* buf, size_t size);
