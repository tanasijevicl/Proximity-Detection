#include <stdint.h>

#ifndef SENSOR_H_
#define SENSOR_H_

extern void psReset();

extern void psInit();

void psReadData();

void bin2bcd(const uint16_t number);

extern void psSendData();

#endif /* SENSOR_H_ */
