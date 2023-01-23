#ifndef Settings_h
#define Settings_h

#include "stdlib.h"

#define SETTINGS_KEY 11917

class Settings
{
public:
    static Settings *shared;
    uint8_t channel = 16; //Default
    static void init();
    void read();
private:
    int getFirstEmptyPage();
    void save(bool format);
};

#endif