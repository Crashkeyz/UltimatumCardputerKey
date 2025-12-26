#ifndef CARDPUTER_ADV_DRIVER_H
#define CARDPUTER_ADV_DRIVER_H

#include <SD.h>
#include <FS.h>

void initialize_driver();
void driver_loop();
bool initialize_sd_card();

#endif // CARDPUTER_ADV_DRIVER_H
