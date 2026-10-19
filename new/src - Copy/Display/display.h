#ifndef DISPLAY_H
#define DISPLAY_H

#include "roaster_config.h"

void initDisplay();
void setWifiIP();
void setDisplay(const char* title, float bt, float et, float setpoint, const char* mode);

#endif // DISPLAY_H
