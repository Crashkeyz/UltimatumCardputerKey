#ifndef CARDPUTER_ADV_DRIVER_H
#define CARDPUTER_ADV_DRIVER_H

#include <SD.h>
#include <FS.h>

void initialize_driver();
bool initialize_sd_card();
void driver_loop();

#endif // CARDPUTER_ADV_DRIVER_H
